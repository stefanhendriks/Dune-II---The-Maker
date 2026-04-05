#pragma once

#include <string>
#include <set>
#include <format>

#include "utils/cPoint.h"
#include "utils/common.h"
#include "controls/eKeyboardEnum.h"

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, const std::set<SDL_Scancode> &keys, const s_KeysCombo &combo);

    eKeyEventType eventType = eKeyEventType::NONE;

    inline const std::string toString() const {
        std::string str= std::format("cKeyboardEvent [type={}], keys: ", toStringKeyboardEventType(eventType));
        str.append("[");
        for (auto aKey : keys) {
            str.append(SDL_GetScancodeName(aKey));
        }
        str.append("]");

        return str;
    }

    bool hasKey(SDL_Scancode scanCode) const {
        return keys.find(scanCode) != keys.end();
    }

    /**
     * Returns true when both scancodes are present in this event
     * @param firstScanCode
     * @param secondScanCode
     * @return
     */
    bool hasKeys(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const {
        return hasKey(firstScanCode) && hasKey(secondScanCode);
    }

    /**
     * Returns true when one of both scancodes are present in this event
     * @param firstScanCode
     * @param secondScanCode
     * @return
     */
    bool hasEitherKey(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const {
        return hasKey(firstScanCode) || hasKey(secondScanCode);
    }

    bool isType(eKeyEventType type) const {
        return eventType == type;
    }

    bool isPrintable() const;
    bool isBackspace() const;
    bool isEnter() const;
    bool isShiftPressed() const;
    bool isAltPressed() const;
    bool isCtrlPressed() const;
    char getChar() const;

    /**
     * If a numerical key is pressed, return that value.
     * @return
     */
    int getGroupNumber() const;

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

    const std::set<SDL_Scancode> &keys;
    const s_KeysCombo &combo;
};
