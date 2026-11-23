#!/usr/bin/env python3
"""Run non-destructive source maintenance checks for C/C++ code.

Checks include:
- trailing whitespace
- tab characters in source lines
- overly long lines
- TODO markers that do not follow TODO(#1234)
- missing newline at end of file
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence


DEFAULT_EXTENSIONS = {".h", ".hpp", ".hh", ".c", ".cc", ".cpp", ".cxx"}
DEFAULT_ROOTS = ["src", "tests"]
DEFAULT_EXCLUDES = {"build", "bin", ".git", ".venv", ".vscode", ".claude"}
TODO_ALLOWED_RE = re.compile(r"TODO\(#\d+\)")


@dataclass(frozen=True)
class Finding:
    file_path: Path
    line: int
    rule: str
    message: str


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Audit C/C++ source files for basic maintenance issues."
    )
    parser.add_argument(
        "roots",
        nargs="*",
        default=DEFAULT_ROOTS,
        help="Directories to scan recursively (default: src tests)",
    )
    parser.add_argument(
        "--max-line-length",
        type=int,
        default=140,
        help="Maximum allowed line length before reporting (default: 140)",
    )
    parser.add_argument(
        "--include-ext",
        nargs="*",
        default=sorted(DEFAULT_EXTENSIONS),
        help="File extensions to include (default: common C/C++ extensions)",
    )
    parser.add_argument(
        "--exclude-dir",
        nargs="*",
        default=sorted(DEFAULT_EXCLUDES),
        help="Directory names to skip everywhere (default: build/bin/.git/.venv/.vscode/.claude)",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Return exit code 1 if any finding is detected",
    )
    return parser.parse_args(argv)


def should_skip_dir(path: Path, excluded_names: set[str]) -> bool:
    return path.name in excluded_names


def iter_source_files(
    roots: Iterable[Path],
    include_ext: set[str],
    excluded_dir_names: set[str],
) -> Iterable[Path]:
    for root in roots:
        for path in root.rglob("*"):
            if path.is_dir() and should_skip_dir(path, excluded_dir_names):
                continue

            if not path.is_file():
                continue

            if any(part in excluded_dir_names for part in path.parts):
                continue

            if path.suffix.lower() in include_ext:
                yield path


def analyze_file(file_path: Path, max_line_length: int) -> list[Finding]:
    findings: list[Finding] = []
    try:
        content = file_path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return findings

    lines = content.splitlines(keepends=True)
    for idx, line in enumerate(lines, start=1):
        stripped_newline = line.rstrip("\r\n")

        if stripped_newline.endswith((" ", "\t")):
            findings.append(
                Finding(
                    file_path=file_path,
                    line=idx,
                    rule="trailing-whitespace",
                    message="Line has trailing whitespace.",
                )
            )

        if "\t" in stripped_newline:
            findings.append(
                Finding(
                    file_path=file_path,
                    line=idx,
                    rule="tab-character",
                    message="Line contains tab character(s).",
                )
            )

        if len(stripped_newline) > max_line_length:
            findings.append(
                Finding(
                    file_path=file_path,
                    line=idx,
                    rule="line-too-long",
                    message=f"Line length is {len(stripped_newline)} (max {max_line_length}).",
                )
            )

        if "TODO" in stripped_newline and not TODO_ALLOWED_RE.search(stripped_newline):
            findings.append(
                Finding(
                    file_path=file_path,
                    line=idx,
                    rule="todo-format",
                    message="TODO should use format TODO(#1234).",
                )
            )

    if content and not content.endswith("\n"):
        findings.append(
            Finding(
                file_path=file_path,
                line=max(1, len(lines)),
                rule="missing-final-newline",
                message="File should end with a newline.",
            )
        )

    return findings


def print_findings(findings: list[Finding], cwd: Path) -> None:
    if not findings:
        print("No findings. Source hygiene looks good.")
        return

    print(f"Findings: {len(findings)}")
    for finding in findings:
        try:
            rel_path = finding.file_path.relative_to(cwd)
        except ValueError:
            rel_path = finding.file_path
        print(f"- {rel_path}:{finding.line} [{finding.rule}] {finding.message}")


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)

    if args.max_line_length <= 0:
        print("Error: --max-line-length must be > 0.", file=sys.stderr)
        return 2

    cwd = Path.cwd().resolve()
    include_ext = {ext if ext.startswith(".") else f".{ext}" for ext in args.include_ext}
    excluded_dirs = set(args.exclude_dir)

    roots: list[Path] = []
    missing_roots: list[str] = []
    for raw_root in args.roots:
        root = (cwd / raw_root).resolve() if not Path(raw_root).is_absolute() else Path(raw_root).resolve()
        if not root.exists() or not root.is_dir():
            missing_roots.append(raw_root)
            continue
        roots.append(root)

    if missing_roots:
        print("Missing root directories:", file=sys.stderr)
        for root in missing_roots:
            print(f"  - {root}", file=sys.stderr)
        return 2

    files = sorted(set(iter_source_files(roots, include_ext, excluded_dirs)))
    all_findings: list[Finding] = []

    for file_path in files:
        all_findings.extend(analyze_file(file_path, args.max_line_length))

    print(
        "Audit summary: "
        f"{len(files)} files scanned | "
        f"{len(all_findings)} findings"
    )
    print_findings(all_findings, cwd)

    if args.strict and all_findings:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
