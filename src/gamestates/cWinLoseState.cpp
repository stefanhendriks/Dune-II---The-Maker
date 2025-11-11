#include "gamestates/cWinLoseState.h"

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

cWinLoseState::cWinLoseState(cGame &theGame, GameContext* ctx) : cGameState(theGame, ctx)
{
    if (m_game.getScreenTexture() != nullptr)
        m_backgroundTexture = m_game.getScreenTexture();
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
        renderDrawer->renderSprite(m_backgroundTexture,0,0);

    auto tex = m_ctx->getGraphicsContext()->gfxinter->getTexture(BMP_LOSING);
    int posW = (game.m_screenW-tex->w)/2;
    int posH = (game.m_screenH-tex->h)/2;
    renderDrawer->renderSprite(tex,posW, posH);

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

void cWinLoseState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{

}

void cWinLoseState::onMouseLeftButtonClicked(const s_MouseEvent &event) const
{
    game.goingToLoseBrief();
    // FADE OUT
    game.initiateFadingOut();
}
