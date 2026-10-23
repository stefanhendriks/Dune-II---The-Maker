#include <catch2/catch_test_macros.hpp>
#include "controls/cKeyBindings.h"

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
