#pragma once

#include <string>
#include <set>

#include "utils/cPoint.h"
#include "enums.h"
#include "utils/common.h"

#define HAVE_STDINT_H
#include <allegro/keyboard.h>

enum class eKeyEventType {
    NONE,
    HOLD,    // key is being pressed down (hold)
    PRESSED, // key is held, then released. Makes a key "pressed".
};

class cKeyboardEvent {

public:
    cKeyboardEvent(eKeyEventType eventType, std::set<int> & keys);

    eKeyEventType eventType = eKeyEventType::NONE;

    inline const std::string toString(const cKeyboardEvent &event) {
        std::string str;
        char partOne[64];
        sprintf(partOne, "cKeyboardEvent [type=%s], keys: ",
                toStringKeyboardEventType(event.eventType)
        );

        str.append(partOne);
        str.append("[");
        for (auto key : event.keys) {
            str.append(std::to_string(key));
            str.append("=");
            str.append(std::to_string(scancode_to_ascii(key)));
            str.append(" ");
        }
        str.append("]");

        return str;
    }

    bool hasKey(int scanCode) const {
        return keys.find(scanCode) != keys.end();
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
    inline const char* toStringKeyboardEventType(const eKeyEventType &type) {
        switch (type) {
            case eKeyEventType::NONE: return "NONE";
            case eKeyEventType::HOLD: return "HOLD";
            case eKeyEventType::PRESSED: return "PRESSED";
            default:
                assert(false);
                break;
        }
        return "";
    }

    std::set<int> keys;
};
