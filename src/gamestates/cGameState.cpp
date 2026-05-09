#include "cGameState.h"
#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"

#include <cassert>

cGameState::cGameState(sGameServices* services) :
    m_ctx(services->ctx),
    m_renderDrawer(m_ctx->getSDLDrawer()),
    m_log(services->m_log)
{
    assert(m_ctx != nullptr);
    assert(m_log != nullptr);
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

