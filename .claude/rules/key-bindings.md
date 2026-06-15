# Key Bindings

When adding or changing any key binding, update all of these files before committing:

- `keys.md` (repo root) — human-readable reference for players
- `resources/bin/settings.ini` — the source settings file (`bin/` is gitignored, so only this one needs updating)

Key bindings that exist in code but are missing from `settings.ini` cannot be remapped by players. Missing entries in `keys.md` leave players without documentation.
