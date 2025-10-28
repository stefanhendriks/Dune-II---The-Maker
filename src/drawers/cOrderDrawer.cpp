#include "cOrderDrawer.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/structures/cOrderProcesser.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include <SDL2/SDL.h>

cOrderDrawer::cOrderDrawer(GameContext *ctx, cPlayer *player) :
    m_ctx(ctx),
    m_player(player)
{
    auto *gfxinter = m_ctx->getGraphicsContext()->gfxinter.get();
    m_buttonBitmap = player->createTextureFromIndexedSurfaceWithPalette(gfxinter->getSurface(BTN_ORDER), TransparentColorIndex);
    int halfOfButton = m_buttonBitmap->w / 2;
    int halfOfSidebar = cSideBar::SidebarWidthWithoutCandyBar / 2;
    int halfOfHeightLeftForButton = 50 / 2; // 50 = height of 1 row icons which is removed for Starport
    int halfOfButtonHeight = m_buttonBitmap->h / 2;
    m_buttonRect = cRectangle((game.m_screenW - halfOfSidebar) - halfOfButton,
                            (game.m_screenH - halfOfHeightLeftForButton) - halfOfButtonHeight,
                            m_buttonBitmap->w, m_buttonBitmap->h);
    m_isMouseOverOrderButton = false;
}

cOrderDrawer::~cOrderDrawer()
{
    if (m_buttonBitmap)
        delete m_buttonBitmap;
}

void cOrderDrawer::drawOrderButton(cPlayer *thePlayer)
{
    assert(thePlayer);

    cOrderProcesser *orderProcesser = thePlayer->getOrderProcesser();

    assert(orderProcesser);
    if (orderProcesser->isOrderPlaced()) { //grey
        renderDrawer->renderSprite(m_buttonBitmap, m_buttonRect.getX(), m_buttonRect.getY());
        renderDrawer->renderRectColor(m_buttonRect,0,0,0,128);
    }
    else {
        renderDrawer->renderSprite(m_buttonBitmap, m_buttonRect.getX(), m_buttonRect.getY());
    }

    if (m_isMouseOverOrderButton) {
        drawRectangleOrderButton();
    }
}

void cOrderDrawer::drawRectangleOrderButton()
{
    int x = m_buttonRect.getX();
    int y = m_buttonRect.getY();
    int width = m_buttonRect.getWidth();
    int height = m_buttonRect.getHeight();
    Color color = m_player->getHouseFadingColor();
    renderDrawer->renderRectColor(x, y, width, height, color);
    renderDrawer->renderRectColor(x+1, y+1, width-2, height-2, color);
}

void cOrderDrawer::onMouseAt(const s_MouseEvent &event)
{
    m_isMouseOverOrderButton = m_buttonRect.isPointWithin(event.coords);
}

void cOrderDrawer::onMouseClickedLeft(const s_MouseEvent &)
{
    // handle "order" button interaction
    if (m_isMouseOverOrderButton) {
        cOrderProcesser *orderProcesser = m_player->getOrderProcesser();
        if (orderProcesser->canPlaceOrder()) {
            orderProcesser->placeOrder();
        }
    }

}

void cOrderDrawer::onNotify(const s_MouseEvent &event)
{
    switch (event.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseAt(event);
            return;
        case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED:
            onMouseClickedLeft(event);
            return;
        default:
            return;
    }
}

void cOrderDrawer::setPlayer(cPlayer *pPlayer)
{
    this->m_player = pPlayer;
}
