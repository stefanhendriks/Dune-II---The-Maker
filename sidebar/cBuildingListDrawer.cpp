/*
 * cBuildingListDrawer.cpp
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cBuildingListDrawer::cBuildingListDrawer() {
}

void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw, int startId) {
	if (listIDToDraw == LIST_CONSTYARD) {
		drawListWithStructures(list, listIDToDraw, startId);
	} else {
		drawListWithUnitsOrAbilities(list, listIDToDraw, startId);
	}
}

void cBuildingListDrawer::drawListWithStructures(cBuildingList *list, int listIDToDraw, int startId) {
	drawList(list, listIDToDraw, startId, true);
}

void cBuildingListDrawer::drawListWithUnitsOrAbilities(cBuildingList *list, int listIDToDraw, int startId) {
	drawList(list, listIDToDraw, startId, false);
}

/**
 * Remember, the buttons are already drawn 'unpressed' with the overall GUI picture (GERALD_XXX , see datafile).
 *
 * So only draw over these buttons the 'not available' or 'pressed' version.
 *
 * @param list
 * @param pressed
 */
void cBuildingListDrawer::drawButton(cBuildingList *list, bool pressed) {
	assert(list != NULL);

	int x = list->getButtonDrawX();
	int y = list->getButtonDrawY();
	int id = list->getButtonIconId();
	bool available = list->isAvailable();

	assert(id > -1);

    // clear
    BITMAP *bmp_trans=create_bitmap(((BITMAP *)gfxinter[BTN_INFANTRY_PRESSED].dat)->w,((BITMAP *)gfxinter[BTN_INFANTRY_PRESSED].dat)->h);
    clear_to_color(bmp_trans, makecol(0,0, 0));

    // set blender
    set_trans_blender(0,0,0,128);
	if (pressed) {
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[id].dat, x, y);		// draw pressed button version (unpressed == default in gui)
	}

    if (!available) {
		fblend_trans(bmp_trans, bmp_screen, x, y, 128);				// make dark
    }

    destroy_bitmap(bmp_trans);
}


/**
 * Draw the list, start from startId, until the max icons in the list to be drawn.
 *
 * @param startId
 */
void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw, int startId, bool shouldDrawStructureSize) {
	// starting draw coordinates
	int iDrawX=572;
	int iDrawY=46;

	int end = startId + 5; // max 5 icons are showed at once

	// is building an item in the list?
	bool isBuildingItemInList = list->isBuildingItem();

	// draw the icons
	for (int i = startId; i < end; i++) {
		cBuildingListItem * item = list->getItem(i);

		if (item == NULL) {
			break; // stop. List became empty.
		}

		if (shouldDrawStructureSize) {
			drawStructureSize(item->getBuildId(), iDrawX, iDrawY);
		}

		int iDrawXEnd = iDrawX + 63;
		int iDrawYEnd = iDrawY + 47;

		// icon id must be set , assert it.
		assert(item->getIconId() > -1);

		draw_sprite(bmp_screen, (BITMAP *)gfxinter[item->getIconId()].dat, iDrawX, iDrawY);

		bool cannotPayIt = item->getBuildCost() > player[HUMAN].credits;

		// when this item is being built.
		if (item->isBuilding()) {
			int iTotalBuildPoints = 0;

			// get the total build time
			if (listIDToDraw == LIST_CONSTYARD) {
				iTotalBuildPoints = structures[item->getBuildId()].build_time;
			}
			else if (listIDToDraw != LIST_STARPORT) {
				iTotalBuildPoints = units[item->getBuildId()].build_time;
			}

			// Now calculate the right frame.
			float iPiece = iTotalBuildPoints / 31; // = 17 - 1 (of above)

			if (iPiece < 0.1) {
				iPiece = 0.1;
			}

			int iFrame = health_bar(31, item->getProgress(), iTotalBuildPoints);

			if (iFrame > 31) {
				iFrame = 31;
			}


			if (item->getProgress() < iTotalBuildPoints) {
				// draw the other progress stuff
				 fblend_trans((BITMAP *)gfxinter[PROGRESSFIX].dat, bmp_screen, iDrawX+2, iDrawY+2, 128);
				 fblend_trans((BITMAP *)gfxinter[PROGRESS001+iFrame].dat, bmp_screen, iDrawX+2, iDrawY+2, 128);

			} else {
				// draw 'ready' text when done building.
				if (listIDToDraw == LIST_CONSTYARD) {
					draw_sprite(bmp_screen, (BITMAP *)gfxinter[READY01].dat, iDrawX+3, iDrawY+16);
				}
			}
		} else {

			// this item is not being built. So we do not draw a progress indicator.
			// however, it could be that an other item is being built.

			// draw the item 'unavailable' when:
			// - is not available (doh)
			// - we cant pay it
			// - some other item is being built
			if (!item->isAvailable() || cannotPayIt || isBuildingItemInList) {
				set_trans_blender(0,0,0,128);
				fblend_trans((BITMAP *)gfxinter[PROGRESSNA].dat, bmp_screen, iDrawX, iDrawY, 64);
			}

			// draw cross when not able to pay
			if (cannotPayIt) {
				rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, makecol(game.fade_select, 0, 0));
				line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, makecol(game.fade_select, 0, 0));
				line(bmp_screen, iDrawX, iDrawY+47, iDrawX+63, iDrawY, makecol(game.fade_select, 0, 0));
				set_trans_blender(0,0,0,128);
			}

			// last built id
			if (list->getLastClickedId() == i) {
				rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), makecol(game.fade_select, game.fade_select, game.fade_select));
				rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, makecol(game.fade_select, game.fade_select, game.fade_select));
			}
		}

		if (item->getTimesToBuild() > 0) {
			// draw number of times to build this thing (queueing)
			int textX = iDrawX + 41;
			int textY = iDrawY + 16;

			if (item->getTimesToBuild() < 10) {
				textX += 10;
			}

			// draw
			alfont_textprintf(bmp_screen, game_font, textX + 1,textY + 1, makecol(0,0,0), "%d", item->getTimesToBuild());
			alfont_textprintf(bmp_screen, game_font, textX,textY, makecol(255,255,255), "%d", item->getTimesToBuild());
		}

		// draw rectangle when mouse hovers over icon
		if (isOverItemCoordinates_Boolean(mouse_x, mouse_y, iDrawX, iDrawY)) {
			int iColor=makecol(game.fade_select, game.fade_select, game.fade_select);

			if (player[0].getHouse() == ATREIDES) {
				iColor = makecol(0, 0, game.fade_select);
			}

			if (player[0].getHouse() == HARKONNEN) {
				iColor = makecol(game.fade_select, 0, 0);
			}

			if (player[0].getHouse() == ORDOS) {
				iColor = makecol(0, game.fade_select, 0);
			}

			rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), iColor);
			rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, iColor);
		}

		iDrawY+=48;
	}



}

/**
 * Draw the amount of squares a structure would take. The X and Y coordinates are the top
 * left coordinates of an icon.
 *
 * @param structureId
 * @param x
 * @param y
 */
void cBuildingListDrawer::drawStructureSize(int structureId, int x, int y) {
	// figure out size
	int iW=structures[structureId].bmp_width/32;
	int iH=structures[structureId].bmp_height/32;
	int iTile = GRID_1X1;

	if (iW == 2 && iH == 2) {
		iTile = GRID_2X2;
	}

	if (iW == 3 && iH == 2) {
		iTile = GRID_3X2;
	}

	if (iW == 3 && iH == 3) {
		iTile = GRID_3X3;
	}

	BITMAP *temp=create_bitmap(19,19);
	clear_bitmap(temp);

	draw_sprite(temp, (BITMAP *)gfxinter[GRID_0X0].dat, 0, 0);

	fblend_trans(temp, bmp_screen, x+43, y+20, 192);

	set_trans_blender(0, 0, 0, 128);

	draw_sprite(bmp_screen, (BITMAP *)gfxinter[iTile].dat, x+43, y+20);

	destroy_bitmap(temp);

}

bool cBuildingListDrawer::isOverItemCoordinates_Boolean(int x, int y, int drawX, int drawY) {
	if (x >= drawX && x <= (drawX + 64) && y >= drawY && y < (drawY + 48)) {
		return true;
	}
	return false;
}

cBuildingListItem * cBuildingListDrawer::isOverItemCoordinates(cBuildingList *list, int x, int y) {
	assert(list != NULL);
	// starting draw coordinates
	int iDrawX=572;
	int iDrawY=46;

	int startId = list->getScrollingOffset();
	int end = startId + 5; // 5 icons in the list
	// draw the icons
	for (int i = startId; i < end; i++) {
		cBuildingListItem * item = list->getItem(i);
		if (item == NULL) break;

		if (isOverItemCoordinates_Boolean(x, y, iDrawX, iDrawY)) {
			return item;
		}

		iDrawY+=48;
	}

	return NULL;
}

bool cBuildingListDrawer::isOverItem(cBuildingList *list, int x, int y) {
	return isOverItemCoordinates(list, x, y) != NULL;
}
