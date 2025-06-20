#pragma once

#include "enums.h"
#include "utils/cPoint.h"

#include <cassert>
#include <string>
#include <fmt/core.h>

// Rename to GUI_EVENT? Might be more appropriate
struct s_MouseEvent {
    eMouseEventType eventType = eMouseEventType::MOUSE_NONE;
    cPoint coords;
    int z = 0;

    static const std::string toString(const s_MouseEvent &event) {
        std::string msg = fmt::format("s_MouseEvent [type={}], [x={}], [y={}], [z={}]",
                                      toStringMouseEventType(event.eventType),
                                      event.coords.x,
                                      event.coords.y,
                                      event.z
                                     );
        return msg;
    }

    static const char *toStringMouseEventType(const eMouseEventType &eventType) {
        switch (eventType) {
            case eMouseEventType::MOUSE_MOVED_TO:
                return "MOUSE_MOVED_TO";
            case eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED:
                return "MOUSE_RIGHT_BUTTON_CLICKED";
            case eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED:
                return "MOUSE_RIGHT_BUTTON_PRESSED";
            case eMouseEventType::MOUSE_SCROLLED_DOWN:
                return "MOUSE_SCROLLED_DOWN";
            case eMouseEventType::MOUSE_SCROLLED_UP:
                return "MOUSE_SCROLLED_UP";
            case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED:
                return "MOUSE_LEFT_BUTTON_CLICKED";
            case eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED:
                return "MOUSE_LEFT_BUTTON_PRESSED";
            case eMouseEventType::MOUSE_NONE:
                return "MOUSE_NONE";
            default:
                assert(false);
                break;
        }
        return "";
    }
};
