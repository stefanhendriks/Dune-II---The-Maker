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

    // copy the last bitmap of screen into a separate bitmap which we use for fading out.
    //renderDrawer->drawSprite(bmp_fadeout, bmp_screen, 0, 0);
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, m_screenW, m_screenH, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        //std::cerr << "Erreur lors de la création de la surface: " << SDL_GetError() << std::endl;
        return;
    }
    // Lire les pixels depuis le framebuffer
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, surface->pixels, surface->pitch) != 0) {
        //std::cerr << "Erreur lors de la lecture des pixels: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    screenTexture = SDL_CreateTextureFromSurface(renderer,surface);
    std::string name = fmt::format("screenshot{}.bmp",m_screenshot);
    m_screenshot++;
    SDL_FreeSurface(surface);
}

// this shows the you have lost bmp at screen, after mouse press the mentat debriefing state will begin
void cGame::drawStateLosing()
{
    //renderDrawer->blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, m_screenW, m_screenH);
    //@Mira save copy screen renderDrawer->copyScreen(&bmp_screen);

    // renderDrawer->drawSprite(bmp_screen, gfxdata->getSurface(MOUSE_NORMAL), m_mouse->getX(), m_mouse->getY());
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
    //renderDrawer->blit(bmp_winlose, bmp_screen, 0, 0, 0, 0, m_screenW, m_screenH);
    //@Mira save copy screen renderDrawer->copyScreen(&bmp_screen);

    // renderDrawer->drawSprite(bmp_screen, gfxdata->getSurface(MOUSE_NORMAL), m_mouse->getX(), m_mouse->getY());
    renderDrawer->renderSprite(gfxdata->getTexture(MOUSE_NORMAL), m_mouse->getX(), m_mouse->getY());

    if (m_mouse->isLeftButtonClicked()) {
        // Mentat will be happy, after that enter "Select your next Conquest"
        m_state = GAME_WINBRIEF;

        createAndPrepareMentatForHumanPlayer(!m_skirmish);

        // FADE OUT
        initiateFadingOut();
    }
}

