#include "gamestates/cGamePlaying.h"

#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <SDL2/SDL.h>
#include <format>

cGamePlaying::cGamePlaying(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{
}

cGamePlaying::~cGamePlaying()
{

}

void cGamePlaying::thinkFast()
{
}

void thinkNormal()
{
}

void cGamePlaying::thinkSlow()
{
}


void cGamePlaying::draw() const
{
}

void cGamePlaying::onNotifyMouseEvent(const s_MouseEvent &event)
{
}

eGameStateType cGamePlaying::getType()
{
    return GAMESTATE_PLAYING;
}

void cGamePlaying::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{

}
