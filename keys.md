# Key Bindings

## Global (any state)

| Key | Action |
|-----|--------|
| F11 | Take screenshot |
| M / MUTE | Toggle music on/off |
| O / VOLUME DOWN | Decrease music volume |
| P / VOLUME UP | Increase music volume |
| KP + | Increase game speed |
| KP - | Decrease game speed |
| ALT + ENTER | Toggle fullscreen |
| ALT + L | Toggle fullscreen scaling mode (integer scale / letterbox) |
| ALT + C | Toggle cheat/debug mode |

---

## Game Playing — General

| Key | Action |
|-----|--------|
| ESCAPE | Open options menu |
| H | Center view on player's focus cell |
| C | Center view on selected structure |
| Z | Reset zoom level |
| Arrow keys (hold) | Scroll map |
| T | Track selected units (press again or scroll map to stop) |
| F (hold) | Show FPS counter |
| ALT + F1–F5 | Save current viewport position/zoom to bookmark 1–5 |
| F1–F5 | Jump to viewport bookmark 1–5 (no-op if that bookmark was never saved) |

---

## Game Playing — Units & Structures

| Key | Action |
|-----|--------|
| R | Toggle repair mode |
| D | Harvester: return to refinery / Repair pad: deploy unit |
| P | Set structure as primary |
| F (units selected) | Retreat selected units to base |
| CTRL + 1–5 | Assign/select unit group 1–5 |
| SHIFT + 1–6 (units selected) | Add group to current selection |
| CTRL + Z (1 unit selected) | Select all of same unit type on screen |
| CTRL (hold, units selected) | Switch mouse to attack mode |
| SHIFT (hold, units selected) | Append to selection |
| SHIFT (hold, build mode) | Enable build multiplier |

---

## Game Playing — Debug Mode

| Key | Action |
|-----|--------|
| TAB | Cycle debug visualization mode |
| G (hold) | Show grid overlay |
| D (hold, + TAB) | Show map without shroud |
| SHIFT + TAB + F2 | Win mission |
| SHIFT + TAB + F3 | Lose mission |
| SHIFT + TAB + F4 | Give 5000 credits to all players |
| SHIFT + TAB + F5 | Reveal entire map |
| SHIFT + TAB + F6 | Kill all carry-alls |
| SHIFT + TAB + F7 | Destroy unit/structure at mouse cell |
| SHIFT + TAB + F8 | Spawn enemy ornithopter at mouse cell |
| SHIFT + TAB + F9 | Spawn sandworm at mouse cell |
| SHIFT + TAB + F10 | Damage unit at mouse cell (→ 25 HP) |
| SHIFT + TAB + F12 | Clear shroud at mouse cell |
| TAB + 0–3 (hold) | Switch camera/control to player 0–3 |

---

## Editor State

| Key | Action |
|-----|--------|
| ESCAPE | Return to main menu |
| S | Save map |
| Arrow keys (hold) | Scroll camera |
| PAGE UP / SHIFT + UP | Zoom in |
| PAGE DOWN / SHIFT + DOWN | Zoom out |

---

## Menu / Other States

| Key | State | Action |
|-----|-------|--------|
| ESCAPE | Main Menu | Exit game |
| ESCAPE | Credits | Return to main menu |
| ESCAPE | Choose House | Return to main menu |
| ESCAPE | Setup Skirmish | Return to main menu |
| ESCAPE | Select Conquest | Return to options |
| Arrow keys | Setup Skirmish | Move map cursor (wraps rows, changes page at edges) |
| ENTER | Setup Skirmish | Start game with selected map |

---

## Gamepad (cursor mode)

Active whenever a gamepad is connected. Mouse and keyboard keep working alongside it.
Speed is set with `GamepadCursorSpeed` in `settings.ini`. Edge scrolling is off while the pad drives the cursor.

| Button | Action |
|--------|--------|
| Left stick | Move the cursor |
| South (A) | Left click (hold and move to box select or drag) |
| East (B) | Right click |
| Right shoulder | Mouse wheel up (zoom in) |
| Left shoulder | Mouse wheel down (zoom out) |

### Gamepad key actions

A pad button triggers the action of a key, so it does exactly what that key does. Bindings are set in the
`[PAD]` section of `settings.ini` (action names as in `[KEYS]`, button names as SDL knows them, `none` to unbind).

| Button | Action |
|--------|--------|
| Start | MENU_BACK (Escape) |
| West (X) | MENU_CONFIRM (Enter) |
| North (Y) | CENTER_ON_HOME |
| D-pad up / down | EDITOR_ZOOM_IN / EDITOR_ZOOM_OUT |
| D-pad left / right | EDITOR_UNDO / EDITOR_REDO |
| Back | EDITOR_SAVE |
| Left stick click (hold) | Acts as Shift (`MODIFIER_SHIFT`) |
| Right stick click (hold) | Acts as Ctrl (`MODIFIER_CTRL`) |
| (unbound) | `MODIFIER_ALT` |
