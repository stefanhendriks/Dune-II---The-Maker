#pragma once

#include "enums.h"
#include "utils/cPoint.h"

#include <cassert>
#include <string>
#include <format>

// Rename to GUI_EVENT? Might be more appropriate
struct s_MouseEvent {
    eMouseEventType eventType = eMouseEventType::MOUSE_NONE;
    cPoint coords;

    static const std::string toString(const s_MouseEvent &event) {
        std::string msg = std::format("s_MouseEvent [type={}], [x={}], [y={}]",
                                      toStringMouseEventType(event.eventType),
                                      event.coords.x,
                                      event.coords.y);
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
