# Code Review Checklist — D2TM

Primary review guide for pull requests to Dune II - The Maker, a C++23 / SDL2
real-time strategy game engine. Work through the relevant sections below. Skip
sections that a diff clearly does not touch.

General principles (see `CLAUDE.md` for the full version):

- Every changed line should trace to the stated purpose of the PR. Flag
  unrelated refactors, reformatting, or "drive-by improvements".
- Prefer the smallest change that solves the problem. Flag speculative
  abstraction, configurability, or error handling for impossible states.
- Match surrounding style rather than imposing a different one.

## Review output format

Keep the posted review short. A maintainer should read it in under a minute.

- One summary comment. Target under 120 words.
- Structure: a one-line verdict, then a list of issues found. Nothing else.
- Each issue: `file:line` — what is wrong — suggested fix. One or two sentences.
- If nothing needs changing, say so in one line and stop.
- Do not narrate your process. No "Checked:" / "Verified:" list, no progress
  recap, no enumeration of what looked fine. The value is the problems found.
- Do not restate the PR description or commit messages back to the author.
- Inline line comments are for concrete defects only — not observations,
  confirmations, or praise.

## Memory safety & resource management

- New/`delete` balanced; ownership of every heap allocation is clear and
  documented by where it lives (which class/pool owns it).
- No dangling pointers or references kept across frames: `cUnit`, `cAbstractStructure`,
  `bullet`, `cParticle` live in pooled arrays and slots get reused. Hold IDs, not
  pointers, across `thinkNormal`/`thinkSlow` boundaries.
- Array/pool indices are bounds-checked before use (unit id, structure id, cell
  index). Watch for `-1` / invalid-id sentinels being used as indices.
- SDL resources (`SDL_Surface`, `SDL_Texture`, `SDL_Renderer`, fonts, chunks) are
  freed on every path, including early returns and error paths.
- No use-after-free when an object destroys itself during iteration (e.g. a unit
  dying inside a loop over all units). Iteration handles removal safely.
- Containers: no iterator invalidation after insert/erase; no reference to a
  vector element retained across a growth.

## Ownership & lifetime

- Prefer references or `const&` for non-owning access. Raw owning pointers only
  where the existing pool pattern requires it.
- No new global mutable state. The `game` / `global_renderDrawer` / `gfxdata`
  globals are legacy and being removed — do not add to them. Pass `GameContext*`
  or the specific dependency instead.
- Object cleanup goes through the existing lifecycle hooks (unit `die()`,
  structure removal, `cGame` teardown), not ad-hoc deletes scattered in callers.

## Game-loop performance

- Know which tick the code runs on: `thinkFast()` (~5ms), `thinkNormal()`
  (~100ms), `thinkSlow()` (~1000ms). Expensive work (path search, full-map
  scans, allocations) belongs on the slowest tick that still meets the need.
- No per-frame heap allocation in `thinkFast()` or in draw code. Reuse buffers.
- No O(map) or O(units^2) scans added to a fast path without justification.
- Rendering: draw only what is on-screen (respect `cMapCamera` viewport).
  No redundant texture creation per frame; cache where the codebase already does.
- Logging (`logbook`, `Log.h`) is not called in tight per-cell / per-unit loops.

## Pathfinding & AI correctness

- Path requests handle "no path found" and target-occupied cases without
  spinning (unit re-requesting a path every tick).
- Cell occupancy is updated consistently: a unit leaving/entering a cell updates
  `cCell` unit/structure ids so pathfinding and collision stay correct.
- `cPlayerBrain` missions have a terminating condition; no state that can loop
  forever (e.g. mission never completes, never fails).
- Player index assumptions hold: 0 = human, 1–6 = AI. No off-by-one over
  `MAX_PLAYERS`. Neutral/sandworm handled where relevant.
- Fog-of-war / discovered state is respected: AI should not act on information a
  human player in the same position could not have.
- Random-dependent logic is seeded consistently and does not diverge between
  otherwise identical runs where determinism is expected.

## Map, units, structures

- Cell coordinate math: `iCell = y * mapWidth + x`. Check for boundary wrap
  (reading x-1 on column 0, etc.) and for `mapWidth` vs hardcoded 32/64/128.
- Structure footprints (multi-cell) fully occupy and fully release their cells.
- Unit/structure removal clears all back-references (selection, group, cell id,
  rally point, repair/refine targets).
- INI-driven data (`cUnitInfos`, `cStructureInfos`, ...) is read through the
  info tables, not hardcoded. New tunables get a `game.ini` entry.

## Build stability & CI

- Compiles with the project toolchain (C++23, gcc-15 on macOS per `CLAUDE.md`).
  No reliance on compiler extensions or platform-specific headers without guards.
- New source files are added to `CMakeLists.txt`.
- No new external dependency without discussion; SDL2 + SDL2_image/ttf/mixer is
  the sanctioned set.
- Does not break `build_pr.yml` / `build_master.yml`. Cross-platform paths use
  the existing helpers, not hardcoded separators.
- If a key binding changed: `keys.md` and `resources/bin/settings.ini` are both
  updated (see `.claude/rules/key-bindings.md`).
- Public/gameplay-facing behavior changes are reflected in docs where the repo
  already documents them (`CONTEXT.md`, `docs/adr/`).

## Style conventions (do not nitpick beyond these)

- `m_camelCase` members, `camelCase()` functions, `s_` structs, `e` enums,
  `c`-prefixed class files.
- `if`/`else` bodies always multi-line braced; never one-liners.
- Compare `std::optional<T>` directly to `T`; no needless dereference.
- `cAbstractStructure` id via `getStructureId()`, never `.id` / `.getId()`.
- Comments explain *why* when non-obvious; no placeholder or narration comments.
