#!/usr/bin/env python3
"""Audit directories that have too many or too few files.

By default, this script scans `src/` recursively and reports directories
containing less than `--min-files` or more than `--max-files` direct files.
"""

from __future__ import annotations

import argparse
import os
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence


@dataclass(frozen=True)
class DirectoryCount:
    path: Path
    file_count: int


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Scan directories recursively and report those with too many "
            "or too few direct files."
        )
    )
    parser.add_argument(
        "roots",
        nargs="*",
        default=["src"],
        help="Root directories to scan recursively (default: src)",
    )
    parser.add_argument(
        "--min-files",
        type=int,
        default=5,
        help="Minimum allowed direct files per directory (default: 5)",
    )
    parser.add_argument(
        "--max-files",
        type=int,
        default=25,
        help="Maximum allowed direct files per directory (default: 25)",
    )
    parser.add_argument(
        "--include-hidden",
        action="store_true",
        help="Include hidden files and directories (names starting with '.')",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Return exit code 1 if at least one directory is outside limits",
    )
    return parser.parse_args(argv)


def is_hidden_name(name: str) -> bool:
    return name.startswith(".")


def iter_directory_counts(root: Path, include_hidden: bool) -> Iterable[DirectoryCount]:
    for current_dir, sub_dirs, file_names in os.walk(root):
        if not include_hidden:
            sub_dirs[:] = [d for d in sub_dirs if not is_hidden_name(d)]
            file_names = [f for f in file_names if not is_hidden_name(f)]

        yield DirectoryCount(path=Path(current_dir), file_count=len(file_names))


def print_section(title: str, rows: list[DirectoryCount], cwd: Path) -> None:
    print(title)
    if not rows:
        print("  (none)")
        return

    for row in rows:
        try:
            relative_path = row.path.relative_to(cwd)
        except ValueError:
            relative_path = row.path
        print(f"  {row.file_count:>3} | {relative_path}")


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    if args.min_files < 0 or args.max_files < 0:
        print("Error: --min-files and --max-files must be >= 0.", file=sys.stderr)
        return 2
    if args.min_files > args.max_files:
        print("Error: --min-files cannot be greater than --max-files.", file=sys.stderr)
        return 2

    cwd = Path.cwd().resolve()
    all_counts: list[DirectoryCount] = []
    missing_roots: list[str] = []

    for raw_root in args.roots:
        root_path = (cwd / raw_root).resolve() if not Path(raw_root).is_absolute() else Path(raw_root).resolve()
        if not root_path.exists() or not root_path.is_dir():
            missing_roots.append(raw_root)
            continue
        all_counts.extend(iter_directory_counts(root_path, include_hidden=args.include_hidden))

    if missing_roots:
        print("Missing root directories:", file=sys.stderr)
        for root in missing_roots:
            print(f"  - {root}", file=sys.stderr)
        return 2

    too_few = sorted(
        [d for d in all_counts if d.file_count < args.min_files],
        key=lambda d: (d.file_count, str(d.path)),
    )
    too_many = sorted(
        [d for d in all_counts if d.file_count > args.max_files],
        key=lambda d: (-d.file_count, str(d.path)),
    )
    in_range = [d for d in all_counts if args.min_files <= d.file_count <= args.max_files]

    print(
        "Verdict: "
        f"{len(all_counts)} directories scanned | "
        f"{len(in_range)} ok | "
        f"{len(too_few)} too small (<{args.min_files}) | "
        f"{len(too_many)} too large (>{args.max_files})"
    )
    print()
    print_section(f"Directories that are too small (<{args.min_files})", too_few, cwd)
    print()
    print_section(f"Directories that are too large (>{args.max_files})", too_many, cwd)

    if args.strict and (too_few or too_many):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
