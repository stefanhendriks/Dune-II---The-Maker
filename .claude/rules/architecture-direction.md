# Architecture Direction

Active architectural migrations and the conventions that go with them. Both
coding agents and the PR review checklist (`.claude/review.md`) should treat
this as current direction, not just historical trivia.

## Removing cGameInterface (#1476)

`cGameInterface` is a legacy facade with no polymorphic purpose — it just
forwards to `cGame`. The direction is to remove it entirely, migrating each
dependency onto `sGameServices` (or a more specific narrow service/class)
instead, one small PR at a time.

**New code should not add new `cGameInterface` dependencies.** If a class
needs something `cGameInterface` currently provides, reach for
`sGameServices` or the specific underlying service/class first. This has not
been consistently followed so far — e.g. the scoring screen (#1454) added
several new `cGameInterface` call sites — so flag new usages in review, not
just migrations of old ones.

## Caching sGameServices fields: no "train-wreck" chains

When a class needs a narrow dependency available on `sGameServices`, cache it
as its own typed member (set in `serviceInit()`, or the constructor when
safe — see below), and call it directly:

```cpp
// correct
m_eventEmitter = services->eventEmitter;
...
m_eventEmitter->emit(event);

// wrong
m_services->eventEmitter->emit(event);
```

Do this even if the class already caches `m_services` as a whole pointer for
other reasons (e.g. to forward it on to another object's `serviceInit()`).

## Construction-order safety for sGameServices fields

Before caching an `sGameServices` field, confirm *when* `cGame::setupGame()`
actually populates it, relative to when the caching code runs:

- Fields populated early in `setupGame()` (`settings`, `structureUtils`,
  `eventEmitter`, `mouse`, `missionStatsCollector`) are safe to cache
  anywhere, including a constructor.
- Fields populated late (`mapCamera`, `drawManager`) are NOT safe to cache in
  a constructor that runs before that point (e.g. `cIni`, which is
  constructed very early). Classes constructed early must read these live
  through the stored `sGameServices*` pointer at the call site instead of
  caching them, unless they have a later `serviceInit()`-style hook
  guaranteed to run after `setupGame()` finishes populating them.

Don't assume a field's timing — check where `cGame::setupGame()` assigns it
before deciding whether to cache it or read it live.

## Context-style service classes should stay narrow

`AudioContext` deliberately has zero dependency on game-object/world types
(no `cMapCamera`, no `cGameObjectContext`). World-position/volume math stays
on the caller. Any future `*Context`-style service class should default to
the same narrowness rather than growing a dependency on higher-level
game/world types — that's what keeps these services easy to reason about and
safe to cache (see above).

## Modernizing legacy C-style code

Modernizing a legacy C-style file to idiomatic C++23 is a deliberate,
maintainer-approved exception to the surgical-changes default in `CLAUDE.md`
§3. It happens as its own dedicated change — see the `ini.cpp`/`ini.h`
modernization (#1389) for the kind of transformation this means in practice
— not bundled into an unrelated fix or feature PR. Don't take touching a
file for another reason as license to modernize it along the way; that's
still the drive-by-refactor case §3 flags.

## New source files

All `src/` files get the standard MIT license header (see any existing file
for the exact block). This applies to every new file, not just the ones
that already have it.
