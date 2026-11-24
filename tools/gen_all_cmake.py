#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_all_cmake.py —— 为仓库里所有 Keil 工程批量生成 CMake 工程。

用途：让 CI 用 GCC（以及后续的 ATfE）编译 examples/ 下的全部工程，
并对每个工程分别按 ing918 / ing916 / ing20 生成，从而验证"切芯片也能编过"。

设计要点：
  * 只读仓库：所有产物写到 --out 指定的目录（默认 build/gen），不修改
    examples/ 下任何工程文件
  * 启动文件与链接脚本按芯片现场生成（Keil 工程的 .s 是 ARMCC 语法、
    链接信息在 .sct 里，都不能直接给 GCC 用）
  * 判定不了的组合（bundle 缺失、无 .ingw）写进 manifest 的 skip 项，
    由 CI 与 baseline 对比，不算失败
  * platform_companion 是 ROM 配套固件，链接布局特殊，单独处理

用法：
    python tools/gen_all_cmake.py --out build/gen
    python tools/gen_all_cmake.py --out build/gen --chips ing918 --filter 'peripheral_*'
    python tools/gen_all_cmake.py --out build/gen --probe      # 生成后立即逐个编译
"""

from __future__ import annotations

import argparse
import fnmatch
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from ing_chip import (  # noqa: E402
    FAMILY_SERIES,
    bundle_available,
    compute_memory,
    startup_text,
)
from keil_uvprojx import parse_uvprojx  # noqa: E402

ALL_CHIPS = ["ing918", "ing916", "ing20"]

# 需要特殊处理的工程：没有 .ingw、链接布局不走 bundle 约定
SPECIAL_PROJECTS = {
    "platform_companion": "ROM 配套固件，链接布局与 APP 不同（见 platform_companion_*.ld）",
}

# 明确排除：不参与跨芯片编译验证
EXCLUDED_PROJECTS = {
    # CherryUSB 的源码要求高于 C99 的 C 标准，Keil4 工程本身不支持；
    # 且 USB 只有 916/20 有，918 无此硬件。
    "cherry_usb": "CherryUSB 不支持 Keil4 的 C99 约束；USB 仅 916/20 有",
}


def to_sdk_path(p: Path, sdk: Path) -> str:
    """SDK 内路径写成 ${ING_SDK_ROOT}/...，SDK 外退回绝对路径。"""
    try:
        rel = Path(p).resolve().relative_to(sdk)
        return "${ING_SDK_ROOT}/" + rel.as_posix()
    except ValueError:
        return Path(p).resolve().as_posix()


# Keil 工程引用的是 ARMCC 专用实现，GCC 需要换成等价目录。
# 只有目标文件确实存在时才替换，否则保留原路径以便暴露问题。
SOURCE_REWRITES = [
    ("/portable/RVDS/", "/portable/GCC/"),
    ("/portable/RVDS\\", "/portable/GCC\\"),
    ("context_rvds.S", "context_gcc.S"),
    ("context_rvds.s", "context_gcc.s"),
]


def rewrite_sources(sources: list[Path]) -> list[Path]:
    out: list[Path] = []
    for src in sources:
        text = src.as_posix()
        replaced = None
        for a, b in SOURCE_REWRITES:
            if a in text:
                cand = Path(text.replace(a, b))
                if cand.is_file():
                    replaced = cand
                    break
        out.append(replaced or src)
    return out


# 预编译库：工程名 -> 额外需要链接的库（相对 SDK）
PROJECT_LIBS = {
    "mesh-light": ["src/SigMesh/SigMesh.lib"],
    "peripheral_console": ["src/Tools/power_ctrl.lib"],
}

# 源文件特征 -> 额外库（{chip} 会按目标芯片展开）
SOURCE_LIBS = {
    "ecc_driver.c": "src/Tools/{chip}_uecc.lib",
    "fota_client.zig": "src/Tools/{chip}_uecc.lib",
}


def extra_libs(sdk: Path, info: dict, name: str, chip: str) -> list[Path]:
    libs: list[Path] = []
    for rel in PROJECT_LIBS.get(name, []):
        cand = sdk / rel
        if cand.is_file():
            libs.append(cand)
    source_names = {p.name for p in info["sources"]}
    for marker, rel in SOURCE_LIBS.items():
        if marker in source_names:
            cand = sdk / rel.format(chip=chip)
            if cand.is_file() and cand not in libs:
                libs.append(cand)
    return libs


def read_bundle(proj_dir: Path) -> str | None:
    ingw = next(iter(sorted(proj_dir.glob("*.ingw"))), None)
    if ingw is None:
        return None
    try:
        return json.loads(ingw.read_text(encoding="utf-8")).get("bundle") or None
    except (json.JSONDecodeError, OSError):
        return None


def render_cmakelists(name: str, info: dict, sdk: Path, chip: str, bundle: str,
                      startup: Path, ldscript: Path,
                      libs: list[Path] | None = None) -> str:
    sources = "\n".join(f"        {to_sdk_path(s, sdk)}" for s in info["sources"])
    includes = "\n".join(f"        {to_sdk_path(i, sdk)}" for i in info["includes"])

    defines = list(info["defines"])
    define_block = ""
    if defines:
        define_block = "    DEFINES\n" + "\n".join(f"        {d}" for d in defines) + "\n"
    include_block = ""
    if includes:
        include_block = "    INCLUDES\n" + includes + "\n"

    lib_block = "\n".join(f"        {to_sdk_path(l, sdk)}" for l in (libs or []))
    lib_section = ")" + "\n"
    if lib_block:
        lib_section += ("target_link_libraries(" + name + " PRIVATE " + "\n"
                         + lib_block + ")" + "\n")

    return f"""# 由 tools/gen_all_cmake.py 自动生成，请勿手工修改。
# 源工程：{info['device'] or 'unknown'} / Target "{info['name']}" / chip={chip} / bundle={bundle}
# 用途：CI 编译验证。产物目录 build/ 已在 .gitignore 中。
cmake_minimum_required(VERSION 3.20)
project({name} LANGUAGES C CXX ASM)

include("{sdk.as_posix()}/cmake/ing_example.cmake")

ing_add_example({name}
    SOURCES
{sources}
    STARTUP
        {to_sdk_path(startup, sdk)}
    LDSCRIPT
        {to_sdk_path(ldscript, sdk)}
    BUNDLE
        {bundle}
{define_block}{include_block}{lib_section}"""


def render_ld(mem: dict, source_ld: Path | None = None) -> str:
    """按内存参数渲染链接脚本。

    以 get_started.ld 为模板（已修好 .heap 在 lld 下的问题），只替换 MEMORY。
    """
    template = source_ld
    if template is None or not template.is_file():
        raise FileNotFoundError("缺少链接脚本模板 get_started.ld")

    text = template.read_text(encoding="utf-8", errors="surrogateescape")
    import re
    text = re.sub(
        r"([ \t]*FLASH\s*\(rx\)\s*:\s*ORIGIN\s*=\s*)\d+(\s*,\s*LENGTH\s*=\s*)\d+",
        lambda m: f"{m.group(1)}{mem['rom_addr']}{m.group(2)}{mem['rom_size']}",
        text, count=1)
    text = re.sub(
        r"([ \t]*RAM\s*\(rwx\)\s*:\s*ORIGIN\s*=\s*)\d+(\s*,\s*LENGTH\s*=\s*)\d+",
        lambda m: f"{m.group(1)}{mem['ram_addr']}{m.group(2)}{mem['ram_size']}",
        text, count=1)
    return text


def generate_one(sdk: Path, proj_dir: Path, uvprojx: Path, chip: str,
                 out_root: Path, ld_template: Path) -> dict:
    name = proj_dir.name
    series = FAMILY_SERIES[chip]
    entry = {"project": name, "chip": chip, "series": series, "status": "skip", "reason": ""}

    if name in EXCLUDED_PROJECTS:
        entry["status"] = "excluded"
        entry["reason"] = EXCLUDED_PROJECTS[name]
        return entry

    if name in SPECIAL_PROJECTS:
        entry["reason"] = SPECIAL_PROJECTS[name]
        entry["status"] = "special"
        return entry

    bundle = read_bundle(proj_dir)
    if not bundle:
        entry["reason"] = "没有 .ingw 或其中没有 bundle 字段"
        return entry

    if not bundle_available(sdk, bundle, series):
        entry["reason"] = f"bundles/{bundle}/{series} 缺少 symdefs.g"
        return entry

    try:
        info = parse_uvprojx(uvprojx)
    except (ValueError, OSError) as exc:
        entry["reason"] = f"解析失败：{exc}"
        return entry

    if not info["sources"]:
        entry["reason"] = "没有解析到 C/C++ 源文件"
        return entry

    try:
        mem = compute_memory(sdk, series, bundle)
    except (FileNotFoundError, ValueError) as exc:
        entry["reason"] = str(exc)
        return entry

    info["sources"] = rewrite_sources(info["sources"])
    libs = extra_libs(sdk, info, name, chip)

    out_dir = out_root / name / chip
    out_dir.mkdir(parents=True, exist_ok=True)

    startup = out_dir / f"gstartup_{mem['family']}00.s"
    startup.write_bytes(startup_text(mem["family"]).replace("\n", "\r\n").encode("utf-8"))

    ldscript = out_dir / f"{name}.ld"
    ldscript.write_text(render_ld(mem, ld_template), encoding="utf-8")

    (out_dir / "CMakeLists.txt").write_text(
        render_cmakelists(name, info, sdk, chip, bundle, startup, ldscript, libs),
        encoding="utf-8")

    entry.update({
        "status": "generated",
        "bundle": bundle,
        "sources": len(info["sources"]),
        "libs": [l.name for l in libs],
        "dir": str(out_dir.relative_to(out_root).as_posix()),
        "rom": f"0x{mem['rom_addr']:X}/{mem['rom_size']}",
        "ram": f"0x{mem['ram_addr']:X}/{mem['ram_size']}",
    })
    return entry


def probe_one(sdk: Path, entry: dict, out_root: Path, toolchain: str,
              cmsis: str | None, gcc_root: str | None,
              c_standard: str | None = None) -> dict:
    """对已生成的组合跑一次 configure + build，记录结果。"""
    build_dir = out_root / entry["dir"] / f"build-{toolchain}"
    tc_file = f"{sdk.as_posix()}/cmake/toolchain-{toolchain}.cmake"

    env = None
    if cmsis:
        import os
        env = dict(os.environ, CMSIS_BASE=cmsis)

    cfg = ["cmake", "-B", str(build_dir), f"--toolchain={tc_file}", "-G", "Ninja",
           f"-DCHIP={entry['chip']}"]
    if gcc_root:
        cfg.append(f"-DARM_GCC_ROOT={gcc_root}")
    if c_standard:
        cfg.append(f"-DING_C_STANDARD={c_standard}")

    try:
        r = subprocess.run(cfg, cwd=str(out_root / entry["dir"]),
                           capture_output=True, text=True, timeout=300, env=env)
        if r.returncode != 0:
            entry["status"] = "configure-fail"
            entry["reason"] = (r.stderr or r.stdout).strip().splitlines()[-1][:200] if (r.stderr or r.stdout) else ""
            return entry
        r = subprocess.run(["cmake", "--build", str(build_dir)],
                           capture_output=True, text=True, timeout=600, env=env)
        if r.returncode != 0:
            entry["status"] = "build-fail"
            lines = [l for l in (r.stdout or r.stderr).splitlines() if "error" in l.lower()]
            entry["reason"] = (lines[0] if lines else "build failed")[:200]
            return entry
        entry["status"] = "ok"
        entry["reason"] = ""
    except subprocess.TimeoutExpired:
        entry["status"] = "timeout"
        entry["reason"] = "configure/build 超时"
    return entry


def main() -> int:
    ap = argparse.ArgumentParser(description="为所有 Keil 工程生成 CMake 工程")
    ap.add_argument("--out", default="build/gen", help="输出目录（默认 build/gen）")
    ap.add_argument("--sdk", default=None, help="SDK 根目录，默认按脚本位置推断")
    ap.add_argument("--chips", default=",".join(ALL_CHIPS),
                    help="目标芯片，逗号分隔，默认 ing918,ing916,ing20")
    ap.add_argument("--filter", default="*", help="工程名通配符，默认全部")
    ap.add_argument("--probe", action="store_true", help="生成后立即逐个编译")
    ap.add_argument("--toolchain", default="arm-none-eabi",
                    choices=["arm-none-eabi", "atfe"], help="探测用工具链")
    ap.add_argument("--cmsis", default=None, help="CMSIS Core 目录（探测用）")
    ap.add_argument("--gcc-root", default=None, help="GNU 工具链根目录（探测用）")
    ap.add_argument("--c-standard", default="gnu90",
                    help="探测用的 C 标准，默认 gnu90（对齐 Keil4）")
    ap.add_argument("--group", type=int, default=None, help="只探测第 N 片（0 起）")
    ap.add_argument("--groups", type=int, default=1, help="总片数，默认 1（不分片）")
    args = ap.parse_args()

    sdk = Path(args.sdk).resolve() if args.sdk else Path(__file__).resolve().parent.parent
    out_root = (sdk / args.out).resolve() if not Path(args.out).is_absolute() else Path(args.out)
    out_root.mkdir(parents=True, exist_ok=True)

    ld_template = sdk / "examples-gcc" / "get_started" / "get_started.ld"
    chips = [c.strip() for c in args.chips.split(",") if c.strip()]
    for c in chips:
        if c not in FAMILY_SERIES:
            raise SystemExit(f"未知芯片：{c}，可选 {sorted(FAMILY_SERIES)}")

    projects = []
    for proj_dir in sorted(p for p in (sdk / "examples").iterdir() if p.is_dir()):
        if not fnmatch.fnmatch(proj_dir.name, args.filter):
            continue
        uvprojx = sorted(proj_dir.glob("*.uvprojx"))
        if not uvprojx:
            continue
        projects.append((proj_dir, uvprojx[0]))

    print(f"SDK      : {sdk}")
    print(f"输出     : {out_root}")
    print(f"芯片     : {', '.join(chips)}")
    print(f"工程     : {len(projects)} 个")
    print("-" * 70)

    combos = []
    for proj_dir, uvprojx in projects:
        for chip in chips:
            entry = generate_one(sdk, proj_dir, uvprojx, chip, out_root, ld_template)
            combos.append(entry)

    if args.probe:
        todo = [e for e in combos if e["status"] == "generated"]
        if args.group is not None:
            if not (0 <= args.group < args.groups):
                raise SystemExit(f"--group 必须在 [0, {args.groups}) 内")
            todo = [e for i, e in enumerate(todo) if i % args.groups == args.group]
            print(f"分片 {args.group}/{args.groups}：本片 {len(todo)} 个组合")
        print(f"开始探测 {len(todo)} 个组合（{args.toolchain}）...")
        for i, entry in enumerate(todo, 1):
            probe_one(sdk, entry, out_root, args.toolchain, args.cmsis,
                      args.gcc_root, args.c_standard)
            mark = "OK  " if entry["status"] == "ok" else "FAIL"
            print(f"  [{i:3d}/{len(todo)}] {mark} {entry['project']:<32} {entry['chip']}"
                  + (f"  {entry['reason']}" if entry["status"] != "ok" else ""))

    manifest = {
        "generated_at": datetime.now(timezone.utc).isoformat(timespec="seconds"),
        "sdk": sdk.as_posix(),
        "chips": chips,
        "toolchain": args.toolchain if args.probe else None,
        "group": args.group,
        "groups": args.groups,
        "combinations": combos,
    }
    manifest_path = out_root / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=1),
                             encoding="utf-8")

    import collections
    stat = collections.Counter(e["status"] for e in combos)
    print("-" * 70)
    print("组合统计：", dict(stat))
    print("manifest：", manifest_path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
