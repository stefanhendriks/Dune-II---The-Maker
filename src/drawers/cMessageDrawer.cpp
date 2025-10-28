#include "cMessageDrawer.h"
#include "data/gfxinter.h"
#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"

#include <SDL2/SDL.h>

// Game Playing State and SelectYourNextConquestState have their own version
// this class has to be abstracted in such a way so it is used in both those states, yet without the wonky init
// functions

cMessageDrawer::cMessageDrawer(GameContext* ctx) :
    m_TIMER_message(0),
    m_alpha(0),
    m_state(messages::eMessageDrawerState::COMBAT),
    m_fadeState(messages::eMessageDrawerFadingState::FADE_IN),
    m_textDrawer(game_font),
    m_ctx(ctx),
    m_gfxinter(ctx->getGraphicsContext()->gfxinter.get())
{
    m_textDrawer.setApplyShadow(false);
    m_bmpBar = nullptr;
    m_keepMessage = false;
    m_timeMessageIsVisible = 10;
    init();
}

cMessageDrawer::~cMessageDrawer()
{
    init();
}

void cMessageDrawer::draw()
{
    if (m_state == messages::eMessageDrawerState::COMBAT) {
        renderDrawer->renderFromSurface(m_bmpBar, m_position.x, m_position.y);
    }

    if (m_alpha > -1) {
        renderDrawer->renderFromSurface(m_bmpBar, m_position.x, m_position.y, m_alpha);
        // draw message
        m_textDrawer.drawText(m_position.x+13, m_position.y+6, Color{0, 0, 0,Uint8(m_alpha)}, m_message);
    }
}

void cMessageDrawer::thinkFast()
{
    if (m_fadeState == messages::eMessageDrawerFadingState::FADE_IN) {
        m_TIMER_message++;

        if (m_TIMER_message > m_timeMessageIsVisible) {
            if (!m_keepMessage) {
                m_fadeState = messages::eMessageDrawerFadingState::FADE_OUT;
            }
        }

        m_alpha += 3;
        // fade in, with a max...
        if (m_alpha > 254) {
            m_alpha = 255;
        }
        return;
    }

    // fade out
    m_alpha -= 6;
    if (m_alpha < 0) {
        m_alpha = -1;
    }

    // clear message
    if (m_alpha < 1) {
        m_message.clear();
    }
    m_TIMER_message = 0;
}

void cMessageDrawer::init()
{
    m_state = messages::eMessageDrawerState::COMBAT;
    m_fadeState = messages::eMessageDrawerFadingState::FADE_IN;
    m_keepMessage = false;
    m_alpha = -1;
    m_message.clear();
    m_TIMER_message = 0;
    initCombatPosition();
}

void cMessageDrawer::destroy()
{
    m_alpha = -1;
    m_TIMER_message = 0;
    if (m_bmpBar != nullptr) {
        SDL_FreeSurface(m_bmpBar);
    }
}

void cMessageDrawer::setKeepMessage(bool value)
{
    m_keepMessage = value;
}

void cMessageDrawer::initRegionPosition(int offsetX, int offsetY)
{
    m_state = messages::eMessageDrawerState::NEXT_CONQUEST;
    m_keepMessage = false;
    m_timeMessageIsVisible = 650; // sensible default

    int desiredWidth = 480;

    createMessageBarBmp(desiredWidth);

    // default positions region mode
    m_position.x = offsetX + 73;
    m_position.y = offsetY + 358;
}

void cMessageDrawer::initCombatPosition()
{
    m_state = messages::eMessageDrawerState::COMBAT;
    m_keepMessage = false;
    m_timeMessageIsVisible = 450; // sensible default

    int margin = 4;
    int widthOfOptionsButton = 160 + margin;
    int desiredWidth = game.m_screenW - (cSideBar::SidebarWidth + widthOfOptionsButton);
    createMessageBarBmp(desiredWidth);
    // default positions in-game (battle mode)
    m_position.x = widthOfOptionsButton;
    //    y = 42;
    m_position.y = 1;
}

/**
 * This sets a message on the message bar, which will fade out
 * after a specific amount of time and then get cleared again.
 *
 * @param msg
 */
void cMessageDrawer::setMessage(const std::string &msg, bool keepMessage)
{
    m_TIMER_message=0;
    m_fadeState = messages::eMessageDrawerFadingState::FADE_IN;
    m_message = msg;
    setKeepMessage(keepMessage);
}

void cMessageDrawer::createMessageBarBmp(int desiredWidth)
{
    if (m_bmpBar) {
        SDL_FreeSurface(m_bmpBar);
    }

    m_bmpBar = SDL_CreateRGBSurface(0,desiredWidth, 30,32,0,0,0,255);
    auto color = SDL_MapRGBA(m_bmpBar->format,255,0,255,255);
    SDL_FillRect(m_bmpBar, nullptr, color);
    SDL_BlitSurface(m_gfxinter->getSurface(MESSAGE_LEFT), nullptr, m_bmpBar, nullptr);

    for (int drawX = 11; drawX < m_bmpBar->w; drawX+= 55) {
        auto src = m_gfxinter->getSurface(MESSAGE_MIDDLE);
        auto tmp = cRectangle{drawX, 0,src->w,src->h};
        SDL_Rect destRect = tmp.toSDL();
        SDL_BlitSurface(src, nullptr, m_bmpBar, &destRect);
    }
    auto src = m_gfxinter->getSurface(MESSAGE_RIGHT);
    auto tmp = cRectangle{m_bmpBar->w - 11, 0,src->w,src->h};
    SDL_Rect destRect = tmp.toSDL();
    SDL_BlitSurface(src, nullptr, m_bmpBar, &destRect);
}
