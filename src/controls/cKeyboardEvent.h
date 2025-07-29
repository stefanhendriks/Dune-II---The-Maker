#pragma once

#include <string>
#include <set>
#include <format>

#include "utils/cPoint.h"
#include "enums.h"
#include "utils/common.h"

enum class eKeyEventType {
    NONE,
    /**
     * key is being pressed down (hold)
     */
    HOLD,
    /**
     * key is held, then released. Makes a key "pressed".
     */
    PRESSED,
};

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, std::set<SDL_Scancode> &keys);

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

    std::set<SDL_Scancode> keys;
};
