#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""keil2cmake.py —— 把 Keil MDK 工程转成 CMake 工程，供 CI 做编译验证。

定位：**CI 内部工具**，不是通用转换器，也不是 ingWizard / KeilFormat 的一部分。
它只做一件事：读 .uvprojx 的源文件、include、宏定义，生成一个调用
`cmake/ing_example.cmake` 的 CMakeLists.txt，让 CI 能用 GCC / ATfE 编译同一批源码，
从而在"改了代码导致 GCC 或 clang 编不过"时立刻暴露。

不修改仓库里的任何 Keil 工程；产物只写到 --out 指定的目录。

启动文件与链接脚本不在 Keil 工程里（Keil 用 .sct），因此需要配套的
`src/gstartup_<chip>00.s` 与 `<proj>_<chip>.ld`。缺失时可先用
`tools/ingw_switch_chip.py` 生成。

用法：
    python .github/scripts/keil2cmake.py \
        --project examples/peripheral_ibeacon/peripheral_ibeacon.uvprojx \
        --chip ing918 --out build/keil-import

生成的 <out>/<proj>/CMakeLists.txt 可这样构建：
    cmake -S <out>/<proj> -B <out>/<proj>/build \
          --toolchain=cmake/toolchain-arm-none-eabi.cmake -DCHIP=ing918
"""

from __future__ import annotations

import argparse
import os
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Keil 工程里这些文件由 SDK 的 GNU 版本替代，或本来就不参与编译
# Keil 工程解析统一维护在 tools/keil_uvprojx.py，避免多处实现漂移
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
from keil_uvprojx import parse_uvprojx  # noqa: E402


def to_rel(p: Path, base: Path) -> str:
    """转成相对 SDK 根的 posix 路径；跨盘符时退回绝对路径。"""
    try:
        return Path(os.path.relpath(p, base)).as_posix()
    except ValueError:
        return p.as_posix()


def sdk_path(p: Path, sdk: Path) -> str:
    """SDK 内的路径写成 ${ING_SDK_ROOT}/...，SDK 外退回绝对路径。"""
    rel = to_rel(p, sdk)
    if rel.startswith(".."):
        return p.as_posix()
    return "${ING_SDK_ROOT}/" + rel


def find_asset(sdk: Path, proj_name: str, chip: str, kind: str) -> Path | None:
    """在 examples-gcc/<proj>/ 下找启动文件或链接脚本。"""
    base = sdk / "examples-gcc" / proj_name
    if kind == "startup":
        for cand in (base / "src" / f"gstartup_{chip}00.s",
                     base / "src" / f"startup_{chip}00.s"):
            if cand.is_file():
                return cand
        return None
    for cand in (base / f"{proj_name}_{chip}.ld", base / f"{proj_name}.ld"):
        if cand.is_file():
            return cand
    return None


def render(info: dict, sdk: Path, startup: Path, ldscript: Path, chip: str,
           proj_name: str) -> str:
    rel_sources = "\n".join(f"        {sdk_path(s, sdk)}" for s in info["sources"])
    rel_includes = "\n".join(f"        {sdk_path(inc, sdk)}" for inc in info["includes"])
    rel_defines = "\n".join(f"        {d}" for d in info["defines"])

    include_block = ""
    if rel_includes:
        include_block = f"""    INCLUDES
{rel_includes}
"""
    define_block = ""
    if rel_defines:
        define_block = f"""    DEFINES
{rel_defines}
"""

    cmake_name = proj_name
    return f"""# 由 .github/scripts/keil2cmake.py 自动生成，请勿手工修改。
# 源：{info['device'] or 'unknown device'} / Target "{info['name']}"
# 用途：让 CI 用 GCC 与 ATfE 编译同一批 Keil 源文件，尽早发现编译回归。
cmake_minimum_required(VERSION 3.20)
project({cmake_name} LANGUAGES C CXX ASM)

include("{sdk.as_posix()}/cmake/ing_example.cmake")

ing_add_example({cmake_name}
    SOURCES
{rel_sources}
    STARTUP
        {sdk_path(startup, sdk)}
    LDSCRIPT
        {sdk_path(ldscript, sdk)}
{define_block}{include_block})
"""


def main() -> int:
    ap = argparse.ArgumentParser(description="把 Keil 工程转成 CMake 工程（CI 编译验证用）")
    ap.add_argument("--project", required=True, help=".uvprojx 路径")
    ap.add_argument("--chip", required=True, choices=["ing918", "ing916", "ing20"],
                    help="目标芯片家族")
    ap.add_argument("--out", required=True, help="输出目录")
    ap.add_argument("--sdk", default=None, help="SDK 根目录，默认按脚本位置推断")
    ap.add_argument("--target", default=None, help="Keil Target 名，默认第一个")
    ap.add_argument("--startup", default=None, help="覆盖启动文件路径")
    ap.add_argument("--ldscript", default=None, help="覆盖链接脚本路径")
    args = ap.parse_args()

    sdk = Path(args.sdk).resolve() if args.sdk else Path(__file__).resolve().parents[2]
    project = Path(args.project).resolve()
    if not project.is_file():
        raise SystemExit(f"工程文件不存在：{project}")

    try:
        info = parse_uvprojx(project, args.target)
    except ValueError as exc:
        raise SystemExit(str(exc))
    if not info["sources"]:
        raise SystemExit(f"{project.name}: 没有解析到任何 C/C++ 源文件")

    # 工程目录名用于查找 examples-gcc 下的资产；Target 名可能带空格（Keil 默认 "Target 1"）
    proj_name = project.parent.name

    startup = Path(args.startup).resolve() if args.startup else find_asset(sdk, proj_name, args.chip, "startup")
    ldscript = Path(args.ldscript).resolve() if args.ldscript else find_asset(sdk, proj_name, args.chip, "ld")

    if startup is None or not startup.is_file():
        raise SystemExit(
            f"找不到 {args.chip} 的启动文件。请先运行：\n"
            f"  python tools/ingw_switch_chip.py --project examples-gcc/{proj_name} "
            f"--series <SERIES>")
    if ldscript is None or not ldscript.is_file():
        raise SystemExit(
            f"找不到 {args.chip} 的链接脚本。请先运行：\n"
            f"  python tools/ingw_switch_chip.py --project examples-gcc/{proj_name} "
            f"--series <SERIES>")

    out_dir = Path(args.out).resolve() / proj_name
    out_dir.mkdir(parents=True, exist_ok=True)
    out_file = out_dir / "CMakeLists.txt"
    out_file.write_text(render(info, sdk, startup, ldscript, args.chip, proj_name),
                        encoding="utf-8")

    print(f"工程    : {project.name} (Target \"{proj_name}\")")
    print(f"Device  : {info['device']}")
    print(f"源文件  : {len(info['sources'])}")
    print(f"include : {len(info['includes'])}")
    print(f"宏定义  : {len(info['defines'])}")
    print(f"启动文件: {startup}")
    print(f"链接脚本: {ldscript}")
    print(f"输出    : {out_file}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
