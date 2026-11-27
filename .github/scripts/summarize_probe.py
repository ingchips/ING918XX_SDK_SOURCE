#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""summarize_probe.py —— 合并各分片的探测结果，按工具链分别判定。

语义：`.github/project-matrix.yaml` 是验收清单。
清单里声明成立的组合必须在**每条工具链**上都编过，任何一个失败都判红。

GCC 与 ATfE 通道的 manifest 通过顶层 `toolchain` 字段区分，合并时按
(project, chip, toolchain) 去重，两个通道互不覆盖。

用法：
    python .github/scripts/summarize_probe.py \
        --groups groups --summary "$GITHUB_STEP_SUMMARY"
"""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

OK_STATES = {"ok"}
SKIP_STATES = {"excluded"}


def load_current(groups_dir: Path) -> dict:
    """合并各分片 manifest，key = (project, chip, toolchain)。"""
    current: dict[tuple[str, str, str], dict] = {}
    files = sorted(groups_dir.rglob("manifest.json"))
    if not files:
        raise SystemExit(f"{groups_dir} 下没有找到任何 manifest.json")

    for path in files:
        data = json.loads(path.read_text(encoding="utf-8"))
        toolchain = data.get("toolchain") or "gcc"
        for e in data.get("combinations", []):
            if e.get("status") == "generated":
                continue
            e["toolchain"] = toolchain
            key = (e["project"], e.get("chip") or "-", toolchain)
            current[key] = e
    return current


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--groups", required=True, help="各分片 artifact 的目录")
    ap.add_argument("--summary", default=None, help="写入 Markdown 摘要的文件")
    args = ap.parse_args()

    current = load_current(Path(args.groups))

    by_tc: dict[str, list[dict]] = defaultdict(list)
    for e in current.values():
        by_tc[e["toolchain"]].append(e)

    lines = ["## 全量编译验证（配置清单驱动）", ""]
    failures: list[dict] = []

    # manifest 里的 toolchain 是工具链文件名（arm-none-eabi），显示成 gcc 更直观
    TC_LABEL = {"arm-none-eabi": "gcc", "atfe": "atfe"}
    for tc in sorted(by_tc):
        entries = by_tc[tc]
        label = TC_LABEL.get(tc, tc)
        ok = sum(1 for e in entries if e["status"] in OK_STATES)
        skip = sum(1 for e in entries if e["status"] in SKIP_STATES)
        bad = [e for e in entries if e["status"] not in OK_STATES | SKIP_STATES]
        failures.extend(bad)
        lines.append(f"### {label}")
        lines.append("")
        lines.append(f"- 清单内组合：**{ok + len(bad)}**")
        lines.append(f"- 通过：**{ok}**")
        lines.append(f"- 失败：**{len(bad)}**")
        lines.append(f"- 不测（配置 skip）：{skip}")
        lines.append("")

    if failures:
        lines.append("### 失败明细")
        lines.append("")
        lines.append("| 工具链 | 工程 | 芯片 | 标准 | 状态 | 原因 |")
        lines.append("| --- | --- | --- | --- | --- | --- |")
        for e in sorted(failures, key=lambda x: (x["toolchain"], x["project"], x.get("chip") or "")):
            lines.append(
                f"| {e['toolchain']} | {e['project']} | {e.get('chip') or '-'} | "
                f"{e.get('c_standard') or '-'} | {e['status']} | "
                f"{(e.get('reason') or '')[:100]} |")
        lines.append("")
        lines.append("清单在 `.github/project-matrix.yaml`。"
                     "若某组合确实不支持，把它从清单里移除、改 `skip: true`"
                     "（整个工程）或 `atfe_skip: true`（仅 ATfE 通道），并写明原因。")
        lines.append("")

    text = "\n".join(lines)
    print(text)
    if args.summary:
        Path(args.summary).write_text(text, encoding="utf-8")

    if failures:
        print(f"\n失败：{len(failures)} 个组合未通过", file=sys.stderr)
        return 1
    print("\n通过：清单内所有组合在全部工具链上编译成功。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
