#!/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Binding:
    action: str
    keys: tuple[str, ...]
    require_ctrl: bool
    require_alt: bool
    require_shift: bool

    def combo_label(self) -> str:
        parts: list[str] = []
        if self.require_ctrl:
            parts.append("Ctrl")
        if self.require_alt:
            parts.append("Alt")
        if self.require_shift:
            parts.append("Shift")
        parts.extend(normalize_scancode_name(key) for key in self.keys)
        return " + ".join(parts) if parts else "<empty>"


def normalize_scancode_name(scancode: str) -> str:
    prefix = "SDL_SCANCODE_"
    if scancode.startswith(prefix):
        return scancode[len(prefix):]
    return scancode


def extract_enum_actions(content: str) -> set[str]:
    match = re.search(r"enum\s+class\s+eKeyAction\s*\{(?P<body>.*?)\};", content, re.DOTALL)
    if not match:
        raise ValueError("Could not locate enum class eKeyAction")

    actions: set[str] = set()
    for raw_line in match.group("body").splitlines():
        line = raw_line.split("//", 1)[0].strip()
        if not line:
            continue
        line = line.rstrip(",")
        if line:
            actions.add(line)
    return actions


def extract_function_body(content: str, signature: str) -> str:
    start = content.find(signature)
    if start == -1:
        raise ValueError(f"Could not locate function: {signature}")

    brace_start = content.find("{", start)
    if brace_start == -1:
        raise ValueError(f"Could not locate function body for: {signature}")

    depth = 0
    for index in range(brace_start, len(content)):
        char = content[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return content[brace_start + 1:index]

    raise ValueError(f"Unbalanced braces while parsing: {signature}")


def split_top_level_csv(text: str) -> list[str]:
    parts: list[str] = []
    current: list[str] = []
    paren_depth = 0
    brace_depth = 0

    for char in text:
        if char == "," and paren_depth == 0 and brace_depth == 0:
            part = "".join(current).strip()
            if part:
                parts.append(part)
            current = []
            continue

        current.append(char)

        if char == "(":
            paren_depth += 1
        elif char == ")":
            paren_depth -= 1
        elif char == "{":
            brace_depth += 1
        elif char == "}":
            brace_depth -= 1

    tail = "".join(current).strip()
    if tail:
        parts.append(tail)

    return parts


def parse_bool(value: str, default: bool = False) -> bool:
    value = value.strip()
    if not value:
        return default
    if value == "true":
        return True
    if value == "false":
        return False
    raise ValueError(f"Unexpected boolean token: {value}")


def extract_default_bindings(content: str) -> list[Binding]:
    body = extract_function_body(content, "void cKeyBindings::loadDefaults()")
    pattern = re.compile(r"bind\((.*?)\);", re.DOTALL)
    bindings: list[Binding] = []

    for match in pattern.finditer(body):
        arguments = split_top_level_csv(match.group(1))
        if len(arguments) < 2:
            continue

        action_match = re.fullmatch(r"eKeyAction::([A-Z0-9_]+)", arguments[0].strip())
        if not action_match:
            raise ValueError(f"Could not parse action from bind(): {arguments[0]}")

        keys_expr = arguments[1].strip()
        keys = tuple(re.findall(r"SDL_SCANCODE_[A-Z0-9_]+", keys_expr))
        if not keys:
            raise ValueError(f"No SDL scancodes found in bind(): {keys_expr}")

        require_ctrl = parse_bool(arguments[2], False) if len(arguments) > 2 else False
        require_alt = parse_bool(arguments[3], False) if len(arguments) > 3 else False
        require_shift = parse_bool(arguments[4], False) if len(arguments) > 4 else False

        bindings.append(
            Binding(
                action=action_match.group(1),
                keys=keys,
                require_ctrl=require_ctrl,
                require_alt=require_alt,
                require_shift=require_shift,
            )
        )

    return bindings


def extract_actions_mapping(content: str) -> dict[str, str]:
    body = extract_function_body(content, "void cKeyBindings::loadFromSection(const cSection &section)")
    pattern = re.compile(r'\{"([A-Z0-9_]+)",\s*eKeyAction::([A-Z0-9_]+)\}')
    return {name: action for name, action in pattern.findall(body)}


def find_duplicate_combos(bindings: list[Binding]) -> dict[str, list[str]]:
    combos: dict[str, list[str]] = defaultdict(list)
    for binding in bindings:
        combos[binding.combo_label()].append(binding.action)
    return {
        combo: sorted(actions)
        for combo, actions in combos.items()
        if len(actions) > 1
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Audit cKeyBindings defaults and verify eKeyAction coverage."
    )
    parser.add_argument(
        "--keybindings",
        type=Path,
        default=Path("src/controls/cKeyBindings.cpp"),
        help="Path to cKeyBindings.cpp",
    )
    parser.add_argument(
        "--actions",
        type=Path,
        default=Path("src/controls/eKeyAction.h"),
        help="Path to eKeyAction.h",
    )
    parser.add_argument(
        "--fail-on-duplicates",
        action="store_true",
        help="Return a non-zero exit code when duplicate default key combinations are found.",
    )
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    keybindings_path = args.keybindings.resolve()
    actions_path = args.actions.resolve()

    keybindings_content = keybindings_path.read_text(encoding="utf-8")
    actions_content = actions_path.read_text(encoding="utf-8")

    enum_actions = extract_enum_actions(actions_content)
    default_bindings = extract_default_bindings(keybindings_content)
    action_mapping = extract_actions_mapping(keybindings_content)

    default_actions = {binding.action for binding in default_bindings}
    mapped_actions = set(action_mapping.values())
    all_referenced_actions = default_actions | mapped_actions

    missing_in_enum = sorted(all_referenced_actions - enum_actions)
    enum_without_default_bind = sorted(enum_actions - default_actions)
    mapped_without_default_bind = sorted(mapped_actions - default_actions)
    default_without_mapping = sorted(default_actions - mapped_actions)

    duplicates = find_duplicate_combos(default_bindings)

    print(f"eKeyAction entries: {len(enum_actions)}")
    print(f"Default binds: {len(default_bindings)}")
    print(f"INI mapping entries: {len(action_mapping)}")
    print()

    if duplicates:
        print("Duplicate default key combinations:")
        for combo, actions in sorted(duplicates.items()):
            print(f"  - {combo}: {', '.join(actions)}")
    else:
        print("No duplicate default key combinations found.")

    print()
    print("eKeyAction consistency:")
    if missing_in_enum:
        print("  Missing in eKeyAction.h:")
        for action in missing_in_enum:
            print(f"    - {action}")
    else:
        print("  All actions referenced in cKeyBindings.cpp exist in eKeyAction.h.")

    if enum_without_default_bind:
        print("  Declared in eKeyAction.h but not bound in loadDefaults():")
        for action in enum_without_default_bind:
            print(f"    - {action}")
    else:
        print("  Every eKeyAction entry has a default bind.")

    if mapped_without_default_bind:
        print("  Present in ACTIONS mapping but missing from loadDefaults():")
        for action in mapped_without_default_bind:
            print(f"    - {action}")

    if default_without_mapping:
        print("  Present in loadDefaults() but missing from ACTIONS mapping:")
        for action in default_without_mapping:
            print(f"    - {action}")

    has_enum_error = bool(missing_in_enum)
    has_duplicate_error = bool(duplicates) and args.fail_on_duplicates
    return 1 if has_enum_error or has_duplicate_error else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"error: {exc}", file=sys.stderr)
        raise SystemExit(1)