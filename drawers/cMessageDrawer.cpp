#include "../include/d2tmh.h"

cMessageDrawer::cMessageDrawer() {
    bmpBar = nullptr;
    temp = nullptr;
	init();
}

cMessageDrawer::~cMessageDrawer() {
	init();
}

void cMessageDrawer::destroy() {
    iMessageAlpha = -1;
    memset(cMessage, 0, sizeof(cMessage));
    TIMER_message = 0;

    destroy_bitmap(bmpBar);
    destroy_bitmap(temp);
}

void cMessageDrawer::init() {
    state = COMBAT;
	iMessageAlpha = -1;
	memset(cMessage, 0, sizeof(cMessage));
	TIMER_message = 0;
	initCombatPosition();
}

void cMessageDrawer::createMessageBarBmp(int desiredWidth) {
    if (bmpBar) {
        destroy_bitmap(bmpBar);
    }

    if (temp) {
        destroy_bitmap(temp);
    }

    bmpBar = create_bitmap(desiredWidth, 30);
    clear_to_color(bmpBar, makecol(255, 0, 255));

    allegroDrawer->drawSprite(bmpBar, (BITMAP *)gfxinter[MESSAGE_LEFT].dat, 0, 0);
    for (int drawX = 11; drawX < bmpBar->w; drawX+= 55) {
        allegroDrawer->drawSprite(bmpBar, (BITMAP *)gfxinter[MESSAGE_MIDDLE].dat, drawX, 0);
    }

    allegroDrawer->drawSprite(bmpBar, (BITMAP *)gfxinter[MESSAGE_RIGHT].dat, bmpBar->w - 11, 0);

    // create this one which we use for actual drawing
    temp = create_bitmap(bmpBar->w, bmpBar->h);
}

void cMessageDrawer::think() {
	int iLimit=250;

	if (game.isState(GAME_REGION)) {
		iLimit=600;
	}

	if (cMessage[0] != '\0')
	{
		TIMER_message++;

		// and clear message after shown
		if (TIMER_message > iLimit) {
			memset(cMessage, 0, sizeof(cMessage));
		}

		iMessageAlpha+=3;
		// fade in
		if (iMessageAlpha > 254) {
			iMessageAlpha = 255;
		}
	}
	else
	{
		iMessageAlpha-=6;
		if (iMessageAlpha < 0) {
			iMessageAlpha=-1;
		}

		TIMER_message=0;
	}
}

/**
 * This sets a message on the message bar, which will fade out
 * after a specific amount of time and then get cleared again.
 *
 * @param msg
 */
void cMessageDrawer::setMessage(const char msg[255]) {
	TIMER_message=0;
	memset(cMessage, 0, sizeof(cMessage));
	sprintf(cMessage, "%s", msg);
}

void cMessageDrawer::draw() {
    if (state == COMBAT) {
        draw_sprite(bmp_screen, bmpBar, x, y);
    }

	if (iMessageAlpha > -1) {
		set_trans_blender(0,0,0,iMessageAlpha);

		clear_to_color(temp, makecol(255, 0, 255));

		draw_sprite(temp, bmpBar, 0,0);

		// draw message
		alfont_textprintf(temp, game_font, 13,21, makecol(0,0,0), cMessage);

		// draw temp
		draw_trans_sprite(bmp_screen, temp, x, y);
	}
}

void cMessageDrawer::initRegionPosition(int offsetX, int offsetY) {
    state = NEXT_CONQUEST;

    int desiredWidth = 480;

    createMessageBarBmp(desiredWidth);

    // default positions region mode
    x = offsetX + 73;
    y = offsetY + 358;
}

void cMessageDrawer::initCombatPosition() {
    state = COMBAT;

    int desiredWidth = game.screen_x - cSideBar::SidebarWidth;
    createMessageBarBmp(desiredWidth);

    // default positions in-game (battle mode)
    x = 1;
//    y = 42;
    y = 1;
}