#include <cassert>
#include "include/enums GameState.h"


const char *stateString(const int &state)
{
    switch (state) {
        case GAME_INITIALIZE:
            return "GAME_INITIALIZE";
        case GAME_OVER:
            return "GAME_OVER";
        case GAME_MENU:
            return "GAME_MENU";
        case GAME_PLAYING:
            return "GAME_PLAYING";
        case GAME_BRIEFING:
            return "GAME_BRIEFING";
        case GAME_EDITING:
            return "GAME_EDITING";
        case GAME_OPTIONS:
            return "GAME_OPTIONS";
        case GAME_REGION:
            return "GAME_REGION";
        case GAME_SELECT_HOUSE:
            return "GAME_SELECT_HOUSE";
        case GAME_TELLHOUSE:
            return "GAME_TELLHOUSE";
        case GAME_WINNING:
            return "GAME_WINNING";
        case GAME_WINBRIEF:
            return "GAME_WINBRIEF";
        case GAME_LOSEBRIEF:
            return "GAME_LOSEBRIEF";
        case GAME_LOSING:
            return "GAME_LOSING";
        case GAME_SETUPSKIRMISH:
            return "GAME_SETUPSKIRMISH";
        case GAME_CREDITS:
            return "GAME_CREDITS";
        case GAME_MISSIONSELECT:
            return "GAME_MISSIONSELECT";
        default:
            assert(false);
            break;
    }
    return "";
}