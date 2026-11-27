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
    SERIES,
    bundle_available,
    compute_memory,
    startup_text,
)
from keil_uvprojx import parse_uvprojx  # noqa: E402

ALL_CHIPS = ["ing918", "ing916", "ing20"]


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
    # sec_fota_utils.c 用到 uECC_* 与 calc_sha_256，需要预编译的 uECC 库
    "sec_fota_utils.c": "src/Tools/{chip}_uecc.lib",
}


def extra_libs(sdk: Path, proj_dir: Path, info: dict, name: str, chip: str) -> list[Path]:
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

    # 工程在 .uvprojx 里直接引用的预编译对象/库（adc_cali.o、ing<chip>_fota_client.o、
    # ing<chip>_uecc.lib 等）。文件名带芯片名的只在对应芯片下使用。
    import re as _re
    for obj in info.get("objects", []):
        m = _re.search(r"(ing918|ing916|ing20)", obj.name)
        if m and m.group(1) != chip:
            continue
        if obj.is_file() and obj not in libs:
            libs.append(obj)

    # 兜底：src/ 下按芯片命名的 .o（部分工程未在 uvprojx 里显式列出）
    for cand in sorted((proj_dir / "src").glob(f"*{chip}*.o")):
        if cand.is_file() and cand not in libs:
            libs.append(cand)
    return libs


DEFAULT_CONFIG = ".github/project-matrix.yaml"


def load_matrix_config(path: Path) -> dict:
    """读 .github/project-matrix.yaml。

    返回 {"defaults": {...}, "projects": {name: {...}}}。
    文件不存在时返回"全部工程 × 全芯片 × gnu90"的默认配置。
    """
    fallback = {
        "defaults": {"chips": list(ALL_CHIPS), "c_standard": "gnu90"},
        "projects": {},
        "path": None,
    }
    if not path.is_file():
        return fallback

    import yaml
    data = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
    defaults = dict(data.get("defaults") or {})
    defaults.setdefault("chips", list(ALL_CHIPS))
    defaults.setdefault("c_standard", "gnu90")
    return {
        "defaults": defaults,
        "projects": data.get("projects") or {},
        "path": str(path),
    }


def read_startup_sizes(proj_dir: Path, chip: str) -> tuple[int | None, int | None]:
    """从工程自带的 startup_*.s 里读 Heap_Size / Stack_Size。

    Keil 的启动文件用 ARMCC 语法（`Heap_Size EQU 0x0`），很多工程堆是 0。
    我们的模板默认 8 KB 堆，对这些工程会撑爆 RAM，所以必须用真实值。
    """
    import re
    src = proj_dir / "src"
    if not src.is_dir():
        return (None, None)
    for f in sorted(src.glob("startup_*.s")) + sorted(src.glob("gstartup_*.s")):
        try:
            text = f.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        heap = re.search(r"Heap_Size\s+EQU\s+(0x[0-9A-Fa-f]+|\d+)", text)
        stack = re.search(r"Stack_Size\s+EQU\s+(0x[0-9A-Fa-f]+|\d+)", text)
        if heap or stack:
            return (int(heap.group(1), 0) if heap else None,
                    int(stack.group(1), 0) if stack else None)
    return (None, None)


def read_ingw(proj_dir: Path) -> tuple[str | None, str | None]:
    """读 .ingw 的 (bundle, series)。"""
    ingw = next(iter(sorted(proj_dir.glob("*.ingw"))), None)
    if ingw is None:
        return (None, None)
    try:
        data = json.loads(ingw.read_text(encoding="utf-8"))
        return (data.get("bundle") or None, data.get("series") or None)
    except (json.JSONDecodeError, OSError):
        return (None, None)


def read_bundle(proj_dir: Path) -> str | None:
    return read_ingw(proj_dir)[0]


def resolve_series(ingw_series: str | None, chip: str) -> str:
    """确定链接用的 series。

    工程在 .ingw 里指定了自己的 series（ING9187xx / ING9188xx 是两颗不同的
    ROM，符号表与内存布局都不同）。只有当它属于目标芯片家族时才沿用；
    跨家族（例如 918 工程测 916）时退回该家族的默认 series。
    """
    default = FAMILY_SERIES[chip]
    if ingw_series and ingw_series in SERIES and SERIES[ingw_series]["family"] == chip:
        return ingw_series
    return default


def render_cmakelists(name: str, info: dict, sdk: Path, chip: str, bundle: str,
                      startup: Path, ldscript: Path,
                      libs: list[Path] | None = None,
                      c_standard: str = "gnu90",
                      series: str = "") -> str:
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

    # 工程用 -Diprintf=platform_printf 把 printf 重定向到 ROM 函数时，
    # newlib 的 stdio.h 会把 int iprintf() 一起重命名，与 SDK 的
    # void platform_printf() 返回类型冲突。定义 __STRICT_ANSI__ 让 newlib
    # 收起这些 POSIX/MISC 声明即可（ARMCC 的 stdio.h 本就没有 iprintf）。
    if any(d.startswith("iprintf=") for d in info["defines"]):
        lib_section += "target_compile_definitions(" + name + " PRIVATE __STRICT_ANSI__)" + "\n"

    if lib_block:
        lib_section += ("target_link_libraries(" + name + " PRIVATE " + "\n"
                         + lib_block + ")" + "\n")

    return f"""# 由 tools/gen_all_cmake.py 自动生成，请勿手工修改。
# 源工程：{info['device'] or 'unknown'} / Target "{info['name']}" / chip={chip} / bundle={bundle}
# 用途：CI 编译验证。产物目录 build/ 已在 .gitignore 中。
cmake_minimum_required(VERSION 3.20)
project({name} LANGUAGES C CXX ASM)

# 该工程的 C 标准来自 .github/project-matrix.yaml
set(ING_C_STANDARD {c_standard} CACHE STRING "C 标准")
# 链接用的 series 取自工程 .ingw（ING9187xx 与 ING9188xx 是两颗不同的 ROM）
set(ING_SERIES {series} CACHE STRING "芯片系列")

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
                 out_root: Path, ld_template: Path,
                 c_standard: str = "gnu90") -> dict:
    name = proj_dir.name
    ingw_bundle, ingw_series = read_ingw(proj_dir)
    series = resolve_series(ingw_series, chip)
    entry = {"project": name, "chip": chip, "series": series, "status": "skip", "reason": ""}

    bundle = ingw_bundle
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
    libs = extra_libs(sdk, proj_dir, info, name, chip)

    out_dir = out_root / name / chip
    out_dir.mkdir(parents=True, exist_ok=True)

    heap_size, stack_size = read_startup_sizes(proj_dir, chip)
    startup = out_dir / f"gstartup_{mem['family']}00.s"
    startup.write_bytes(startup_text(mem["family"], heap_size, stack_size)
                        .replace("\n", "\r\n").encode("utf-8"))

    ldscript = out_dir / f"{name}.ld"
    ldscript.write_text(render_ld(mem, ld_template), encoding="utf-8")

    (out_dir / "CMakeLists.txt").write_text(
        render_cmakelists(name, info, sdk, chip, bundle, startup, ldscript, libs,
                          c_standard, series),
        encoding="utf-8")

    entry.update({
        "status": "generated",
        "c_standard": c_standard,
        "bundle": bundle,
        "sources": len(info["sources"]),
        "libs": [l.name for l in libs],
        "heap": heap_size,
        "stack": stack_size,
        "heap": heap_size,
        "stack": stack_size,
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
    effective_cstd = entry.get("c_standard") or c_standard
    if effective_cstd:
        cfg.append(f"-DING_C_STANDARD={effective_cstd}")

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
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="工程测试矩阵配置，默认 .github/project-matrix.yaml")
    ap.add_argument("--probe", action="store_true", help="生成后立即逐个编译")
    ap.add_argument("--toolchain", default="arm-none-eabi",
                    choices=["arm-none-eabi", "atfe"], help="探测用工具链")
    ap.add_argument("--cmsis", default=None, help="CMSIS Core 目录（探测用）")
    ap.add_argument("--gcc-root", default=None, help="GNU 工具链根目录（探测用）")
    ap.add_argument("--c-standard", default="gnu90",
                    help="探测用的 C 标准，默认 gnu90（对齐 Keil4）")
    ap.add_argument("--force-c-standard", default=None,
                    help="强制覆盖所有工程的 C 标准（ATfE 通道用 c99，"
                         "因为 CMSIS 6 的 cmsis_gcc.h 用了 C99 的循环内声明）")
    ap.add_argument("--group", type=int, default=None, help="只探测第 N 片（0 起）")
    ap.add_argument("--groups", type=int, default=1, help="总片数，默认 1（不分片）")
    args = ap.parse_args()

    sdk = Path(args.sdk).resolve() if args.sdk else Path(__file__).resolve().parent.parent
    out_root = (sdk / args.out).resolve() if not Path(args.out).is_absolute() else Path(args.out)
    out_root.mkdir(parents=True, exist_ok=True)

    ld_template = sdk / "examples-gcc" / "get_started" / "get_started.ld"
    cli_chips = [c.strip() for c in args.chips.split(",") if c.strip()]
    for c in cli_chips:
        if c not in FAMILY_SERIES:
            raise SystemExit(f"未知芯片：{c}，可选 {sorted(FAMILY_SERIES)}")

    cfg_path = Path(args.config)
    if not cfg_path.is_absolute():
        cfg_path = sdk / cfg_path
    cfg = load_matrix_config(cfg_path)
    defaults = cfg["defaults"]
    projects_cfg = cfg["projects"]

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
    print(f"配置     : {cfg['path'] or '(未找到，使用默认: 全工程 × 全芯片 × gnu90)'}")
    print(f"默认     : chips={defaults['chips']} c_standard={defaults['c_standard']}")
    print(f"工程     : {len(projects)} 个")
    print("-" * 70)

    existing = {p.name for p in (sdk / "examples").iterdir() if p.is_dir()}
    for name in sorted(set(projects_cfg) - existing):
        print(f"提示：配置中的 {name} 在 examples/ 下不存在")

    combos = []
    for proj_dir, uvprojx in projects:
        pc = projects_cfg.get(proj_dir.name) or {}
        if pc.get("skip"):
            combos.append({
                "project": proj_dir.name,
                "chip": "-",
                "status": "excluded",
                "reason": pc.get("note") or "配置中标记为 skip",
            })
            continue

        # 某些工程只在某一工具链下无解（如 ATfE 的 platform_printf 冲突）
        if args.toolchain == "atfe" and pc.get("atfe_skip"):
            combos.append({
                "project": proj_dir.name, "chip": "-", "status": "excluded",
                "reason": "ATfE 通道跳过：" + (pc.get("note") or "").replace("\n", " "),
            })
            continue

        chips_for = [c for c in (pc.get("chips") or defaults["chips"]) if c in cli_chips]
        cstd = args.force_c_standard or pc.get("c_standard") or defaults["c_standard"]
        if not chips_for:
            combos.append({
                "project": proj_dir.name, "chip": "-", "status": "excluded",
                "reason": "配置的芯片与 --chips 无交集",
            })
            continue
        for chip in chips_for:
            combos.append(generate_one(sdk, proj_dir, uvprojx, chip, out_root,
                                       ld_template, cstd))

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
        "chips": defaults["chips"],
        "config": cfg["path"],
        "force_c_standard": args.force_c_standard,
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
