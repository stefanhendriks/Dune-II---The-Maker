#pragma once

#include <string>
#include <bitset>
#include <format>

#include "utils/cPoint.h"
#include "utils/common.h"
#include "controls/eKeyAction.h"
#include "controls/eKeyboardEnum.h"

class cKeyBindings;

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, const std::bitset<SDL_NUM_SCANCODES> &keys, const s_KeysCombo &combo,
                const cKeyBindings *keyBindings, std::string textInput = "");

    inline const std::string toString() const {
        std::string str= std::format("cKeyboardEvent [type={}], keys: ", toStringKeyboardEventType(m_eventType));
        str.append("[");
        for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc) {
            if (!m_keys.test(static_cast<size_t>(sc))) continue;
            str.append(SDL_GetScancodeName(static_cast<SDL_Scancode>(sc)));
        }
        str.append("]");
        if (!m_textInput.empty()) {
            str.append(std::format(", text: [{}]", m_textInput));
        }

        return str;
    }

    eKeyEventType getType() const {
        return m_eventType;
    }

    bool isType(eKeyEventType type) const {
        return m_eventType == type;
    }

    /**
     * Returns true when the event matches the given game action, according to the configured key bindings.
     */
    bool isAction(eKeyAction action) const;

    // bool isPrintable() const;
    bool isBackspace() const;
    bool isEnter() const;
    bool isShiftPressed() const;
    bool isAltPressed() const;
    bool isCtrlPressed() const;
    bool hasTextInput() const;
    const std::string &getTextInput() const;
    // char getChar() const;

    /**
     * Returns the group number (1–5) if a group key is pressed according to the configured bindings, 0 otherwise.
     */
    int getGroupNumber() const;

    // Raw key checks — kept public during migration to isAction(); will be removed once all callers are converted.
    bool hasKey(SDL_Scancode scanCode) const {
        if (scanCode < 0 || scanCode >= SDL_NUM_SCANCODES) return false;
        return m_keys.test(static_cast<size_t>(scanCode));
    }

    bool hasKeys(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const {
        return hasKey(firstScanCode) && hasKey(secondScanCode);
    }

    bool hasEitherKey(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const {
        return hasKey(firstScanCode) || hasKey(secondScanCode);
    }

private:
    inline const char *toStringKeyboardEventType(const eKeyEventType &type) const {
        switch (type) {
            case eKeyEventType::NONE:
                return "NONE";
            case eKeyEventType::HOLD:
                return "HOLD";
            case eKeyEventType::PRESSED:
                return "PRESSED";
            default:
                d2tm_assert(false);
                break;
        }
        return "";
    }

    eKeyEventType m_eventType = eKeyEventType::NONE;
    const std::bitset<SDL_NUM_SCANCODES> &m_keys;
    const s_KeysCombo &m_combo;
    const cKeyBindings *m_keyBindings;
    std::string m_textInput;
};
