#ifndef D2TM_SKEYBOARDEVENT_H
#define D2TM_SKEYBOARDEVENT_H

#include "allegroh.h"

#include <string>
#include "../utils/cPoint.h"
#include "enums.h"

struct s_KeyboardEvent {
    eKeyboardEventType eventType = eKeyboardEventType::KEY_NONE;
    int key; // TODO: Make this a vector/array of all keys pressed

    static const std::string toString(const s_KeyboardEvent &event) {
        char msg[255];
        sprintf(msg, "s_KeyboardEvent [type=%s], [key=%d (=%c)]",
                toStringKeyboardEventType(event.eventType),
                event.key,
                scancode_to_ascii(event.key)
        );
        return std::string(msg);
    }

    static const char* toStringKeyboardEventType(const eKeyboardEventType &eventType) {
        switch (eventType) {
            case eKeyboardEventType::KEY_NONE: return "KEY_NONE";
            case eKeyboardEventType::KEY_HOLD: return "KEY_HOLD";
            case eKeyboardEventType::KEY_PRESSED: return "KEY_PRESSED";
            default:
                assert(false);
                break;
        }
        return "";
    }
};

#endif //D2TM_SKEYBOARDEVENT_H
