#include "include/cAssert.h"
#include "include/eGameState.h"

#include "utils/Log.h"

const char *stateToString(const int &state)
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
        case GAME_EDITOR:
            return "GAME_EDITOR";
        case GAME_NEW_MAP_EDITOR:
            return "GAME_NEW_MAP_EDITOR";
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
            Logger::fatal( eLogComponent::COMP_GAMESTATE , "operator[]", "State index {} is out of range", state);
            d2tm_assert(false);
            break;
    }
    return "";
}

const char *gameStateToString(eGameState state)
{
    switch (state) {
        case eGameState::INITIALIZE: return "INITIALIZE";
        case eGameState::OVER: return "OVER";
        case eGameState::MENU: return "MENU";
        case eGameState::PLAYING: return "PLAYING";
        case eGameState::BRIEFING: return "BRIEFING";
        case eGameState::EDITOR: return "EDITOR";
        case eGameState::NEW_MAP_EDITOR: return "NEW_MAP_EDITOR";
        case eGameState::OPTIONS: return "OPTIONS";
        case eGameState::REGION: return "REGION";
        case eGameState::SELECT_HOUSE: return "SELECT_HOUSE";
        case eGameState::INTRO: return "INTRO";
        case eGameState::TELLHOUSE: return "TELLHOUSE";
        case eGameState::WINNING: return "WINNING";
        case eGameState::WINBRIEF: return "WINBRIEF";
        case eGameState::LOSEBRIEF: return "LOSEBRIEF";
        case eGameState::LOSING: return "LOSING";
        case eGameState::SKIRMISH: return "SKIRMISH";
        case eGameState::SETUPSKIRMISH: return "SETUPSKIRMISH";
        case eGameState::CREDITS: return "CREDITS";
        case eGameState::MISSIONSELECT: return "MISSIONSELECT";
            // COUNT is not a valid state for the game, but it is for the iteration.
            // We return a special string. Maybe an out of range error is better ?
        case eGameState::COUNT: return "[Error: GameState::COUNT]";
    }
    // Handling unrecognized values ​​(if the enum is cast from an arbitrary character)
    throw std::out_of_range("Unknown GameState value");
}
