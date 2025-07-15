/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks
*/

#include "cGame.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include <string>
#include <SDL2/SDL.h>

// Fading between menu items
void cGame::initiateFadingOut()
{
    // set state to fade out
    m_fadeAction = eFadeAction::FADE_OUT; // fade out
    m_fadeAlpha = 250;

    renderDrawer->beginDrawingToTexture(screenTexture);
    SDL_RenderCopy(renderer, actualRenderer->tex,nullptr, nullptr);
    renderDrawer->endDrawingToTexture();
}

// this shows the you have lost bmp at screen, after mouse press the mentat debriefing state will begin
void cGame::drawStateLosing()
{
    if (screenTexture)
        renderDrawer->renderSprite(screenTexture,0,0);

    auto tex = gfxinter->getTexture(BMP_LOSING);
    int posW = (m_screenW-tex->w)/2;
    int posH = (m_screenH-tex->h)/2;
    renderDrawer->renderSprite(tex,posW, posH);
    renderDrawer->renderSprite(gfxdata->getTexture(MOUSE_NORMAL), m_mouse->getX(), m_mouse->getY());

    if (m_mouse->isLeftButtonClicked()) {
        m_state = GAME_LOSEBRIEF;

        createAndPrepareMentatForHumanPlayer(!m_skirmish);

        // FADE OUT
        initiateFadingOut();
    }
}

// this shows the you have won bmp at screen, after mouse press the mentat debriefing state will begin
void cGame::drawStateWinning()
{
    if (screenTexture)
        renderDrawer->renderSprite(screenTexture,0,0);
        
    auto tex = gfxinter->getTexture(BMP_WINNING);
    int posW = (m_screenW-tex->w)/2;
    int posH = (m_screenH-tex->h)/2;
    renderDrawer->renderSprite(tex,posW, posH);
    renderDrawer->renderSprite(gfxdata->getTexture(MOUSE_NORMAL), m_mouse->getX(), m_mouse->getY());

    if (m_mouse->isLeftButtonClicked()) {
        // Mentat will be happy, after that enter "Select your next Conquest"
        m_state = GAME_WINBRIEF;

        createAndPrepareMentatForHumanPlayer(!m_skirmish);

        // FADE OUT
        initiateFadingOut();
    }
}

