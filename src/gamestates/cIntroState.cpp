#include "gamestates/cIntroState.h"
#include "game/cGame.h"
#include "utils/ini.h"
#include "include/d2tmc.h"
#include "player/cPlayer.h"
#include "data/gfxdata.h"
#include "context/GameContext.hpp"
#include "utils/Graphics.hpp"
#include "drawers/SDLDrawer.hpp"

#include <iostream>

cIntroState::cIntroState(cGame &game, GameContext* ctx)
    : cGameState(game, ctx)
{
    gfxmovie = std::make_shared<Graphics>(renderDrawer->getRenderer(), "data/scenes/sdl_intro4.dat");
    TIMER_movie = 0;
    iMovieFrame = 0;
    // offsetX = 0 for screen resolution 640x480, ie, meaning > 640 we take the difference / 2
    offsetX = (game.m_screenW - 640) / 2;
    offsetY = (game.m_screenH - 480) / 2; // same goes for offsetY (but then for 480 height).
    movieTopleftX = offsetX + 256;
    movieTopleftY = offsetY + 120;
}

cIntroState::~cIntroState()
{
    
}

eGameStateType cIntroState::getType()
{
    return eGameStateType::GAMESTATE_INTRO;
}


void cIntroState::thinkFast()
{
    if (gfxmovie != nullptr) {
        TIMER_movie++;

        if (TIMER_movie > 20) {
            iMovieFrame++;

            if (iMovieFrame == gfxmovie->getNumberOfFiles()) {
                iMovieFrame = 0;
            }
            TIMER_movie = 0;
        }
    }
}

void cIntroState::draw() const
{
    Texture *tmp = gfxmovie->getTexture(iMovieFrame);
    cRectangle dest = {movieTopleftX, movieTopleftY,tmp->w, tmp->h};
    renderDrawer->renderStrechFullSprite(tmp, dest);
}

void cIntroState::onNotifyMouseEvent(const s_MouseEvent &event)
{

}

void cIntroState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{

}
