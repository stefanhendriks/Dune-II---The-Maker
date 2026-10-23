/**
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 */

#include <catch2/catch_test_macros.hpp>
#include <bitset>
#include "controls/cKeyBindings.h"

namespace {
using Keys = std::bitset<SDL_SCANCODE_COUNT>;

Keys pressed(SDL_Scancode sc)
{
    Keys k;
    k.set(static_cast<size_t>(sc));
    return k;
}

cKeyBindings defaults()
{
    cKeyBindings bindings;
    bindings.loadDefaults();
    return bindings;
}
}

TEST_CASE("cKeyBindings::matches - bound key matches without modifiers", "[keybindings]")
{
    auto b = defaults();
    REQUIRE(b.matches(pressed(SDL_SCANCODE_H), s_KeysCombo{}, eKeyAction::CENTER_ON_HOME));
}

TEST_CASE("cKeyBindings::matches - unbound key does not match", "[keybindings]")
{
    auto b = defaults();
    REQUIRE_FALSE(b.matches(pressed(SDL_SCANCODE_X), s_KeysCombo{}, eKeyAction::CENTER_ON_HOME));
    REQUIRE_FALSE(b.matches(Keys{}, s_KeysCombo{}, eKeyAction::CENTER_ON_HOME));
}

TEST_CASE("cKeyBindings::matches - action without any binding never matches", "[keybindings]")
{
    cKeyBindings empty;
    REQUIRE_FALSE(empty.matches(pressed(SDL_SCANCODE_H), s_KeysCombo{}, eKeyAction::CENTER_ON_HOME));
}

TEST_CASE("cKeyBindings::matches - either alternative key of a binding matches", "[keybindings]")
{
    auto b = defaults();
    REQUIRE(b.matches(pressed(SDL_SCANCODE_M), s_KeysCombo{}, eKeyAction::TOGGLE_MUSIC));
    REQUIRE(b.matches(pressed(SDL_SCANCODE_MUTE), s_KeysCombo{}, eKeyAction::TOGGLE_MUSIC));
}

TEST_CASE("cKeyBindings::matches - required modifier must be held", "[keybindings]")
{
    auto b = defaults();
    s_KeysCombo alt;
    alt.altPressed = true;
    REQUIRE_FALSE(b.matches(pressed(SDL_SCANCODE_RETURN), s_KeysCombo{}, eKeyAction::TOGGLE_FULLSCREEN));
    REQUIRE(b.matches(pressed(SDL_SCANCODE_RETURN), alt, eKeyAction::TOGGLE_FULLSCREEN));
}

TEST_CASE("cKeyBindings::matches - unrequired modifier blocks the binding", "[keybindings]")
{
    // regression for #1009: a held modifier must not trigger the plain-key binding
    auto b = defaults();
    s_KeysCombo shift;
    shift.shiftPressed = true;
    s_KeysCombo ctrl;
    ctrl.ctrlPressed = true;
    REQUIRE_FALSE(b.matches(pressed(SDL_SCANCODE_H), shift, eKeyAction::CENTER_ON_HOME));
    REQUIRE_FALSE(b.matches(pressed(SDL_SCANCODE_H), ctrl, eKeyAction::CENTER_ON_HOME));
}

TEST_CASE("cKeyBindings::matches - modifier key itself can be the bound key", "[keybindings]")
{
    auto b = defaults();
    s_KeysCombo ctrl;
    ctrl.ctrlPressed = true;
    s_KeysCombo shift;
    shift.shiftPressed = true;
    REQUIRE(b.matches(pressed(SDL_SCANCODE_LCTRL), ctrl, eKeyAction::ATTACK_MODE));
    REQUIRE(b.matches(pressed(SDL_SCANCODE_RSHIFT), shift, eKeyAction::ADD_TO_SELECTION));
}

TEST_CASE("cKeyBindings::matches - same key with and without ctrl are different actions", "[keybindings]")
{
    auto b = defaults();
    s_KeysCombo ctrl;
    ctrl.ctrlPressed = true;
    REQUIRE(b.matches(pressed(SDL_SCANCODE_Z), ctrl, eKeyAction::SELECT_SAME_TYPE_ON_MAP));
    REQUIRE_FALSE(b.matches(pressed(SDL_SCANCODE_Z), ctrl, eKeyAction::ZOOM_RESET));
    REQUIRE(b.matches(pressed(SDL_SCANCODE_Z), s_KeysCombo{}, eKeyAction::ZOOM_RESET));
}

namespace {
std::bitset<SDL_SCANCODE_COUNT> keysOf(std::initializer_list<SDL_Scancode> scancodes)
{
    std::bitset<SDL_SCANCODE_COUNT> keys;
    for (SDL_Scancode sc : scancodes) {
        keys.set(static_cast<size_t>(sc));
    }
    return keys;
}
}

TEST_CASE("cKeyBindings::containsActionKey - true for any key bound to the action", "[keybindings]")
{
    cKeyBindings bindings;
    bindings.loadDefaults();

    REQUIRE(bindings.containsActionKey(keysOf({SDL_SCANCODE_LSHIFT}), eKeyAction::ADD_TO_SELECTION));
    REQUIRE(bindings.containsActionKey(keysOf({SDL_SCANCODE_RSHIFT}), eKeyAction::ADD_TO_SELECTION));
    REQUIRE(bindings.containsActionKey(keysOf({SDL_SCANCODE_LCTRL}), eKeyAction::ATTACK_MODE));
}

TEST_CASE("cKeyBindings::containsActionKey - false when the action's key is absent", "[keybindings]")
{
    cKeyBindings bindings;
    bindings.loadDefaults();

    REQUIRE_FALSE(bindings.containsActionKey(keysOf({}), eKeyAction::ADD_TO_SELECTION));
    REQUIRE_FALSE(bindings.containsActionKey(keysOf({SDL_SCANCODE_LCTRL}), eKeyAction::ADD_TO_SELECTION));
}

TEST_CASE("cKeyBindings::containsActionKey - false for an action without a binding", "[keybindings]")
{
    cKeyBindings bindings;

    REQUIRE_FALSE(bindings.containsActionKey(keysOf({SDL_SCANCODE_LSHIFT}), eKeyAction::ADD_TO_SELECTION));
}

TEST_CASE("cKeyBindings::containsActionKey - ignores modifiers that make matches() refuse", "[keybindings]")
{
    cKeyBindings bindings;
    bindings.loadDefaults();

    // Shift released while Alt is still held: the event still contains the Shift key.
    const auto keys = keysOf({SDL_SCANCODE_LSHIFT, SDL_SCANCODE_LALT});
    const s_KeysCombo combo{.altPressed = true, .ctrlPressed = false, .shiftPressed = false};
    REQUIRE_FALSE(bindings.matches(keys, combo, eKeyAction::ADD_TO_SELECTION));
    REQUIRE(bindings.containsActionKey(keys, eKeyAction::ADD_TO_SELECTION));
}
