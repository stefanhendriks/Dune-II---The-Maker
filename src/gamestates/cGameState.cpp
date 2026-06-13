#include "cGameState.h"
#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"

#include "include/cAssert.h"

cGameState::cGameState(sGameServices* services) :
    m_ctx(services->ctx),
    m_renderDrawer(m_ctx->getSDLDrawer())
{
    d2tm_assert(m_ctx != nullptr);
}

const char *gameStateToString(eGameStateType state)
{
    switch (state) {
        case GAMESTATE_CHOOSE_HOUSE: return "GAMESTATE_CHOOSE_HOUSE";
        case GAMESTATE_CREDITS: return "GAMESTATE_CREDITS";
        case GAMESTATE_EDITOR: return "GAMESTATE_EDITOR";
        case GAMESTATE_NEW_MAP_EDITOR: return "GAMESTATE_NEW_MAP_EDITOR";
        case GAMESTATE_MAIN_MENU: return "GAMESTATE_MAIN_MENU";
        case GAMESTATE_MENTAT: return "GAMESTATE_MENTAT";
        case GAMESTATE_OPTIONS: return "GAMESTATE_OPTIONS";
        case GAMESTATE_PLAYING: return "GAMESTATE_PLAYING";
        case GAMESTATE_SELECT_MISSION: return "GAMESTATE_SELECT_MISSION";
        case GAMESTATE_SELECT_YOUR_NEXT_CONQUEST: return "GAMESTATE_SELECT_YOUR_NEXT_CONQUEST";
        case GAMESTATE_SETUP_SKIRMISH: return "GAMESTATE_SETUP_SKIRMISH";
        case GAMESTATE_TELLHOUSE: return "GAMESTATE_TELLHOUSE";
        case GAMESTATE_WINLOSE: return "GAMESTATE_WINLOSE";
    }
    // Handling unrecognized values ​​(if the enum is cast from an arbitrary character)
    return "Unknown GameState value";
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

void cGameState::thinkNormal()
{}

void cGameState::thinkSlow()
{}

void cGameState::update()
{}

