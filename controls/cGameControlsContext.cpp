/*
 * cGameControlsContext.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"
#include "cGameControlsContext.h"


cGameControlsContext::cGameControlsContext(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	mouseCell = -99;
	drawToolTip = false;
}

cGameControlsContext::~cGameControlsContext() {
	player = NULL;
}


void cGameControlsContext::updateMouseCell(int mouseX, int mouseY) {
    if (mouseY < cSideBar::TopBarHeight) {
		mouseCell = MOUSECELL_TOPBAR; // at the top bar or higher, so no mouse cell id.
		return;
	}

    if (drawManager->getMiniMapDrawer()->isMouseOver()) {
        mouseCell = MOUSECELL_MINIMAP ; // on minimap
        return;
    }

	if (mouseX > (game.screen_x - cSideBar::SidebarWidth)) {
		mouseCell = MOUSECELL_SIDEBAR; // on sidebar
		return;
	}

    mouseCell = getMouseCellFromScreen(mouseX, mouseY);
}

int cGameControlsContext::getMouseCellFromScreen(int mouseX, int mouseY) const {
    int absMapX = mapCamera->getAbsMapMouseX(mouseX);
    int absMapY = mapCamera->getAbsMapMouseY(mouseY);
//    int iMouseX = mapCamera->divideByZoomLevel(mouseX);
//    int iMouseY = mapCamera->divideByZoomLevel(mouseY - heightTopBar);
//
//    // absolute coordinates
//    int viewportMouseX = iMouseX + mapCamera->getViewportStartX();
//    int viewportMouseY = iMouseY + mapCamera->getViewportStartY();
//
//    // map coordinates
//    int mapX = viewportMouseX / TILESIZE_WIDTH_PIXELS;
//    int mapY = viewportMouseY / TILESIZE_HEIGHT_PIXELS;

    return mapCamera->getCellFromAbsolutePosition(absMapX, absMapY);
//    return map.getCellWithMapBorders(mapX, mapY);
}

void cGameControlsContext::determineToolTip() {
	drawToolTip = false;
	if (key[KEY_T] && isMouseOnBattleField()) {
		drawToolTip = true;
	}
}

void cGameControlsContext::determineHoveringOverStructureId(int mouseX, int mouseY) {
	mouseHoveringOverStructureId = -1;

	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {
			if (structureUtils.isStructureVisibleOnScreen(theStructure)) {
				if (structureUtils.isMouseOverStructure(theStructure, mouseX, mouseY)) {
					mouseHoveringOverStructureId = i;
					break;
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

void cGameControlsContext::onMouseAt(int x, int y) {
    updateMouseCell(x, y);
    if (isMouseOnBattleField()) {
        determineToolTip();
        determineHoveringOverStructureId(x, y);
        determineHoveringOverUnitId();
    } else {
        mouseHoveringOverStructureId = -1;
        mouseHoveringOverUnitId = -1;
    }
}
