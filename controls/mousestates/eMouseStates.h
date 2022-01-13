#pragma once

#include <string>
#include <assert.h>

// states, apply to battlefield only
enum eMouseState {
    MOUSESTATE_SELECT, // mouse is in 'normal' mode, it is able to select units
    MOUSESTATE_UNITS_SELECTED, // mouse has selected units
    MOUSESTATE_REPAIR, // mouse is in repair state
    MOUSESTATE_PLACE, // mouse is going to place a structure
    MOUSESTATE_DEPLOY, // mouse is going to deploy something
};

inline std::string mouseStateString(eMouseState state) {
    switch (state) {
        case MOUSESTATE_SELECT: return "MOUSESTATE_SELECT";
        case MOUSESTATE_UNITS_SELECTED: return "MOUSESTATE_UNITS_SELECTED";
        case MOUSESTATE_REPAIR: return "MOUSESTATE_REPAIR";
        case MOUSESTATE_PLACE: return "MOUSESTATE_PLACE";
        case MOUSESTATE_DEPLOY: return "MOUSESTATE_DEPLOY";
        default:
            assert(false);
            break;
    }
    return "";
}