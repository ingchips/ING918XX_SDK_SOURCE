#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""compare_baseline.py —— 合并各分片的探测结果并与 baseline 对比。

判定规则：
  baseline=ok        且本次失败   -> 回归，CI 判红
  baseline=skip/fix  且本次失败   -> 已知，不判红
  baseline=skip/fix  且本次成功   -> 提示可更新 baseline（不判红）
  baseline 里没有    且本次失败   -> 新问题，判红

用法：
    python .github/scripts/compare_baseline.py \
        --groups groups --baseline ci/build-baseline.json \
        --summary "$GITHUB_STEP_SUMMARY"
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

# 这些状态算"编译成功"
OK_STATES = {"ok"}
# 这些状态算"已知不通过"，不判红
KNOWN_STATES = {"skip", "excluded", "special", "needs-fix"}


def load_current(groups_dir: Path) -> dict:
    """合并各分片 manifest，按 (project, chip) 取已探测的结果。"""
    current: dict[tuple[str, str], dict] = {}
    files = sorted(groups_dir.rglob("manifest.json"))
    if not files:
        raise SystemExit(f"{groups_dir} 下没有找到任何 manifest.json")

    for path in files:
        data = json.loads(path.read_text(encoding="utf-8"))
        for e in data.get("combinations", []):
            # 'generated' 表示这一片没轮到它，跳过
            if e.get("status") == "generated":
                continue
            current[(e["project"], e["chip"])] = e
    return current


def load_baseline(path: Path) -> dict:
    if not path.is_file():
        return {}
    data = json.loads(path.read_text(encoding="utf-8"))
    return {(c["project"], c["chip"]): c for c in data.get("combinations", [])}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--groups", required=True, help="各分片 artifact 的目录")
    ap.add_argument("--baseline", default="ci/build-baseline.json")
    ap.add_argument("--summary", default=None, help="写入 Markdown 摘要的文件")
    ap.add_argument("--update", action="store_true", help="按本次结果重写 baseline")
    args = ap.parse_args()

    current = load_current(Path(args.groups))
    baseline = load_baseline(Path(args.baseline))

    regressions, new_failures, improvements, known_failures = [], [], [], []

    for key, cur in sorted(current.items()):
        ok_now = cur["status"] in OK_STATES
        base = baseline.get(key)

        if base is None:
            if not ok_now:
                new_failures.append(cur)
            continue

        base_ok = base["status"] in OK_STATES
        if base_ok and not ok_now:
            regressions.append((base, cur))
        elif not base_ok and ok_now:
            improvements.append((base, cur))
        elif not base_ok and not ok_now:
            known_failures.append(cur)

    total = len(current)
    ok_count = sum(1 for e in current.values() if e["status"] in OK_STATES)

    lines = ["## 全量编译验证", ""]
    lines.append(f"- 本次探测组合：**{total}**")
    lines.append(f"- 通过：**{ok_count}**")
    lines.append(f"- 回归（baseline 为 ok 却失败）：**{len(regressions)}**")
    lines.append(f"- 新失败（baseline 未记录）：**{len(new_failures)}**")
    lines.append(f"- 已知不通过：{len(known_failures)}")
    lines.append(f"- 新增通过（可从 baseline 移除）：{len(improvements)}")
    lines.append("")

    if regressions:
        lines.append("### 回归")
        lines.append("")
        lines.append("| 工程 | 芯片 | baseline | 本次 | 原因 |")
        lines.append("| --- | --- | --- | --- | --- |")
        for base, cur in regressions:
            lines.append(f"| {cur['project']} | {cur['chip']} | {base['status']} | "
                         f"{cur['status']} | {(cur.get('reason') or '')[:80]} |")
        lines.append("")

    if new_failures:
        lines.append("### 新失败")
        lines.append("")
        lines.append("| 工程 | 芯片 | 原因 |")
        lines.append("| --- | --- | --- |")
        for cur in new_failures:
            lines.append(f"| {cur['project']} | {cur['chip']} | "
                         f"{(cur.get('reason') or '')[:100]} |")
        lines.append("")

    if improvements:
        lines.append("### 新增通过（建议更新 baseline）")
        lines.append("")
        for base, cur in improvements[:40]:
            lines.append(f"- {cur['project']} / {cur['chip']}（原 {base['status']}）")
        lines.append("")

    text = "\n".join(lines)
    print(text)

    if args.summary:
        Path(args.summary).write_text(text, encoding="utf-8")

    if args.update:
        combos = []
        for key, cur in sorted(current.items()):
            combos.append({
                "project": cur["project"],
                "chip": cur["chip"],
                "status": cur["status"],
                "reason": (cur.get("reason") or "")[:200],
            })
        out = {"generated_at": "auto", "combinations": combos}
        Path(args.baseline).write_text(
            json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
        print(f"\nbaseline 已更新：{args.baseline}（{len(combos)} 条）")

    if regressions or new_failures:
        print(f"\n失败：{len(regressions)} 个回归，{len(new_failures)} 个新失败", file=sys.stderr)
        return 1
    print("\n通过：没有回归。")
    return 0


if __name__ == "__main__":
    sys.exit(main())
