#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ingw_switch_chip.py —— 按 ingWizard 的切芯片逻辑更新 ING SDK 示例工程。

复刻 ingWizard 的行为（data/common.cs、gcc.cs、gcc.fix，
以及 checkprojform.pas 的 ConfigProjRam / app_rom_* 计算）。

面向 CI/CD 的编译验证：只处理"决定能否按目标芯片编过"的内容。

  默认处理：
  * 启动文件按 family 命名          src/gstartup_<family>00.s（缺失则生成）
  * 链接脚本的 FLASH/RAM            按 bundle meta + series.json 重算
  * CMake 工程                      生成 <proj>_<family>.ld（并存，供矩阵构建）
  * .ingw 元数据                    series / family / bundle_meta 同步

  显式开启：
  * --with-makefile                 改写 makefile.conf 的 ARCH_FLAGS、
                                    Makefile 的 ING_BUNDLE/SYMDEFS 与 <proj>.ld
  * --with-flash-ini                同步 flash_download.ini（烧录配置，CI 不需要）

内存算法与 checkprojform.pas 完全一致：

    app_rom_addr = meta.app.base
    app_rom_size = series.flash_base + series.flash_size - app_rom_addr
    app_ram_addr = align8(meta.ram.base + meta.ram.size)
    app_ram_size = meta.ram.base + sys_ram - app_ram_addr
    sys_ram      = series.sys_ram[bundle] or series.def_sys_ram or series.ram_size // 2

用法：
    python tools/ingw_switch_chip.py --project examples-gcc/get_started --series ING9168xx
    python tools/ingw_switch_chip.py --project examples-gcc/get_started --series ING9187xx --dry-run
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from pathlib import Path

from ing_chip import (  # 同目录模块，运行时 sys.path[0] 即 tools/
    ARCH_FLAGS,
    HEAP_SIZE,
    SERIES,
    align,
    compute_memory,
    startup_text,
    sys_ram_of,
)

# 芯片系列参数、内存算法与 GNU 启动文件模板统一维护在 tools/ing_chip.py。

# ---------------------------------------------------------------------------
# 文件读写
# ---------------------------------------------------------------------------
def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="surrogateescape")


def write_text(path: Path, text: str) -> None:
    """保持原有行尾符写回。"""
    raw = path.read_bytes() if path.exists() else b""
    crlf = b"\r\n" in raw
    data = text.replace("\r\n", "\n")
    if crlf:
        data = data.replace("\n", "\r\n")
    path.write_bytes(data.encode("utf-8", errors="surrogateescape"))


# ---------------------------------------------------------------------------
# 链接脚本
# ---------------------------------------------------------------------------
RE_FLASH = re.compile(
    r"([ \t]*FLASH\s*\(rx\)\s*:\s*ORIGIN\s*=\s*)(\d+)(\s*,\s*LENGTH\s*=\s*)(\d+)")
RE_RAM = re.compile(
    r"([ \t]*RAM\s*\(rwx\)\s*:\s*ORIGIN\s*=\s*)(\d+)(\s*,\s*LENGTH\s*=\s*)(\d+)")


def update_ld(path: Path, mem: dict, apply: bool) -> list[str]:
    if not path.is_file():
        return [f"跳过（不存在）：{path.name}"]
    text = read_text(path)

    new, n1 = RE_FLASH.subn(
        lambda m: f"{m.group(1)}{mem['rom_addr']}{m.group(3)}{mem['rom_size']}", text, count=1)
    new, n2 = RE_RAM.subn(
        lambda m: f"{m.group(1)}{mem['ram_addr']}{m.group(3)}{mem['ram_size']}", new, count=1)

    if n1 != 1 or n2 != 1:
        return [f"警告：{path.name} 的 MEMORY 段未按预期匹配（FLASH={n1} RAM={n2}）"]

    if apply:
        write_text(path, new)
    return [f"{path.name}: FLASH 0x{mem['rom_addr']:X}/{mem['rom_size']} "
            f"RAM 0x{mem['ram_addr']:X}/{mem['ram_size']}"]


def ensure_ld(path: Path, template: Path | None, mem: dict, apply: bool) -> list[str]:
    """目标链接脚本不存在时先从模板复制，再改写 MEMORY 段。"""
    if not path.is_file():
        if template is None or not template.is_file():
            return [f"跳过（不存在且无模板）：{path.name}"]
        if not apply:
            return [f"{path.name}: 将基于 {template.name} 生成，"
                    f"FLASH 0x{mem['rom_addr']:X}/{mem['rom_size']} "
                    f"RAM 0x{mem['ram_addr']:X}/{mem['ram_size']}"]
        shutil.copyfile(template, path)
    return update_ld(path, mem, apply)


# ---------------------------------------------------------------------------
# Makefile / makefile.conf（仅在 --with-makefile 时使用）
# ---------------------------------------------------------------------------
def update_makefile_conf(path: Path, family: str, old_family: str, apply: bool) -> list[str]:
    if not path.is_file():
        return [f"跳过（不存在）：{path.name}"]
    text = read_text(path)
    flags = ARCH_FLAGS[family]
    new, n = re.subn(r"^ARCH_FLAGS=.*$", f"ARCH_FLAGS={flags}", text, count=1, flags=re.M)
    if n != 1:
        return [f"警告：{path.name} 未找到 ARCH_FLAGS 行"]
    if old_family and old_family != family:
        new = new.replace(old_family, family)
    if apply:
        write_text(path, new)
    return [f"{path.name}: ARCH_FLAGS={flags}"]


def update_makefile(path: Path, series: str, bundle: str, sdk_rel: str, apply: bool) -> list[str]:
    if not path.is_file():
        return [f"跳过（不存在）：{path.name}"]
    text = read_text(path)
    notes = []
    rules = [
        (r"^ING_BUNDLE=.*$", f"ING_BUNDLE={sdk_rel}/bundles/{bundle}/inc"),
        (r"^SYMDEFS=.*$",
         f"SYMDEFS=-Wl,--just-symbols={sdk_rel}/bundles/{bundle}/{series}/symdefs.g"),
    ]
    for pattern, repl in rules:
        text, n = re.subn(pattern, repl, text, count=1, flags=re.M)
        if n != 1:
            notes.append(f"警告：{path.name} 未匹配 {pattern}")
    if apply:
        write_text(path, text)
    notes.append(f"{path.name}: ING_BUNDLE/SYMDEFS -> {bundle}/{series}")
    return notes


# ---------------------------------------------------------------------------
# flash_download.ini（仅在 --with-flash-ini 时使用；CI 不需要）
# ---------------------------------------------------------------------------
def _ini_set(text: str, section: str, key: str, value: str) -> tuple[str, int]:
    """在 INI 指定节里设置 key=value，其余行原样保留（含行尾）。"""
    lines = text.splitlines(keepends=True)
    sec_re = re.compile(r"^\s*\[([^\]]+)\]\s*$")
    key_re = re.compile(r"^(\s*" + re.escape(key) + r"\s*=\s*)(.*?)(\r?\n)?$")
    out: list[str] = []
    in_sec = False
    done = False

    for line in lines:
        m = sec_re.match(line)
        if m:
            if in_sec and not done:
                out.append(f"{key}={value}\n")
                done = True
            in_sec = m.group(1).strip().lower() == section.lower()
            out.append(line)
            continue
        if in_sec and not done:
            km = key_re.match(line)
            if km:
                out.append(f"{km.group(1)}{value}{km.group(3) or ''}")
                done = True
                continue
        out.append(line)

    if in_sec and not done:
        out.append(f"{key}={value}\n")
        done = True
    return "".join(out), (1 if done else 0)


def update_flash_ini(path: Path, series: str, bundle: str, sdk_rel: str,
                     mem: dict, family: str, apply: bool) -> list[str]:
    if not path.is_file():
        return [f"跳过（不存在）：{path.name}"]
    text = read_text(path)
    notes = []

    platform = f"{sdk_rel}/bundles/{bundle}/{series}/platform.bin"
    subs = [
        ("main", "family", family),
        ("bin-0", "FileName", platform),
        ("bin-0", "Address", str(mem["meta"]["rom"]["base"])),
        ("bin-1", "Address", str(mem["rom_addr"])),
    ]
    for section, key, value in subs:
        text, n = _ini_set(text, section, key, value)
        if not n:
            notes.append(f"提示：{path.name} 未找到 [{section}] {key}")

    if apply:
        write_text(path, text)
    notes.append(f"{path.name}: family={family}, platform.bin -> {series}")
    return notes


# ---------------------------------------------------------------------------
# .ingw 元数据（只同步编译相关的 series/family/bundle_meta）
# ---------------------------------------------------------------------------
def update_ingw(path: Path, series: str, mem: dict, apply: bool,
                with_flash_fields: bool) -> list[str]:
    if not path.is_file():
        return [f"跳过（不存在）：{path.name}"]
    data = json.loads(path.read_text(encoding="utf-8"))
    data["series"] = series
    data["family"] = mem["family"]
    data["bundle_meta"] = mem["meta"]
    if with_flash_fields:
        data["app_rom_addr"] = mem["rom_addr"]
        data["app_rom_size"] = mem["rom_size"]
        data["app_ram_addr"] = mem["ram_addr"]
        data["app_ram_size"] = mem["ram_size"]
    if apply:
        path.write_text(json.dumps(data, ensure_ascii=False, indent=1) + "\n", encoding="utf-8")
    return [f"{path.name}: series={series} family={mem['family']}"]


# ---------------------------------------------------------------------------
# 启动文件
# ---------------------------------------------------------------------------
def ensure_startup(src_dir: Path, family: str, apply: bool) -> list[str]:
    target = src_dir / f"gstartup_{family}00.s"
    if target.is_file():
        return [f"{target.name}: 已存在，未改动"]

    arch = "armv7e-m" if family == "ing916" else "armv7-m"
    fpu = "    .fpu    fpv4-sp-d16\n" if family == "ing916" else ""
    content = STARTUP_TEMPLATE.format(arch=arch, fpu=fpu, family=family,
                                      heap_size=HEAP_SIZE[family])
    if apply:
        target.write_bytes(content.replace("\n", "\r\n").encode("utf-8"))
    verb = "已生成" if apply else "将生成"
    return [f"{target.name}: {verb}（arch={arch}, heap={HEAP_SIZE[family]}）"]


def prune_old_startup(src_dir: Path, family: str, apply: bool) -> list[str]:
    """把其他 family 的 GCC 启动文件改名为 .bak（ingWizard 的 rename 语义）。"""
    notes = []
    for path in sorted(src_dir.glob("gstartup_*.s")):
        if path.name == f"gstartup_{family}00.s":
            continue
        backup = path.with_suffix(".s.bak")
        if apply:
            shutil.move(str(path), str(backup))
        notes.append(f"{path.name} -> {backup.name}")
    return notes


# ---------------------------------------------------------------------------
# 主流程
# ---------------------------------------------------------------------------
def main() -> int:
    parser = argparse.ArgumentParser(
        description="按 ingWizard 的逻辑切换 ING SDK 示例工程的目标芯片（面向编译验证）")
    parser.add_argument("--project", required=True,
                        help="示例工程目录，如 examples-gcc/get_started")
    parser.add_argument("--series", required=True, choices=sorted(SERIES),
                        help="目标芯片系列")
    parser.add_argument("--sdk", default=None, help="SDK 根目录，默认按脚本位置推断")
    parser.add_argument("--bundle", default=None, help="bundle 名，默认取 .ingw 或 typical")
    parser.add_argument("--dry-run", action="store_true", help="只打印计划，不写文件")
    parser.add_argument("--with-makefile", action="store_true",
                        help="同时改写 Makefile/makefile.conf 与 <proj>.ld；"
                             "默认不改，以免破坏其他芯片的既有构建基线")
    parser.add_argument("--with-ingw", action="store_true",
                        help="同步 .ingw 的 series/family/bundle_meta；"
                             "默认不改，避免与未同步的 Makefile 配置冲突")
    parser.add_argument("--with-flash-ini", action="store_true",
                        help="同步 flash_download.ini（烧录配置，CI 编译验证不需要）")
    parser.add_argument("--prune-startup", action="store_true",
                        help="把其他 family 的 gstartup_*.s 改名为 .bak；默认保留以便多芯片并存")
    args = parser.parse_args()

    sdk_root = Path(args.sdk).resolve() if args.sdk else Path(__file__).resolve().parent.parent
    project = Path(args.project).resolve()
    if not project.is_dir():
        raise SystemExit(f"工程目录不存在：{project}")

    apply = not args.dry_run

    ingw_files = sorted(project.glob("*.ingw"))
    ingw = ingw_files[0] if ingw_files else None
    old_data = json.loads(ingw.read_text(encoding="utf-8")) if ingw else {}
    old_series = old_data.get("series", "")
    old_family = old_data.get("family") or SERIES.get(old_series, {}).get("family", "")
    bundle = args.bundle or old_data.get("bundle") or "typical"
    sdk_rel = old_data.get("sdk_release_path", "../..")

    mem = compute_memory(sdk_root, args.series, bundle)
    family = mem["family"]

    print(f"工程    : {project}")
    print(f"bundle  : {bundle}")
    print(f"切换    : {old_series or '(未知)'} -> {args.series}  "
          f"(family {old_family or '?'} -> {family})")
    print(f"FLASH   : 0x{mem['rom_addr']:08X}  size {mem['rom_size']} (0x{mem['rom_size']:X})")
    print(f"RAM     : 0x{mem['ram_addr']:08X}  size {mem['ram_size']} (0x{mem['ram_size']:X})")
    print(f"模式    : {'dry-run' if args.dry_run else 'apply'}")
    print("-" * 62)

    notes: list[str] = []

    if args.with_makefile and ((project / "Makefile").is_file()
                               or (project / "makefile.conf").is_file()):
        notes += update_makefile_conf(project / "makefile.conf", family, old_family, apply)
        notes += update_makefile(project / "Makefile", args.series, bundle, sdk_rel, apply)
        notes += update_ld(project / f"{project.name}.ld", mem, apply)

    # CMake 工程：每个 family 一份链接脚本并存，供 CI 矩阵使用
    if (project / "CMakeLists.txt").is_file():
        notes += ensure_ld(project / f"{project.name}_{family}.ld",
                           project / f"{project.name}.ld", mem, apply)

    src_dir = project / "src"
    if src_dir.is_dir():
        notes += ensure_startup(src_dir, family, apply)
        if args.prune_startup and old_family and old_family != family:
            notes += prune_old_startup(src_dir, family, apply)

    if args.with_flash_ini:
        notes += update_flash_ini(project / "flash_download.ini", args.series, bundle,
                                  sdk_rel, mem, family, apply)

    if ingw and args.with_ingw:
        notes += update_ingw(ingw, args.series, mem, apply, args.with_flash_ini)

    for line in notes:
        print("  " + line)

    print("-" * 62)
    if args.dry_run:
        print("dry-run：未写入任何文件。去掉 --dry-run 以应用。")
    else:
        print("完成。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
