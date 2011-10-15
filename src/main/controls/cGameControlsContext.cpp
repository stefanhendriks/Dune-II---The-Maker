#include "../include/d2tmh.h"

#include "../utils/CellCalculator.h"

#define MOUSECELL_MINIMAP -2

cGameControlsContext::cGameControlsContext(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	mouse = Mouse::getInstance();
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

Rectangle * cGameControlsContext::getMouseLastDraggedSelectionRectangle() {
	return mouse->getLastCreatedRectangle();
}

/**
 * Note this function is also taking into account that the mouse is on the battle field!
 */
bool cGameControlsContext::isMouseDraggingSelectionRectangle() {
	return mouse->isMouseDraggingRectangle() && isMouseOnBattleField();
}

void cGameControlsContext::determineMouseCell() {
	if (mouse->getY() < 42) {
		mouseCell = -1; // at the top bar or higher, so no mouse cell id.
		return;
	}

	if (mouse->getX() > (game.getScreenResolution()->getWidth() - 128)) {
		if (mouse->getY() > (game.getScreenResolution()->getHeight() - 128)) {
			mouseCell = MOUSECELL_MINIMAP; // on minimap
			return;
		}
	}

	if (mouse->getX() > (game.getScreenResolution()->getWidth() - 160)) {
		mouseCell = -3; // on sidebar
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

	CellCalculator * cellCalculator = new CellCalculator(map);
	mouseCell = cellCalculator->getCell(iMouseX, iMouseY);
	delete cellCalculator;
	cellCalculator = NULL;
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

	for (int i = 0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];

		if (theStructure) {
			if (structureUtils.isStructureOnScreen(theStructure)) {
				Rectangle * rectangle = theStructure->getRectangle();
				if (mouse->isOverRectangle(rectangle)) {
					mouseHoveringOverStructureId = i;
					break;
				}
			}
		}
	}
}

void cGameControlsContext::determineHoveringOverUnitId() {
	mouseHoveringOverUnitId = -1;
	if (isMouseOnBattleField()) {
		cMapCell cell = map->cell[mouseCell];
		if (cell.gameObjectId[MAPID_UNITS] > -1) {
			int iUnitId = cell.gameObjectId[MAPID_UNITS];

			// TODO: this really makes no sense, but it works now.
			if (unit[iUnitId].iTempHitPoints < 0) { // is not being repaired or in structure
				mouseHoveringOverUnitId = iUnitId;
			}
		} else if (cell.gameObjectId[MAPID_WORMS] > -1) {
			int iUnitId = cell.gameObjectId[MAPID_WORMS];
			mouseHoveringOverUnitId = iUnitId;
		}
	}
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
		CellCalculator * cellCalculator = new CellCalculator(map);
		int result = cellCalculator->getCellTakingMapBordersIntoAccount(newX, newY);
		delete cellCalculator;
		return result;
	} else {
		return -1;
	}
}
