# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial tasks, use judgment.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.

## Project Overview

D2TM (Dune II - The Maker) is a C++23 remake of the classic RTS game Dune II. 
It uses SDL3 (+ SDL3_image, SDL3_ttf, SDL3_mixer) for rendering, audio, and input. 

Unit tests use Catch2 (v3) and live in `tests/` (registered in `tests/CMakeLists.txt`). They build into `d2tm_tests` alongside the game and run via `ctest`; CI runs them on every platform. The tests cover pure logic only (utils, map geometry); there are no tests that need a running game or SDL window. See also `.claude/rules/build-workflow.md`.

### Related repos

Raw game assets (sprites, sounds) and the `sdl_*.dat` packer tool live in the sibling repo `Dune-II---The-Maker-Data`, not in this repo. Asset-level fixes go in `resources/bin/data/` there, not `bin/data/` (which is this repo's gitignored, packaged output).

## Build Commands
**One-time setup (dependencies):**
```sh
./install_dependencies_macosx.sh   # macOS: cmake, ninja, pkg-config, sdl3 (+ ttf/image/mixer), catch2 via brew
./install_dependencies_ubuntu.sh   # Ubuntu/Debian: build tools + catch2 via apt, SDL3 libs built from source
```
On Windows, build in an MSYS2 MINGW64 shell and install the packages from the `install:` line of the
"Install MSYS2 with MinGW 64-bit" step in `.github/workflows/build_pr.yml` with `pacman -S`.

**Build and test:**
```sh
mkdir build && cd build
cmake ..          # Windows (MinGW64): cmake .. -G "MinGW Makefiles"
make -j4          # or: cmake --build . --target all -- -j 6
ctest --output-on-failure
```

**Quick rebuild:**
```sh
./rebuildmacos.sh   # macOS/Linux: rm -rf build, cmake + build, ctest, copy build/d2tm to bin/
rebuildwin.bat      # Windows (from an MSYS2 MINGW64 shell): rm -rf build, cmake + build, ctest
```

CI builds with `-DCMAKE_BUILD_TYPE=RelWithDebInfo` and treats warnings as errors; locally the default
build type is Debug and warnings are not errors unless you configure with `-DD2TM_WERROR=ON`.

The binary is `build/d2tm` (`build/d2tm.exe` on Windows).
Before you can run the game, make sure to run ./create_release script (on Windows this is .bat, on *nix systems .sh one)

After that you will have a complete package in <PROJECT ROOT>/bin where you can execute d2tm.exe or ./d2tm.
Run it with `bin/` as the working directory: data paths such as `settings.ini` and `skirmish/` are resolved relative to it.

**CLion on macOS (Apple Silicon):** Set C compiler to `/opt/homebrew/bin/gcc-15`, C++ to `/opt/homebrew/bin/g++-15`.

**Code formatting** (requires `astyle`):
```sh
python3 astyle.py
```

## Architecture

### Entry Point

`src/main.cpp` creates the `cGame` instance as a local, applies settings (`settings.ini` plus command-line arguments), then calls `setupGame()`, `run()` and `shutdown()`. The old globals (`game`, `global_renderDrawer`, `gfxdata`) are gone; shared state is reached through the context classes and `sGameServices` described below.

### cGame — Central Facade

`src/game/cGame.h` is the top-level class (implementation in `src/game/cGame_logic.cpp`). It owns or references virtually everything:
- **Game objects** via `cGameObjectContext` (`src/context/cGameObjectContext.h`): `cUnits`, `cStructures`, `cBullets`, `cParticles`, `cMap`, `cPlayers`
- **Info/config tables** (loaded from INI) via `cInfoContext` (`src/context/cInfoContext.h`): `cUnitInfos`, `cStructureInfos`, `cBulletInfos`, `cParticleInfos`, `cSpecialInfos`, `cUpgradeInfos`
- **State machine**: `m_state`, `m_nextState`, `m_currentState` with `setNextStateToTransitionTo()` + `transitionStateIfRequired()`; `cCreatorState` creates and caches the `cGameState` instances
- **Subsystems**: `cTimeManager`, `cDrawManager`, `cMapCamera`, `AudioContext` (wraps `cSoundPlayer`), `cScreenShake`, `cScreenFader`, `cEventEmitter`, `cMissionStatsCollector`
- **Services**: `sGameServices` (`src/include/sGameServices.h`) bundles pointers to the above and is handed to objects that need them

Game loop timing uses three speeds called in `run()`:
- `thinkFast()` — every ~5 ms (input, animation, audio)
- `thinkNormal()` — every ~100 ms (unit AI, bullets, structures)
- `thinkSlow()` — every ~1000 ms (economy, slow AI decisions)

### Game State Machine

States are defined in `src/gamestates/` and implement `cGameState` (abstract base with `thinkFast()`, `thinkNormal()`, `thinkSlow()`, `draw()`). States include: `cMainMenuState`, `cGamePlaying`, `cSetupSkirmishState`, `cMentatState`, `cChooseHouseState`, `cSelectYourNextConquestState`, `cWinLoseState`, etc.

State transitions go through `cGame::setNextStateToTransitionTo()` + `initiateFadingOut()`.

### GameContext — Dependency Container

`src/context/GameContext.hpp` aggregates shared services passed to states and drawers: `GraphicsContext`, `TextContext`, `cTimeManager`, `AudioContext`, `SDLDrawer` (and, for now, `cGameInterface`). States receive a `GameContext*` and use it instead of reaching into globals.

This is part of an active, ongoing migration — see `.claude/rules/architecture-direction.md` for the current direction (removing `cGameInterface`, the `sGameServices` caching pattern, construction-order safety, and narrow `*Context` service classes).

### Rendering

`src/drawers/` contains specialized drawers (one per concern: map, minimap, sidebar, particles, structures, mouse, messages, etc.). `cDrawManager` (`src/managers/cDrawManager.h`) composes them for the combat/playing state. All rendering goes through `SDLDrawer` which wraps SDL3's renderer.

Graphics assets are loaded from `.dat` files in `bin/data/` (sources in `resources/bin/data/`) via the `Graphics` class. 
Multiple `Graphics` instances are used: `gfxdata`, `gfxinter`, `gfxworld`, `gfxmentat`, `gfxeditor`, held by `GraphicsContext` (`src/context/GraphicsContext.hpp`). The generated `src/data/gfx*.h` headers define the index constants into each `.dat` file.

### Game Objects

All live in `src/gameobjects/`:
- **Units** (`cUnit`, `cUnits`) — array-based pool, `cUnitInfos` holds static type data
- **Structures** (`cAbstractStructure` subclasses, `cStructures`) — each building type is a subclass (e.g. `cRefinery`, `cBarracks`, `cPalace`)
- **Projectiles** (`cBullet` in `bullet.h`, `cBullets`)
- **Particles** (`cParticle`, `cParticles`)

### Players & AI

`src/gameobjects/players/cPlayer.h` represents each of up to 7 players (index 0 = human, 1–6 = AI). 

Each player owns a `cPlayerBrain` (`src/gameobjects/players/brains/`) with variants: `cPlayerBrainCampaign`, `cPlayerBrainSkirmish`, `cPlayerBrainSandworm`, `cPlayerBrainEmpty`. 

Brains are ticked every ~100 ms and execute `cPlayerBrainMission`s (attack, explore, DeathHand, Fremen, Saboteur).

### Map

`cMap` (`src/gameobjects/map/cMap.h`) is a flat cell array (32×32 px tiles). `cCell` holds per-cell data (terrain type, unit/structure IDs, fog-of-war). `cMapCamera` handles viewport scrolling.

### Events / Observer Pattern

Game events flow through two observer interfaces:
- `cScenarioObserver` — receives `s_GameEvent` (unit died, structure built, etc.)
- `cInputObserver` — receives `s_MouseEvent` and `cKeyboardEvent`

`cGame` implements both and re-dispatches to the active `cGameState`, `cDrawManager`, etc.

### INI Configuration

Game data (unit stats, structure costs, scenario rules) is loaded from INI files at startup via `cIniFile` / `cIniUtils`. Campaign missions are in `bin/campaign/`, skirmish maps in `bin/skirmish/` (sources in `resources/bin/campaign/` and `resources/bin/skirmish/`; edit those, `bin/` is regenerated by create_release). `game.ini` drives most gameplay parameters: `resources/game.ini.org` holds the shipped rules (create_release copies it to `bin/game.ini`), `resources/game.ini` is a developer profile with tweaked values. New keys usually go into both.

## Naming Conventions

See `.claude/rules/naming-conventions.md` — the single source of truth for code and file naming, including the no-`c`-prefix rule for new classes.

## Game concepts

- This is a RTS game. Based on Dune 2; but with modernizations. (Much like Red Alert of Command & Conquer)
- A 'cell' is a tile on a map

## Code & Github
- changes should never be done directly on master or main branch, if you are on master or main branch create a new branch
- produce small PR's (a very limited set of changes)
- branch names are usually: `<prefix>/<ticket-githubnr here>/<short-description-of-feature>`
  - prefixes can be `feature` (new feature), `bugfix` or `improv` (for technical improvements usually)
- prefer rebasing a branch onto the latest base branch over merging; confirm with the maintainer first if the branch is already shared with another contributor
- do not add a `Co-Authored-By` trailer (or similar AI-attribution line) to commit messages
- confirm with the maintainer before creating or closing issues, changing labels, or posting comments — don't do these unprompted (see `docs/agents/issue-tracker.md` for more)

## Agent skills

### Issue tracker

Issues live in GitHub Issues (`stefanhendriks/Dune-II---The-Maker`). See `docs/agents/issue-tracker.md`.

### Triage labels

Default canonical label vocabulary (needs-triage, needs-info, ready-for-agent, ready-for-human, wontfix). See `docs/agents/triage-labels.md`.

### Domain docs

Single-context layout: `CONTEXT.md` + `docs/adr/` at the repo root. See `docs/agents/domain.md`.