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
	mouseCell = -99;
	cellCalculator = new cCellCalculator(&map);
	drawToolTip = false;
}

cGameControlsContext::~cGameControlsContext() {
	player = NULL;
	delete cellCalculator;
}


void cGameControlsContext::updateState() {
    updateMouseCell();
    if (isMouseOnBattleField()) {
        determineToolTip();
        determineHoveringOverStructureId();
        determineHoveringOverUnitId();
    }
}

void cGameControlsContext::updateMouseCell() {
    int screenY = cMouse::getY();
    int screenX = cMouse::getX();

    if (screenY < cSideBar::TopBarHeight) {
		mouseCell = MOUSECELL_TOPBAR; // at the top bar or higher, so no mouse cell id.
		return;
	}

    if (drawManager->getMiniMapDrawer()->isMouseOver()) {
        mouseCell = MOUSECELL_MINIMAP ; // on minimap
        return;
    }

	if (screenX > (game.screen_x - cSideBar::SidebarWidth)) {
		mouseCell = MOUSECELL_SIDEBAR; // on sidebar
		return;
	}

    mouseCell = getMouseCellFromScreen(cSideBar::TopBarHeight, screenY, screenX);
}

int cGameControlsContext::getMouseCellFromScreen(int heightTopBar, int screenY, int screenX) const {
    int iMouseX = mapCamera->divideByZoomLevel(screenX);
    int iMouseY = mapCamera->divideByZoomLevel(screenY - heightTopBar);

    int viewportMouseX = iMouseX + mapCamera->getViewportStartX();
    int viewportMouseY = iMouseY + mapCamera->getViewportStartY();

    int mapX = viewportMouseX / TILESIZE_WIDTH_PIXELS;
    int mapY = viewportMouseY / TILESIZE_HEIGHT_PIXELS;

    return cellCalculator->getCellWithMapBorders(mapX, mapY);
}

void cGameControlsContext::determineToolTip() {
	drawToolTip = false;
	if (key[KEY_T] && isMouseOnBattleField()) {
		drawToolTip = true;
	}
}

void cGameControlsContext::determineHoveringOverStructureId() {
	mouseHoveringOverStructureId = -1;

	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {
			if (structureUtils.isStructureVisibleOnScreen(theStructure)) {
				if (structureUtils.isMouseOverStructure(theStructure, cMouse::getX(), cMouse::getY())) {
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
