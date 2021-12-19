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
    mouseHoveringOverUnitId = -1;
    mouseHoveringOverStructureId = -1;
}

cGameControlsContext::~cGameControlsContext() {
	player = NULL;
}


void cGameControlsContext::updateMouseCell(const cPoint &coords) {
    if (coords.y < cSideBar::TopBarHeight) {
		mouseCell = MOUSECELL_TOPBAR; // at the top bar or higher, so no mouse cell id.
		return;
	}

    if (drawManager->getMiniMapDrawer()->isMouseOver()) {
        mouseCell = MOUSECELL_MINIMAP ; // on minimap
        return;
    }

	if (coords.x > (game.screen_x - cSideBar::SidebarWidth)) {
		mouseCell = MOUSECELL_SIDEBAR; // on sidebar
		return;
	}

    mouseCell = getMouseCellFromScreen(coords.x, coords.y);
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
    if (mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = false;
        }
    }
    mouseHoveringOverUnitId=-1;
    int mc = getMouseCell();
    tCell *cellOfMouse = map.getCell(mc);
    if (cellOfMouse == nullptr) return; // mouse is not on battlefield

    if (cellOfMouse->id[MAPID_UNITS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_UNITS];

        if (unit[iUnitId].iTempHitPoints < 0) {
            mouseHoveringOverUnitId = iUnitId;
        }

    } else if (cellOfMouse->id[MAPID_WORMS] > -1) {
        int iUnitId = cellOfMouse->id[MAPID_WORMS];
        mouseHoveringOverUnitId = iUnitId;
    }

    if (mouseHoveringOverUnitId > -1) {
        cUnit &aUnit = unit[mouseHoveringOverUnitId];
        if (aUnit.isValid()) {
            aUnit.bHovered = true;
        }
    }
}

cAbstractStructure * cGameControlsContext::getStructurePointerWhereMouseHovers() {
	if (mouseHoveringOverStructureId < 0) {
		return nullptr;
	}
	return structure[mouseHoveringOverStructureId];
}

void cGameControlsContext::onMouseAt(const s_MouseEvent &event) {
    updateMouseCell(event.coords);
    if (isMouseOnBattleField()) {
        determineToolTip();
        determineHoveringOverStructureId(event.coords.x, event.coords.y);
        determineHoveringOverUnitId();
    } else {
        mouseHoveringOverStructureId = -1;
        mouseHoveringOverUnitId = -1;
    }
}

void cGameControlsContext::onNotify(const s_MouseEvent &event) {
  if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
      onMouseAt(event);
  }
}
