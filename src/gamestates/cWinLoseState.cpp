#include "gamestates/cWinLoseState.h"

#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <SDL2/SDL.h>
#include <format>

cWinLoseState::cWinLoseState(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{

}

cWinLoseState::~cWinLoseState()
{

}

void cWinLoseState::thinkFast()
{
}

void cWinLoseState::draw() const
{

}

void cWinLoseState::onNotifyMouseEvent(const s_MouseEvent &event)
{

}

eGameStateType cWinLoseState::getType()
{
    return GAMESTATE_WINLOSE;
}

void cWinLoseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{

}
