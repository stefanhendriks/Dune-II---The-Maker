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
| 1–6 | Select unit group 1–6 |
| CTRL + 1–6 | Assign selected units to group 1–6 |
| SHIFT + 1–6 (units selected) | Add group to current selection |
| CTRL + Z (1 unit selected) | Select all of same unit type on screen |
| CTRL + Z | Select all units of the same type on the map (see #1522: `settings.ini` also binds the on-screen variant to CTRL + Z) |
| CTRL + I / SHIFT + I | Select all infantry on the map / on screen |
| CTRL + W / SHIFT + W | Select all wheeled units on the map / on screen |
| CTRL + T / SHIFT + T | Select all tanks on the map / on screen |
| CTRL + L / SHIFT + L | Select all rocket launchers on the map / on screen |
| CTRL + H / SHIFT + H | Select all harvesters on the map / on screen |
| CTRL + A | Select all attacking units on the map |
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
| CTRL + S | Save map |
| Arrow keys (hold) | Scroll camera |
| PAGE UP / SHIFT + UP | Zoom in |
| PAGE DOWN / SHIFT + DOWN | Zoom out |
| G | Show grid overlay |
| A | Show axes |
| CTRL + Z / CTRL + Y | Undo / redo |
| CTRL + C / CTRL + X / CTRL + V | Copy / cut / paste |

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
Edge scrolling is off while the pad drives the cursor.

Tunable in `settings.ini` (a missing or out of range value falls back to the default):

| Setting | Default | Range | Meaning |
|---------|---------|-------|---------|
| `GamepadCursorSpeed` | 600 | 1 to 10000 | Cursor speed in pixels per second at full stick tilt |
| `GamepadDeadzone` | 0.2 | 0 to 0.9 | Stick tilt that is ignored (both sticks) |
| `GamepadCursorCurve` | 2 | 0.1 to 5 | Cursor stick response: 1 is linear, higher is more precise near the center |
| `GamepadScrollThreshold` | 0.5 | 0.05 to 0.95 | Right stick tilt (after the deadzone) at which the map scrolls |

| Button | Action |
|--------|--------|
| Left stick | Move the cursor |
| Right stick | Scroll the map (holds the `SCROLL_*` keys, same speed as the keyboard or a screen edge) |
| South (A) | Left click (hold and move to box select or drag) |
| East (B) | Right click |
| Right shoulder | Mouse wheel up (zoom in) |
| Left shoulder | Mouse wheel down (zoom out) |

The right stick holds the `SCROLL_*` keys itself, so do not bind `SCROLL_*` actions to pad buttons in `[PAD]`: the stick and the button would release each other's key.

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
