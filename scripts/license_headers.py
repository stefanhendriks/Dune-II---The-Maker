#!/usr/bin/env python3
"""Apply the standard D2TM license header to every source file under src/.

See GitHub issue #1067 / #1388.
"""

import os
import re
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC_DIR = os.path.join(REPO_ROOT, "src")

HEADER_TEMPLATE = """/**
 * @file {filename}
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */
"""

OLD_HEADER_RE = re.compile(r"\A/\*.*?Dune II - The Maker.*?\*/\n?", re.DOTALL)
NEW_HEADER_MARKER = "Dune 2 - The Maker"


def process_file(path: str) -> bool:
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    if NEW_HEADER_MARKER in content[:400]:
        return False

    content = OLD_HEADER_RE.sub("", content, count=1)
    content = content.lstrip("\n")

    header = HEADER_TEMPLATE.format(filename=os.path.basename(path))
    new_content = header + "\n" + content

    with open(path, "w", encoding="utf-8") as f:
        f.write(new_content)

    return True


def main() -> int:
    changed = 0
    for dirpath, _dirnames, filenames in os.walk(SRC_DIR):
        for filename in filenames:
            if not filename.endswith((".h", ".hpp", ".cpp")):
                continue
            path = os.path.join(dirpath, filename)
            if process_file(path):
                changed += 1

    print(f"Updated {changed} file(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
