#include "gamestates/cWinLoseState.h"

#include "game/cGame.h"
#include "include/d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "game/cGameInterface.h"
#include <SDL2/SDL.h>
#include <format>
#include <cassert>

cWinLoseState::cWinLoseState(cGame &theGame, sGameServices* services, Outcome value) : 
    cGameState(theGame, services),
    m_settings(services->settings),
    m_statement(value)
{
    assert(services != nullptr);
    assert(m_settings != nullptr);

    auto ctx = services->ctx;
    m_interface = ctx->getGameInterface();
    assert(m_interface != nullptr);
    auto gfxinter = ctx->getGraphicsContext()->gfxinter;
    assert(gfxinter != nullptr);

    if (m_interface->getScreenTexture() != nullptr)
        m_backgroundTexture = m_interface->getScreenTexture();
    
    if (m_statement == Outcome::Lose) {
        m_tex = gfxinter->getTexture(BMP_LOSING);
    } else {
        m_tex = gfxinter->getTexture(BMP_WINNING);
    }
}

cWinLoseState::~cWinLoseState()
{
}

void cWinLoseState::thinkFast()
{
}

void cWinLoseState::draw() const
{
    if (m_backgroundTexture)
        m_renderDrawer->renderSprite(m_backgroundTexture,0,0);

    int posW = (m_settings->getScreenW()-m_tex->w)/2;
    int posH = (m_settings->getScreenH()-m_tex->h)/2;
    m_renderDrawer->renderSprite(m_tex,posW, posH);

    m_interface->drawCursor();
}

void cWinLoseState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
            break;
        default:
            break;
    }
}

eGameStateType cWinLoseState::getType()
{
    return GAMESTATE_WINLOSE;
}

void cWinLoseState::onNotifyKeyboardEvent(const cKeyboardEvent &)
{
}

void cWinLoseState::onMouseLeftButtonClicked(const s_MouseEvent &) const
{
    if (m_statement == Outcome::Lose) {
        m_interface->goingToWinLoseBrief(GAME_LOSEBRIEF);
    } else {
        m_interface->goingToWinLoseBrief(GAME_WINBRIEF);
    }
    // FADE OUT
    m_interface->initiateFadingOut();
}
