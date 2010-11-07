/*
 * cGameControlsContext.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

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
	determineToolTip();
	determineHoveringOverStructureId();
	determineHoveringOverUnitId();
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

void cGameControlsContext::determineToolTip() {
	drawToolTip = false;
	if (key[KEY_T] && isMouseOnBattleField()) {
		drawToolTip = true;
	}
}

void cGameControlsContext::determineHoveringOverStructureId() {
	cStructureUtils structureUtils;

	mouseHoveringOverStructureId = -1;

	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {
			if (structureUtils.isStructureOnScreen(theStructure)) {
				if (structureUtils.isMouseOverStructure(mouse, theStructure)) {
					mouseHoveringOverStructureId = i;
				}
			}
		}
	}
}

void cGameControlsContext::determineHoveringOverUnitId() {
	mouseHoveringOverUnitId=-1;
}

cAbstractStructure * cGameControlsContext::getStructurePointerWhereMouseHovers() {
	if (mouseHoveringOverStructureId < 0) {
		return NULL;
	}
	return structure[mouseHoveringOverStructureId];
}
