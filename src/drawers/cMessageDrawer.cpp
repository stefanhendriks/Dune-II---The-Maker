#include "../include/d2tmh.h"

cMessageDrawer::cMessageDrawer() {
	init();
}

cMessageDrawer::~cMessageDrawer() {
	init();
}

void cMessageDrawer::init() {
	iMessageAlpha = -1;
	memset(cMessage, 0, sizeof(cMessage));
	TIMER_message = 0;
}

void cMessageDrawer::think() {
	int iLimit=250;

	if (game.isState(REGION)) {
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

void cMessageDrawer::setMessage(const char msg[255]) {
	TIMER_message=0;
	memset(cMessage, 0, sizeof(cMessage));
	sprintf(cMessage, "%s", msg);
}

void cMessageDrawer::draw() {
	if (iMessageAlpha > -1) {
		set_trans_blender(0,0,0,iMessageAlpha);
		BITMAP *temp = create_bitmap(480,30);
		clear_bitmap(temp);
		rectfill(temp, 0,0,480,40, makecol(255,0,255));
		draw_sprite(temp, (BITMAP *)gfxinter[BMP_MESSAGEBAR].dat, 0,0);

		// draw message
		alfont_textprintf(temp, game_font, 13,21, makecol(0,0,0), cMessage);

		// draw temp
		draw_trans_sprite(bmp_screen, temp, 1, 42);

		destroy_bitmap(temp);
	}
}
