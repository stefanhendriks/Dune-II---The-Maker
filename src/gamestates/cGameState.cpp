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

const char *gameStateTypeToString(eGameStateType state)
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
        case GAMESTATE_VIDEO: return "GAMESTATE_VIDEO";
    }
    // Handling unrecognized values ​​(if the enum is cast from an arbitrary character)
    return "Unknown GameState value";
}

void cGameState::thinkNormal()
{}

void cGameState::thinkSlow()
{}

void cGameState::update()
{}

