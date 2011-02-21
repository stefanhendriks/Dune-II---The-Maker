/*
 * cGameControlsContext.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

#define MOUSECELL_MINIMAP -2

cGameControlsContext::cGameControlsContext(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	mouse = cMouse::getInstance();
	mouseCell = -99;
	drawToolTip = false;
}

cGameControlsContext::~cGameControlsContext() {
	player = NULL;
	mouse = NULL;
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

	if (mouse->getX() > (game.getScreenResolution()->getWidth() - 128)) {
		if (mouse->getY() > (game.getScreenResolution()->getHeight() - 128)) {
			mouseCell = MOUSECELL_MINIMAP ; // on minimap
			return;
		}
	}

	if (mouse->getX() > (game.getScreenResolution()->getWidth() - 160)) {
		mouseCell = -3 ; // on sidebar
		return;
	}

	if (map == NULL || mapCamera == NULL) {
		mouseCell = -1;
		return;
	}

	int iMouseX = mouse->getX() / 32;
	int iMouseY = (mouse->getY() - 42) / 32;

	iMouseX += mapCamera->getX();
	iMouseY += mapCamera->getY();

	cCellCalculator * cellCalculator = new cCellCalculator(map);
	mouseCell = cellCalculator->getCell(iMouseX, iMouseY);
	delete cellCalculator;
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
	//TODO:implementation of determineHoveringOverUnitId
	mouseHoveringOverUnitId=-1;
}

cAbstractStructure * cGameControlsContext::getStructurePointerWhereMouseHovers() {
	if (mouseHoveringOverStructureId < 0) {
		return NULL;
	}
	return structure[mouseHoveringOverStructureId];
}

int cGameControlsContext::getMouseCellFromMiniMap() {
	// is not on minimap
	if (getMouseCell() != MOUSECELL_MINIMAP) {
		return -1;
	}

	int drawStartX = gameDrawer->getMiniMapDrawer()->getDrawStartX();
	int drawStartY = gameDrawer->getMiniMapDrawer()->getDrawStartY();

	// the minimap is 128x128 pixels at the bottom right of the screen.
	int mouseMiniMapX = mouse_x - drawStartX;
	int mouseMiniMapY = mouse_y - drawStartY;

	// However, every dot is (due the 64x64 map) 2 pixels wide.
	mouseMiniMapX /= 2;
	mouseMiniMapY /= 2;

	// the mouse is the center of the screen, so substract half of the viewport coordinates
	int newX = mouseMiniMapX;/* - centerOfViewPortWidth;*/
	int newY = mouseMiniMapY;/* - centerOfViewPortHeight;*/

	if (map) {
		cCellCalculator * cellCalculator = new cCellCalculator(map);
		int result = cellCalculator->getCellWithMapBorders(newX, newY);
		delete cellCalculator;
		return result;
	} else {
		return -1;
	}
}
