#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""summarize_probe.py —— 合并各分片的探测结果，判定配置内组合是否全部通过。

语义：`.github/project-matrix.yaml` 是验收清单。
清单里声明成立的组合必须全部编过，任何一个失败都判红。

用法：
    python .github/scripts/summarize_probe.py \
        --groups groups --summary "$GITHUB_STEP_SUMMARY"
"""

from __future__ import annotations

import argparse
import json
import sys
from collections import Counter
from pathlib import Path


def load_current(groups_dir: Path) -> dict:
    """合并各分片 manifest，按 (project, chip) 取已探测的结果。"""
    current: dict[tuple[str, str], dict] = {}
    files = sorted(groups_dir.rglob("manifest.json"))
    if not files:
        raise SystemExit(f"{groups_dir} 下没有找到任何 manifest.json")

    for path in files:
        data = json.loads(path.read_text(encoding="utf-8"))
        for e in data.get("combinations", []):
            # 'generated' 表示这一片没轮到它
            if e.get("status") == "generated":
                continue
            key = (e["project"], e.get("chip") or "-")
            current[key] = e
    return current


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--groups", required=True, help="各分片 artifact 的目录")
    ap.add_argument("--summary", default=None, help="写入 Markdown 摘要的文件")
    args = ap.parse_args()

    current = load_current(Path(args.groups))
    stat = Counter(e["status"] for e in current.values())

    ok = stat.get("ok", 0)
    excluded = stat.get("excluded", 0)
    failures = [e for e in current.values()
                if e["status"] not in ("ok", "excluded")]

    lines = ["## 全量编译验证（配置清单驱动）", ""]
    lines.append(f"- 清单内组合：**{ok + len(failures)}**")
    lines.append(f"- 通过：**{ok}**")
    lines.append(f"- 失败：**{len(failures)}**")
    lines.append(f"- 不测（配置 skip）：{excluded}")
    lines.append("")

    if failures:
        lines.append("### 失败明细")
        lines.append("")
        lines.append("| 工程 | 芯片 | 标准 | 状态 | 原因 |")
        lines.append("| --- | --- | --- | --- | --- |")
        for e in sorted(failures, key=lambda x: (x["project"], x.get("chip") or "")):
            lines.append(
                f"| {e['project']} | {e.get('chip') or '-'} | "
                f"{e.get('c_standard') or '-'} | {e['status']} | "
                f"{(e.get('reason') or '')[:100]} |")
        lines.append("")
        lines.append("清单在 `.github/project-matrix.yaml`。"
                     "若某组合确实不支持，把它从清单里移除或改 `skip: true` 并写明原因。")
        lines.append("")

    text = "\n".join(lines)
    print(text)
    if args.summary:
        Path(args.summary).write_text(text, encoding="utf-8")

    if failures:
        print(f"\n失败：{len(failures)} 个组合未通过", file=sys.stderr)
        return 1
    print(f"\n通过：清单内 {ok} 个组合全部编译成功。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
