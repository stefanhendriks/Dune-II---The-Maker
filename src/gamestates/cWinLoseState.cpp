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
#include <SDL2/SDL.h>
#include <format>
#include <cassert>

cWinLoseState::cWinLoseState(cGame &theGame, GameContext* ctx, Outcome value) : cGameState(theGame, ctx), m_statement(value)
{
    assert(ctx != nullptr);
    if (m_game.getScreenTexture() != nullptr)
        m_backgroundTexture = m_game.getScreenTexture();
    
    if (m_statement == Outcome::Lose) {
        m_tex = m_ctx->getGraphicsContext()->gfxinter->getTexture(BMP_LOSING);
    } else {
        m_tex = m_ctx->getGraphicsContext()->gfxinter->getTexture(BMP_WINNING);
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

    // auto tex = m_ctx->getGraphicsContext()->gfxinter->getTexture(BMP_LOSING);
    int posW = (game.m_gameSettings->getScreenW()-m_tex->w)/2;
    int posH = (game.m_gameSettings->getScreenH()-m_tex->h)/2;
    m_renderDrawer->renderSprite(m_tex,posW, posH);

    // MOUSE
    m_game.getMouse()->draw();
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
        game.goingToWinLoseBrief(GAME_LOSEBRIEF);
    } else {
        game.goingToWinLoseBrief(GAME_WINBRIEF);
    }
    // FADE OUT
    game.initiateFadingOut();
}
