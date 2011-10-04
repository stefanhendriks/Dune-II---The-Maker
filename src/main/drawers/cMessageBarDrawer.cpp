#include "../include/d2tmh.h"

cMessageBarDrawer::cMessageBarDrawer(MessageBar * theMessageBar) {
	assert(theMessageBar);
	messageBar = theMessageBar;
}

cMessageBarDrawer::cMessageBarDrawer() {
	messageBar = new MessageBar(0, 0, 0);
}

cMessageBarDrawer::~cMessageBarDrawer() {
	delete messageBar;
	messageBar = NULL;
}

void cMessageBarDrawer::setMessage(const char msg[255]) {
	messageBar->setMessage(msg);
}

void cMessageBarDrawer::think() {
	int iLimit = 250;

	if (game.isState(NEXTCONQUEST)) {
		iLimit = 600;
	}

	if (messageBar->isMessageSet()) {
		messageBar->incrementTimerMessage();

		if (messageBar->getTimerMessage() > iLimit) {
			messageBar->clearMessage();
		}

		messageBar->incrementAlpha();
	} else {
		messageBar->decreaseAlpha();
		messageBar->resetTimerMessage();
	}
}


void cMessageBarDrawer::drawMessageBar() {
	int x = messageBar->getX();
	int y = messageBar->getY();
	int alpha = messageBar->getAlpha();

	if (alpha > -1) {
		int totalWidth = 11 + messageBar->getWidth() + 11;

		set_trans_blender(0, 0, 0, alpha);
		BITMAP *temp = create_bitmap(totalWidth, 30);
		clear_bitmap(temp);

		rectfill(temp, 0, 0, totalWidth, 40, makecol(255, 0, 255));

		// first draw the left part (11 pixels wide)
		draw_sprite(temp, (BITMAP *) gfxinter[MESSAGE_LEFT].dat, 0, 0);

		// draw now parts of the bars to the right
		for (int i = 11; i < totalWidth; i += 55) {
			draw_sprite(temp, (BITMAP *) gfxinter[MESSAGE_MIDDLE].dat, i, 0);
		}

		// draw rectangle for the last part
		int drawXOfRightPart = (totalWidth - 11);
		rectfill(temp, drawXOfRightPart, 0, totalWidth, 40, makecol(255, 0, 255));
		draw_sprite(temp, (BITMAP *) gfxinter[MESSAGE_RIGHT].dat, drawXOfRightPart, 0);

		// draw message
		alfont_textprintf(temp, game_font, 13,21, makecol(0,0,0), messageBar->getMessage());

		// draw temp
		draw_trans_sprite(bmp_screen, temp, x, y);

		destroy_bitmap(temp);
	}
}
