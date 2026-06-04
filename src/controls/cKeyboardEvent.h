#pragma once

#include <SDL3/SDL.h>
#include <bitset>
#include <string>

#include "controls/eKeyAction.h"
#include "controls/eKeyboardEnum.h"

class cKeyBindings;

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, const std::bitset<SDL_SCANCODE_COUNT> &keys, const s_KeysCombo &combo,
                const cKeyBindings *keyBindings, std::string textInput = "");

    const std::string toString() const;

    eKeyEventType getType() const;

    bool isType(eKeyEventType type) const;

    /**
     * Returns true when the event matches the given game action, according to the configured key bindings.
     */
    bool isAction(eKeyAction action) const;

    bool isBackspace() const;
    bool isEnter() const;
    bool isShiftPressed() const;
    bool isAltPressed() const;
    bool isCtrlPressed() const;
    bool hasTextInput() const;
    const std::string &getTextInput() const;

    /**
     * Returns the group number (1–6) if a plain group key is pressed (no modifier), 0 otherwise.
     */
    int getGroupNumber() const;

    /**
     * Returns the group number (1–6) if CTRL+number is pressed (group assignment), 0 otherwise.
     */
    int getAssignGroupNumber() const;

    /**
     * Returns the group number (1–6) if SHIFT+number is pressed (add group to selection), 0 otherwise.
     */
    int getAddToSelGroupNumber() const;

    // Raw key checks — kept public during migration to isAction(); will be removed once all callers are converted.
    bool hasKey(SDL_Scancode scanCode) const;

    bool hasKeys(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const;

    bool hasEitherKey(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const;

private:
    eKeyEventType m_eventType = eKeyEventType::NONE;
    const std::bitset<SDL_SCANCODE_COUNT> &m_keys;
    const s_KeysCombo &m_combo;
    const cKeyBindings *m_keyBindings;
    std::string m_textInput;
};
