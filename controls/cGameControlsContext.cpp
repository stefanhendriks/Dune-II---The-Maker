/*
 * cGameControlsContext.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cGameControlsContext::cGameControlsContext(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	mouse = cMouse::getInstance();
	mouseCell = -99;
	cellCalculator = new cCellCalculator(&map);
	drawToolTip = false;
}

cGameControlsContext::~cGameControlsContext() {
	player = NULL;
	mouse = NULL;
	delete cellCalculator;
}


void cGameControlsContext::updateState() {
	determineMouseCell();
	drawToolTip = false;
	if (key[KEY_T] && isMouseOnBattleField()) {
		drawToolTip = true;
	}
}

void cGameControlsContext::determineMouseCell() {
	if (mouse->getY() < 42) {
		mouseCell = -1; // at the top bar or higher, so no mouse cell id.
		return;
	}

	if (mouse->getX() > (game.screen_x - 128)) {
		if (mouse->getY() > (game.screen_y - 128)) {
			mouseCell = -2 ; // on minimap
			return;
		}
	}

	if (mouse->getX() > (game.screen_x - 160)) {
		mouseCell = -3 ; // on sidebar
		return;
	}

	int iMouseX = mouse->getX() / 32;
	int iMouseY = (mouse->getY() - 42) / 32;

	iMouseX += mapCamera->getX();
	iMouseY += mapCamera->getY();

	mouseCell = cellCalculator->getCell(iMouseX, iMouseY);
}
