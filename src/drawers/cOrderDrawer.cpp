#include "cOrderDrawer.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/structures/cOrderProcesser.h"
#include "player/cPlayer.h"
#include "sidebar/cSideBar.h"
#include "utils/Graphics.hpp"

#include <SDL2/SDL.h>

cOrderDrawer::cOrderDrawer(cPlayer *thePlayer) : player(thePlayer)
{
    buttonBitmap = thePlayer->createTextureFromIndexedSurfaceWithPalette(gfxinter->getSurface(BTN_ORDER),232);
    int halfOfButton = buttonBitmap->w / 2;
    int halfOfSidebar = cSideBar::SidebarWidthWithoutCandyBar / 2;
    int halfOfHeightLeftForButton = 50 / 2; // 50 = height of 1 row icons which is removed for Starport
    int halfOfButtonHeight = buttonBitmap->h / 2;
    buttonRect = cRectangle((game.m_screenW - halfOfSidebar) - halfOfButton,
                            (game.m_screenH - halfOfHeightLeftForButton) - halfOfButtonHeight,
                            buttonBitmap->w, buttonBitmap->h);
    _isMouseOverOrderButton = false;

    // greyedButtonBitmap = gfxinter->getSurface(BTN_ORDER);
    /*
    // @Mira fix ORDER button greyed
    auto temp_bitmap = SDL_CreateRGBSurface(0,buttonRect.getWidth(), buttonRect.getHeight(),32,0,0,0,255);
    clear_to_color(temp_bitmap, makecol(255,0,255));
    draw_sprite(temp_bitmap, buttonBitmap, 0, 0);


    greyedButtonBitmap = create_bitmap(buttonRect.getWidth(), buttonRect.getHeight());
    clear_to_color(greyedButtonBitmap, makecol(0, 0, 0));
    set_trans_blender(0, 0, 0, 128);
    draw_trans_sprite(greyedButtonBitmap, temp_bitmap, 0, 0);*/

    // SDL_FreeSurface(temp_bitmap);
}

cOrderDrawer::~cOrderDrawer()
{
    // SDL_FreeSurface(greyedButtonBitmap);
    if (buttonBitmap)
        delete buttonBitmap;
}

void cOrderDrawer::drawOrderButton(cPlayer *thePlayer)
{
    assert(thePlayer);

    cOrderProcesser *orderProcesser = thePlayer->getOrderProcesser();

    assert(orderProcesser);
    if (orderProcesser->isOrderPlaced()) { //grey
        renderDrawer->renderSprite(buttonBitmap, buttonRect.getX(), buttonRect.getY());
        renderDrawer->renderRectColor(buttonRect,0,0,0,128);
    }
    else {
        renderDrawer->renderSprite(buttonBitmap, buttonRect.getX(), buttonRect.getY());
    }

    if (_isMouseOverOrderButton) {
        drawRectangleOrderButton();
    }
}

void cOrderDrawer::drawRectangleOrderButton()
{
    int x = buttonRect.getX();
    int y = buttonRect.getY();
    int width = buttonRect.getWidth();
    int height = buttonRect.getHeight();
    Color color = player->getHouseFadingColor();
    renderDrawer->renderRectColor(x, y, width, height, color);
    renderDrawer->renderRectColor(x+1, y+1, width-2, height-2, color);
}

void cOrderDrawer::onMouseAt(const s_MouseEvent &event)
{
    _isMouseOverOrderButton = buttonRect.isPointWithin(event.coords);
}

void cOrderDrawer::onMouseClickedLeft(const s_MouseEvent &)
{
    // handle "order" button interaction
    if (_isMouseOverOrderButton) {
        cOrderProcesser *orderProcesser = player->getOrderProcesser();
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
    this->player = pPlayer;
}
