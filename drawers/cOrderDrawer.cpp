#include "../include/d2tmh.h"


cOrderDrawer::cOrderDrawer(cPlayer *thePlayer) : player(thePlayer) {
    buttonBitmap = (BITMAP *)gfxinter[BTN_ORDER].dat;
    int halfOfButton = buttonBitmap->w / 2;
    int halfOfSidebar = cSideBar::SidebarWidthWithoutCandyBar / 2;
    int halfOfHeightLeftForButton = 50 / 2; // 50 = height of 1 row icons which is removed for Starport
    int halfOfButtonHeight = buttonBitmap->h / 2;
    buttonRect = cRectangle((game.screen_x - halfOfSidebar) - halfOfButton,
                                (game.screen_y - halfOfHeightLeftForButton) - halfOfButtonHeight,
                                buttonBitmap->w, buttonBitmap->h);
    _isMouseOverOrderButton = false;
}

cOrderDrawer::~cOrderDrawer() {
}

void cOrderDrawer::drawOrderPlaced() {
	BITMAP *bmp_trans=create_bitmap(buttonBitmap->w, buttonBitmap->h);
	clear_to_color(bmp_trans, makecol(255,0,255));

	// copy
	draw_sprite(bmp_trans, (BITMAP *)gfxinter[BTN_ORDER].dat, 0, 0);

	// make black
	allegroDrawer->drawRectangleFilled(bmp_screen, buttonRect, makecol(0,0,0));

	// trans
	fblend_trans(bmp_trans, bmp_screen, buttonRect.getX(), buttonRect.getY(), 128);

	// destroy - phew
	destroy_bitmap(bmp_trans);
}

void cOrderDrawer::drawOrderButton(cPlayer * thePlayer) {
	assert(thePlayer);

	cOrderProcesser * orderProcesser = thePlayer->getOrderProcesser();

	assert(orderProcesser);
	if (orderProcesser->isOrderPlaced()) {
		drawOrderPlaced();
	} else {
		draw_sprite(bmp_screen, buttonBitmap, buttonRect.getX(), buttonRect.getY());
	}

    if (_isMouseOverOrderButton) {
        drawRectangleOrderButton();
    }
}

void cOrderDrawer::drawRectangleOrderButton() {
    int x = buttonRect.getX();
    int y = buttonRect.getY();
    int width = buttonRect.getWidth();
    int height = buttonRect.getHeight();
    int color = player->getHouseFadingColor();
    allegroDrawer->drawRectangle(bmp_screen, x, y, width, height, color);
    allegroDrawer->drawRectangle(bmp_screen, x+1, y+1, width-2, height-2, color);
}

void cOrderDrawer::onMouseAt(const s_MouseEvent &event) {
    _isMouseOverOrderButton = buttonRect.isPointWithin(event.coords);
}

void cOrderDrawer::onMouseClickedLeft(const s_MouseEvent &) {
    cOrderProcesser * orderProcesser = player->getOrderProcesser();

    // handle "order" button interaction
    if (_isMouseOverOrderButton) {
        if (orderProcesser->canPlaceOrder()) {
            orderProcesser->placeOrder();
        }
    }

}

void cOrderDrawer::onNotify(const s_MouseEvent &event) {
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

void cOrderDrawer::setPlayer(cPlayer *pPlayer) {
    this->player = pPlayer;
}
