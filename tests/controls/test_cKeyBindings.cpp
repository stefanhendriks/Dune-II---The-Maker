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
