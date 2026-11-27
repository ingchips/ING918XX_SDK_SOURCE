#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""keil2cmake.py —— 把单个 Keil 工程转成 CMake 工程（CI 内部工具）。

这是 `tools/gen_all_cmake.py` 的单工程入口：真正生成 CMakeLists、启动文件、
链接脚本的逻辑都在那边，这里只负责把命令行参数翻译成一次 generate_one()。

之所以不自己实现一套：早期版本确实独立生成，结果是两套路径处理、两套
CMakeLists 模板，而且漏掉了工程 `.ingw` 里的 series（ING9187xx 与 ING9188xx
是两颗不同的 ROM）和启动文件的现场生成，只能处理 examples-gcc/ 下已有对应
工程的场景。

用法：
    python .github/scripts/keil2cmake.py \
        --project examples/peripheral_ibeacon/peripheral_ibeacon.uvprojx \
        --chip ing918 --out build/keil-import

产物：<out>/<工程名>/CMakeLists.txt（含现场生成的 gstartup_*.s 与 .ld）。
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

# 复用批量生成器的全部逻辑
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "tools"))
from gen_all_cmake import (  # noqa: E402
    DEFAULT_CONFIG,
    FAMILY_SERIES,
    generate_one,
    load_matrix_config,
)


def main() -> int:
    ap = argparse.ArgumentParser(description="把单个 Keil 工程转成 CMake 工程（CI 用）")
    ap.add_argument("--project", required=True, help=".uvprojx 路径")
    ap.add_argument("--chip", required=True, choices=sorted(FAMILY_SERIES),
                    help="目标芯片家族")
    ap.add_argument("--out", required=True, help="输出目录")
    ap.add_argument("--sdk", default=None, help="SDK 根目录，默认按脚本位置推断")
    ap.add_argument("--config", default=DEFAULT_CONFIG,
                    help="工程测试矩阵配置，用于取该工程的 C 标准")
    args = ap.parse_args()

    sdk = Path(args.sdk).resolve() if args.sdk else Path(__file__).resolve().parents[2]
    project = Path(args.project).resolve()
    if not project.is_file():
        raise SystemExit(f"工程文件不存在：{project}")

    proj_dir = project.parent
    out_root = Path(args.out)
    if not out_root.is_absolute():
        out_root = sdk / out_root
    out_root.mkdir(parents=True, exist_ok=True)

    cfg = load_matrix_config(sdk / args.config)
    pc = (cfg["projects"].get(proj_dir.name) or {})
    if pc.get("skip"):
        raise SystemExit(f"{proj_dir.name} 在 {args.config} 里标记为 skip：{pc.get('note', '')}")
    c_standard = pc.get("c_standard") or cfg["defaults"]["c_standard"]

    ld_template = sdk / "examples-gcc" / "get_started" / "get_started.ld"
    entry = generate_one(sdk, proj_dir, project, args.chip, out_root, ld_template,
                         c_standard)

    if entry["status"] != "generated":
        raise SystemExit(f"生成失败：{entry.get('reason', '未知原因')}")

    out_file = out_root / entry["dir"] / "CMakeLists.txt"
    print(f"工程    : {project.name}")
    print(f"芯片    : {args.chip} / series {entry.get('series', '')} / "
          f"bundle {entry.get('bundle', '')}")
    print(f"标准    : {c_standard}")
    print(f"源文件  : {entry.get('sources', 0)}")
    print(f"链接库  : {', '.join(entry.get('libs') or []) or '(无)'}")
    print(f"输出    : {out_file}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
