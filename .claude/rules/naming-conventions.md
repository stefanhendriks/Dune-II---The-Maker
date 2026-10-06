# Naming Conventions

Single source of truth for naming across the codebase. `CLAUDE.md` and the
code review checklist (`.claude/review.md`) both point here instead of
repeating this list — update this file only, not copies elsewhere.

## Code naming

- Member variables: `m_camelCasedVariableName`
- Functions: `camelCased()`
- Speed-differentiated methods: `thinkFast_*()`, `thinkSlow_*()`
- State-related methods (candidates for extraction): `state_*()`
- Structs use `s_` prefix, enums use `e` prefix

## File naming

- Class files: `c` prefix + PascalCase — `cHousesInfo.h`, `cPlayer.cpp`
- Non-class utility files: PascalCase — `Color.hpp`, `HouseColors.h`, `Log.h`

## New classes: no `c` prefix

Classes introduced from here on should NOT use the `c` prefix — follow the
pattern already set by `GameContext`, `TextContext`, `GraphicsContext`, and
`AudioContext`. This is not retroactive: existing `c`-prefixed classes keep
their names as-is. The long-term intent is to eventually drop the `c`-prefix
convention codebase-wide, but that is not being done as a mass rename right
now — it only applies when naming something new.
