#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""keil_uvprojx.py —— Keil .uvprojx 解析。

被两个脚本共用：
  * .github/scripts/keil2cmake.py  单个工程转 CMake（CI 用）
  * tools/gen_all_cmake.py         批量生成全量工程的 CMake 工程

只做解析，不生成任何文件。
"""

from __future__ import annotations

import xml.etree.ElementTree as ET
from pathlib import Path

# 这些文件由 SDK 的 GNU 版本替代，或本来就不参与编译
SKIP_SUFFIXES = {".s", ".sct", ".ini", ".lst", ".o", ".d", ".crf", ".axf", ".hex", ".bin"}
SKIP_NAMES = {"symdefs.m", "symdefs.s", "symdefs.i", "symdefs.g"}


def text_of(node, path: str, default: str = "") -> str:
    found = node.find(path)
    return (found.text or "").strip() if found is not None and found.text else default


def parse_uvprojx(path: Path, target_name: str | None = None) -> dict:
    """解析 .uvprojx，返回 name/device/sources/includes/defines/targets。

    sources 是已解析的绝对路径；includes 同样已解析为绝对路径。
    """
    path = Path(path)
    root = ET.parse(path).getroot()

    targets = root.findall(".//Target")
    if not targets:
        raise ValueError(f"{path.name}: 没有找到 <Target>")

    target = None
    if target_name:
        for t in targets:
            if text_of(t, "TargetName") == target_name:
                target = t
                break
        if target is None:
            names = [text_of(t, "TargetName") for t in targets]
            raise ValueError(f"{path.name}: 没有名为 {target_name} 的 Target，可选：{names}")
    else:
        target = targets[0]

    name = text_of(target, "TargetName") or path.stem
    device = text_of(target, ".//Device")

    includes_raw = text_of(target, ".//Cads/VariousControls/IncludePath")
    defines_raw = text_of(target, ".//Cads/VariousControls/Define")
    asm_includes_raw = text_of(target, ".//Aads/VariousControls/IncludePath")

    includes: list[Path] = []
    for raw in (includes_raw + ";" + asm_includes_raw).split(";"):
        raw = raw.strip()
        if not raw:
            continue
        includes.append((path.parent / raw.replace("\\", "/")).resolve())

    # Define 用逗号分隔（Keil 习惯），也兼容分号
    defines = [x.strip() for x in defines_raw.replace(",", ";").split(";") if x.strip()]

    sources: list[Path] = []
    for f in target.findall(".//Files/File"):
        fname = text_of(f, "FileName")
        fpath = text_of(f, "FilePath")
        if not fname or not fpath:
            continue
        if fname.lower() in SKIP_NAMES:
            continue
        if Path(fname).suffix.lower() in SKIP_SUFFIXES:
            continue
        p = (path.parent / fpath.replace("\\", "/")).resolve()
        if p.is_file():
            sources.append(p)

    return {
        "name": name,
        "device": device,
        "sources": sources,
        "includes": includes,
        "defines": defines,
        "targets": [text_of(t, "TargetName") for t in targets],
    }
