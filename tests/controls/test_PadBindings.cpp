/**
 * @file test_PadBindings.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include <catch2/catch_test_macros.hpp>

#include "controls/PadBindings.hpp"
#include "controls/cKeyBindings.h"

TEST_CASE("PadBindings defaults bind editor undo and redo to the d-pad", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    REQUIRE(bindings.getButton(eKeyAction::EDITOR_UNDO) == SDL_GAMEPAD_BUTTON_DPAD_LEFT);
    REQUIRE(bindings.getButton(eKeyAction::EDITOR_REDO) == SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    REQUIRE(bindings.getShiftButton() == SDL_GAMEPAD_BUTTON_LEFT_STICK);
    REQUIRE(bindings.getCtrlButton() == SDL_GAMEPAD_BUTTON_RIGHT_STICK);
    REQUIRE_FALSE(bindings.getAltButton().has_value());
}

TEST_CASE("PadBindings never binds the cursor mode buttons by default", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    for (const auto &[action, button] : bindings.getActionButtons()) {
        REQUIRE(button != SDL_GAMEPAD_BUTTON_SOUTH);
        REQUIRE(button != SDL_GAMEPAD_BUTTON_EAST);
        REQUIRE(button != SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
        REQUIRE(button != SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
    }
}

TEST_CASE("PadBindings section overrides defaults, case insensitive", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    cSection section("PAD");
    section.addValue("EDITOR_UNDO", "Y");
    section.addValue("MODIFIER_ALT", "back");
    bindings.loadFromSection(section);
    REQUIRE(bindings.getButton(eKeyAction::EDITOR_UNDO) == SDL_GAMEPAD_BUTTON_NORTH);
    REQUIRE(bindings.getAltButton() == SDL_GAMEPAD_BUTTON_BACK);
    // untouched
    REQUIRE(bindings.getButton(eKeyAction::EDITOR_REDO) == SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
}

TEST_CASE("PadBindings none unbinds, unknown button unbinds", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    cSection section("PAD");
    section.addValue("EDITOR_UNDO", "none");
    section.addValue("EDITOR_REDO", "nonsense");
    section.addValue("MODIFIER_SHIFT", "none");
    bindings.loadFromSection(section);
    REQUIRE_FALSE(bindings.getButton(eKeyAction::EDITOR_UNDO).has_value());
    REQUIRE_FALSE(bindings.getButton(eKeyAction::EDITOR_REDO).has_value());
    REQUIRE_FALSE(bindings.getShiftButton().has_value());
}

TEST_CASE("PadBindings explicit binding takes the button from a default one", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    cSection section("PAD");
    section.addValue("EDITOR_UNDO", "dpright"); // EDITOR_REDO has it by default
    bindings.loadFromSection(section);
    REQUIRE(bindings.getButton(eKeyAction::EDITOR_UNDO) == SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    REQUIRE_FALSE(bindings.getButton(eKeyAction::EDITOR_REDO).has_value());
}

TEST_CASE("PadBindings explicit modifier takes the button from actions and other modifiers", "[padbindings]")
{
    PadBindings bindings;
    bindings.loadDefaults();
    cSection section("PAD");
    section.addValue("MODIFIER_SHIFT", "start"); // MENU_BACK has it by default
    section.addValue("MODIFIER_ALT", "rightstick"); // MODIFIER_CTRL has it by default
    bindings.loadFromSection(section);
    REQUIRE(bindings.getShiftButton() == SDL_GAMEPAD_BUTTON_START);
    REQUIRE_FALSE(bindings.getButton(eKeyAction::MENU_BACK).has_value());
    REQUIRE(bindings.getAltButton() == SDL_GAMEPAD_BUTTON_RIGHT_STICK);
    REQUIRE_FALSE(bindings.getCtrlButton().has_value());
}
