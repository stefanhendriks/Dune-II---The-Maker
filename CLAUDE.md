# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

D2TM (Dune II - The Maker) is a C++23 remake of the classic RTS game Dune II. 
It uses SDL2 (+ SDL2_image, SDL2_ttf, SDL2_mixer) for rendering, audio, and input. 

There are no automated tests in this project.

## Build Commands
**macOS (one-time setup):**
```sh
./install_dependencies_macosx.sh   # installs gcc, cmake, ninja, sdl2 via brew
```

**Build:**
```sh
mkdir build && cd build
cmake ..
make -j4          # or: cmake --build . --target all -- -j 6
```

**Quick rebuild (macOS):**
```sh
./rebuildmacos.sh   # rm -rf build, then cmake + build
```

The binary is `build/d2tm`. Run it from the repo root so it can find `bin/` (game data lives there).
Before you can run the game, make sure to run ./create_release script (on Windows this is .bat, on *nix systems .sh one)

After that you will have a complete package in <PROJECT ROOT>/bin where you can execute d2tm.exe or ./d2tm

**CLion on macOS (Apple Silicon):** Set C compiler to `/opt/homebrew/bin/gcc-15`, C++ to `/opt/homebrew/bin/g++-15`.

**Code formatting** (requires `astyle`):
```sh
python3 astyle.py
```

## Architecture

### Entry Point & Global State

`src/include/d2tmc.h` declares the single global `cGame game` instance plus `global_renderDrawer` and `gfxdata`. 

These globals are explicitly acknowledged as legacy and are being incrementally moved into proper classes.

### cGame — Central Facade

`src/game/cGame.h` is the top-level class. It owns or references virtually everything:
- **Game objects** directly as members: `cUnits`, `cStructures`, `cBullets`, `cParticles`, `cMap`, `cPlayers`
- **Info/config tables** (loaded from INI): `cUnitInfos`, `cStructureInfos`, `cBulletInfos`, `cParticleInfos`, `cSpecialInfos`, `cUpgradeInfos`
- **State machine**: `m_state`, `m_nextState`, array of `cGameState*` with `setNextStateToTransitionTo()` + `transitionStateIfRequired()`
- **Subsystems**: `cTimeManager`, `cDrawManager`, `cMapCamera`, `cInteractionManager`, `cSoundPlayer`, `cScreenShake`, `cScreenFader`

Game loop timing uses three speeds called in `run()`:
- `thinkFast()` — every ~5 ms (input, animation, audio)
- `thinkNormal()` — every ~100 ms (unit AI, bullets, structures)
- `thinkSlow()` — every ~1000 ms (economy, slow AI decisions)

### Game State Machine

States are defined in `src/gamestates/` and implement `cGameState` (abstract base with `thinkFast()`, `thinkNormal()`, `thinkSlow()`, `draw()`). States include: `cMainMenuState`, `cGamePlaying`, `cSetupSkirmishState`, `cMentatState`, `cChooseHouseState`, `cSelectYourNextConquestState`, `cWinLoseState`, etc.

State transitions go through `cGame::setNextStateToTransitionTo()` + `initiateFadingOut()`. See `m_game_implication_on_state.txt` for documented state-to-state flows.

### GameContext — Dependency Container

`src/context/GameContext.hpp` aggregates shared services passed to states and drawers: `GraphicsContext`, `TextContext`, `cTimeManager`, `cSoundPlayer`, `SDLDrawer`. States receive a `GameContext*` and use it instead of reaching into globals.

### Rendering

`src/drawers/` contains specialized drawers (one per concern: map, minimap, sidebar, particles, structures, mouse, messages, etc.). `cDrawManager` (`src/managers/cDrawManager.h`) composes them for the combat/playing state. All rendering goes through `SDLDrawer` which wraps SDL2's renderer.

Graphics assets are loaded from `.dat` files in `bin/data/` via the `Graphics` class. 
Multiple `Graphics` instances are used: `gfxdata`, `gfxinter`, `gfxworld`, `gfxmentat`, etc. (declared in `src/data/gfx*.h`).

### Game Objects

All live in `src/gameobjects/`:
- **Units** (`cUnit`, `cUnits`) — array-based pool, `cUnitInfos` holds static type data
- **Structures** (`cAbstractStructure` subclasses, `cStructures`) — each building type is a subclass (e.g. `cRefinery`, `cBarracks`, `cPalace`)
- **Projectiles** (`bullet`, `cBullets`)
- **Particles** (`cParticle`, `cParticles`)

### Players & AI

`src/player/cPlayer.h` represents each of up to 7 players (index 0 = human, 1–6 = AI). 

Each player owns a `cPlayerBrain` (`src/player/brains/`) with variants: `cPlayerBrainCampaign`, `cPlayerBrainSkirmish`, `cPlayerBrainSandworm`, `cPlayerBrainEmpty`. 

Brains are ticked every ~100 ms and execute `cPlayerBrainMission`s (attack, explore, DeathHand, Fremen, Saboteur).

### Map

`cMap` (`src/map/cMap.h`) is a flat cell array (32×32 px tiles). `cCell` holds per-cell data (terrain type, unit/structure IDs, fog-of-war). `cMapCamera` handles viewport scrolling.

### Events / Observer Pattern

Game events flow through two observer interfaces:
- `cScenarioObserver` — receives `s_GameEvent` (unit died, structure built, etc.)
- `cInputObserver` — receives `s_MouseEvent` and `cKeyboardEvent`

`cGame` implements both and re-dispatches to the active `cGameState`, `cDrawManager`, etc.

### INI Configuration

Game data (unit stats, structure costs, scenario rules) is loaded from INI files at startup via `cIniFile` / `cIniUtils`. Campaign missions are in `bin/campaign/`, skirmish maps in `bin/skirmish/`. `game.ini` in the repo root (also `resources/game.ini`) drives most gameplay parameters.

## Naming Conventions

From comments in `cGame.h`:
- Member variables: `m_camelCasedVariableName`
- Functions: `camelCased()`
- Speed-differentiated methods: `thinkFast_*()`, `thinkSlow_*()`
- State-related methods (candidates for extraction): `state_*()`
- Structs use `s_` prefix, enums use `e` prefix

## Game concepts

- This is a RTS game. Based on Dune 2; but with modernizations. (Much like Red Alert of Command & Conquer)
- A 'cell' is a tile on a map

## Code & Github
- changes should never be done directly on master or main branch, if you are on master or main branch create a new branch
- produce small PR's (a very limited set of changes)
- branch names are usually: `<prefix>/<ticket-githubnr here>/<short-description-of-feature>`
  - prefixes can be `feature` (new feature), `bugfix` or `improv` (for technical improvements usually)
-