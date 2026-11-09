/**
 * @file test_cKeyboardEvent.cpp
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
#include "controls/cKeyboardEvent.h"
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

// cKeyboardEvent keeps references to its keys and combo, so they must outlive the event.
struct EventHolder {
    Keys keys;
    s_KeysCombo combo;
    cKeyboardEvent event;

    EventHolder(eKeyEventType type, const Keys &k, const s_KeysCombo &c, const cKeyBindings *bindings,
                const std::string &text = "")
        : keys(k), combo(c), event(type, keys, combo, bindings, text)
    {
    }
    EventHolder(const EventHolder &) = delete;
    EventHolder &operator=(const EventHolder &) = delete;
};
}

TEST_CASE("cKeyboardEvent - reports its type", "[keyboardevent]")
{
    EventHolder h(eKeyEventType::PRESSED, Keys{}, s_KeysCombo{}, nullptr);
    REQUIRE(h.event.getType() == eKeyEventType::PRESSED);
    REQUIRE(h.event.isType(eKeyEventType::PRESSED));
    REQUIRE_FALSE(h.event.isType(eKeyEventType::HOLD));
}

TEST_CASE("cKeyboardEvent - key helpers look at the pressed keys", "[keyboardevent]")
{
    Keys keys = pressed(SDL_SCANCODE_A);
    keys.set(static_cast<size_t>(SDL_SCANCODE_B));
    EventHolder h(eKeyEventType::HOLD, keys, s_KeysCombo{}, nullptr);
    const auto &event = h.event;

    REQUIRE(event.hasKey(SDL_SCANCODE_A));
    REQUIRE_FALSE(event.hasKey(SDL_SCANCODE_C));
    REQUIRE(event.hasKeys(SDL_SCANCODE_A, SDL_SCANCODE_B));
    REQUIRE_FALSE(event.hasKeys(SDL_SCANCODE_A, SDL_SCANCODE_C));
    REQUIRE(event.hasEitherKey(SDL_SCANCODE_C, SDL_SCANCODE_B));
    REQUIRE_FALSE(event.hasEitherKey(SDL_SCANCODE_C, SDL_SCANCODE_D));
}

TEST_CASE("cKeyboardEvent - hasKey rejects out of range scancodes", "[keyboardevent]")
{
    EventHolder h(eKeyEventType::HOLD, Keys{}, s_KeysCombo{}, nullptr);
    REQUIRE_FALSE(h.event.hasKey(static_cast<SDL_Scancode>(-1)));
    REQUIRE_FALSE(h.event.hasKey(static_cast<SDL_Scancode>(SDL_SCANCODE_COUNT)));
}

TEST_CASE("cKeyboardEvent - backspace and enter detection", "[keyboardevent]")
{
    EventHolder backspace(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_BACKSPACE), s_KeysCombo{}, nullptr);
    EventHolder enter(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_RETURN), s_KeysCombo{}, nullptr);
    EventHolder keypadEnter(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_KP_ENTER), s_KeysCombo{}, nullptr);
    EventHolder other(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_A), s_KeysCombo{}, nullptr);

    REQUIRE(backspace.event.isBackspace());
    REQUIRE(enter.event.isEnter());
    REQUIRE(keypadEnter.event.isEnter());
    REQUIRE_FALSE(other.event.isEnter());
    REQUIRE_FALSE(other.event.isBackspace());
}

TEST_CASE("cKeyboardEvent - modifier and text input accessors", "[keyboardevent]")
{
    s_KeysCombo combo;
    combo.shiftPressed = true;
    combo.ctrlPressed = true;
    EventHolder h(eKeyEventType::PRESSED, Keys{}, combo, nullptr, "abc");

    REQUIRE(h.event.isShiftPressed());
    REQUIRE(h.event.isCtrlPressed());
    REQUIRE_FALSE(h.event.isAltPressed());
    REQUIRE(h.event.hasTextInput());
    REQUIRE(h.event.getTextInput() == "abc");

    EventHolder plain(eKeyEventType::PRESSED, Keys{}, s_KeysCombo{}, nullptr);
    REQUIRE_FALSE(plain.event.hasTextInput());
}

TEST_CASE("cKeyboardEvent - without key bindings no action matches", "[keyboardevent]")
{
    EventHolder h(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_H), s_KeysCombo{}, nullptr);
    REQUIRE_FALSE(h.event.isAction(eKeyAction::CENTER_ON_HOME));
    REQUIRE_FALSE(h.event.involvesActionKey(eKeyAction::CENTER_ON_HOME));
}

TEST_CASE("cKeyboardEvent - isAction and involvesActionKey use the key bindings", "[keyboardevent]")
{
    const auto bindings = defaults();
    EventHolder h(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_H), s_KeysCombo{}, &bindings);
    REQUIRE(h.event.isAction(eKeyAction::CENTER_ON_HOME));
    REQUIRE(h.event.involvesActionKey(eKeyAction::CENTER_ON_HOME));
    REQUIRE_FALSE(h.event.isAction(eKeyAction::TOGGLE_MUSIC));
}

TEST_CASE("cKeyboardEvent - group numbers depend on the held modifier", "[keyboardevent]")
{
    const auto bindings = defaults();
    const Keys one = pressed(SDL_SCANCODE_1);

    s_KeysCombo ctrl;
    ctrl.ctrlPressed = true;
    s_KeysCombo shift;
    shift.shiftPressed = true;

    EventHolder plain(eKeyEventType::PRESSED, one, s_KeysCombo{}, &bindings);
    REQUIRE(plain.event.getGroupNumber() == 1);
    REQUIRE(plain.event.getAssignGroupNumber() == 0);
    REQUIRE(plain.event.getAddToSelGroupNumber() == 0);

    EventHolder withCtrl(eKeyEventType::PRESSED, one, ctrl, &bindings);
    REQUIRE(withCtrl.event.getGroupNumber() == 0);
    REQUIRE(withCtrl.event.getAssignGroupNumber() == 1);

    EventHolder withShift(eKeyEventType::PRESSED, one, shift, &bindings);
    REQUIRE(withShift.event.getGroupNumber() == 0);
    REQUIRE(withShift.event.getAddToSelGroupNumber() == 1);
}

TEST_CASE("cKeyboardEvent - viewport bookmark numbers depend on the held modifier", "[keyboardevent]")
{
    const auto bindings = defaults();
    const Keys f1 = pressed(SDL_SCANCODE_F1);

    s_KeysCombo alt;
    alt.altPressed = true;

    EventHolder jump(eKeyEventType::PRESSED, f1, s_KeysCombo{}, &bindings);
    REQUIRE(jump.event.getJumpToViewportBookmarkNumber() == 1);
    REQUIRE(jump.event.getSaveViewportBookmarkNumber() == 0);

    EventHolder save(eKeyEventType::PRESSED, f1, alt, &bindings);
    REQUIRE(save.event.getSaveViewportBookmarkNumber() == 1);
    REQUIRE(save.event.getJumpToViewportBookmarkNumber() == 0);
}

TEST_CASE("cKeyboardEvent - toString names the type and the text input", "[keyboardevent]")
{
    EventHolder h(eKeyEventType::PRESSED, pressed(SDL_SCANCODE_A), s_KeysCombo{}, nullptr, "a");
    const std::string text = h.event.toString();
    REQUIRE(text.find("PRESSED") != std::string::npos);
    REQUIRE(text.find("text: [a]") != std::string::npos);
}
