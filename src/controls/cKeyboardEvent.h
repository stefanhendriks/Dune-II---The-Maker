#pragma once

#include <string>
#include <set>
#include <format>

#include "utils/cPoint.h"
#include "utils/common.h"
#include "controls/eKeyAction.h"
#include "controls/eKeyboardEnum.h"

class cKeyBindings;

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, const std::set<SDL_Scancode> &keys, const s_KeysCombo &combo,
                   const cKeyBindings *keyBindings);

    inline const std::string toString() const {
        std::string str= std::format("cKeyboardEvent [type={}], keys: ", toStringKeyboardEventType(m_eventType));
        str.append("[");
        for (auto aKey : m_keys) {
            str.append(SDL_GetScancodeName(aKey));
        }
        str.append("]");

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

    bool isPrintable() const;
    bool isBackspace() const;
    bool isEnter() const;
    bool isShiftPressed() const;
    bool isAltPressed() const;
    bool isCtrlPressed() const;
    char getChar() const;

    /**
     * Returns the group number (1–5) if a group key is pressed according to the configured bindings, 0 otherwise.
     */
    int getGroupNumber() const;

    // Raw key checks — kept public during migration to isAction(); will be removed once all callers are converted.
    bool hasKey(SDL_Scancode scanCode) const {
        return m_keys.find(scanCode) != m_keys.end();
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
                assert(false);
                break;
        }
        return "";
    }

    eKeyEventType m_eventType = eKeyEventType::NONE;
    const std::set<SDL_Scancode> &m_keys;
    const s_KeysCombo &m_combo;
    const cKeyBindings *m_keyBindings;
};
