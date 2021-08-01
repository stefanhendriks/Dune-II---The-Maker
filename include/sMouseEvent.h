#ifndef D2TM_SMOUSEEVENT_H
#define D2TM_SMOUSEEVENT_H

#include "enums.h"

struct s_MouseEvent {
    eMouseEventType eventType = eMouseEventType::MOUSE_NONE;
    int x, y, z = 0;

    static const char* toString(const eMouseEventType &eventType) {
        switch (eventType) {
            case eMouseEventType::MOUSE_MOVED_TO: return "MOUSE_MOVED_TO";
            case eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED: return "MOUSE_RIGHT_BUTTON_CLICKED";
            case eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED: return "MOUSE_RIGHT_BUTTON_PRESSED";
            case eMouseEventType::MOUSE_SCROLLED_DOWN: return "MOUSE_SCROLLED_DOWN";
            case eMouseEventType::MOUSE_SCROLLED_UP: return "MOUSE_SCROLLED_UP";
            case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED: return "MOUSE_LEFT_BUTTON_CLICKED";
            case eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED: return "MOUSE_LEFT_BUTTON_PRESSED";
            case eMouseEventType::MOUSE_NONE: return "MOUSE_NONE";
            default:
                assert(false);
                break;
        }
        return "";
    }
};

#endif //D2TM_SMOUSEEVENT_H
