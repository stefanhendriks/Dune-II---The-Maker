#include "../include/d2tmh.h"

cBuildingListDrawer::cBuildingListDrawer() {
    textDrawer = new cTextDrawer(game_font);
}

cBuildingListDrawer::~cBuildingListDrawer() {
    delete textDrawer;
}

void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw) {
	if (listIDToDraw == LIST_CONSTYARD) {
        drawListWithStructures(list, listIDToDraw);
	} else {
        drawListWithUnitsOrAbilities(list, listIDToDraw);
	}
}

void cBuildingListDrawer::drawListWithStructures(cBuildingList *list, int listIDToDraw) {
    drawList(list, listIDToDraw, true);
}

void cBuildingListDrawer::drawListWithUnitsOrAbilities(cBuildingList *list, int listIDToDraw) {
    drawList(list, listIDToDraw, false);
}

void cBuildingListDrawer::drawButtonHoverRectangle(cBuildingList *list) {
    assert(list != NULL);

    int x = list->getButtonDrawX();
    int y = list->getButtonDrawY();
    int id = list->getButtonIconIdUnpressed();

    int width = ((BITMAP *)gfxinter[id].dat)->w;
    int height = ((BITMAP *)gfxinter[id].dat)->h;

    int color = player[HUMAN].getSelectFadingColor();

    allegroDrawer->drawRectangle(bmp_screen, x, y, width, height, color);
    allegroDrawer->drawRectangle(bmp_screen, x + 1, y + 1, width-2, height-2, color);

}

void cBuildingListDrawer::drawButton(cBuildingList *list, bool pressed) {
	assert(list != NULL);

	int x = list->getButtonDrawX();
	int y = list->getButtonDrawY();
	int id = list->getButtonIconIdUnpressed();

	if (pressed) {
		id = list->getButtonIconIdPressed();
	}

	if (id < 0) {
	    // this is not good
	    logbook("ERROR UNABLE TO DRAW LIST BECAUSE ID IS NOT VALID!?");
	    return;
	}

	int width = ((BITMAP *)gfxinter[id].dat)->w;
	int height = ((BITMAP *)gfxinter[id].dat)->h;

    // clear
	draw_sprite(bmp_screen, (BITMAP *)gfxinter[list->getButtonIconIdUnpressed()].dat, x, y);		// draw pressed button version (unpressed == default in gui)

    // set blender
    set_trans_blender(0,0,0,128);
	draw_sprite(bmp_screen, (BITMAP *)gfxinter[id].dat, x, y);

    if (!list->isAvailable()) {
    	fblend_rect_trans(bmp_screen, x, y, width, height, makecol(0,0,0), 96);
    }

    if (pressed) {
        int color = player[HUMAN].getHouseFadingColor();

        allegroDrawer->drawRectangle(bmp_screen, x, y, width, height, color);
        allegroDrawer->drawRectangle(bmp_screen, x + 1, y + 1, width-2, height-2, color);
    }
}


int cBuildingListDrawer::getDrawX() {
	return (game.screen_x - cSideBar::SidebarWidthWithoutCandyBar) + 2;
}

int cBuildingListDrawer::getDrawY() {
    return cSideBar::TopBarHeight + 230 + 30;
}

/**
 * Draw the list, start from startId, until the max icons in the list to be drawn.
 *
 * @param startId
 */
void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw, bool shouldDrawStructureSize) {
	// starting draw coordinates
	int iDrawX=getDrawX();
	int iDrawY=getDrawY();

    int selectFadingColor = player[HUMAN].getSelectFadingColor();

    int end = MAX_ITEMS;

	// is building an item in the list?
	std::array<int, 5> isBuildingItemInList = list->isBuildingItem();

	// draw the icons, in rows of 3
	int rowNr = 0;
	for (int i = 0; i < end; i++) {
		cBuildingListItem * item = list->getItem(i);

		if (item == NULL) {
			continue; // allow gaps in the list data structure (just not with rendering)
		}

        int iDrawXEnd = iDrawX + 63;
		int iDrawYEnd = iDrawY + 47;

		// icon id must be set , assert it.
		assert(item->getIconId() > -1);

		rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, makecol(255, 255, 255));
		line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, makecol(255, 255, 255));

		draw_sprite(bmp_screen, (BITMAP *)gfxinter[item->getIconId()].dat, iDrawX, iDrawY);

		if (shouldDrawStructureSize) {
			drawStructureSize(item->getBuildId(), iDrawX, iDrawY);
		}

		// asumes drawing for human player
		bool cannotPayIt = item->getBuildCost() > player[HUMAN].credits;

		// when this item is being built.
		if (item->isBuilding()) {
			int iTotalBuildPoints = 0;

            // get the total build time
            if (listIDToDraw != LIST_STARPORT) {
                iTotalBuildPoints = item->getBuildTime();
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
				set_trans_blender(0, 0, 0, 128);
				draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[PROGRESSFIX].dat, iDrawX+2, iDrawY+2);
				draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[PROGRESS001+iFrame].dat, iDrawX+2, iDrawY+2);

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
			// - list is being upgraded, so you cannot build items
			 /*|| cannotPayIt*/
            bool isBuildingSameSubListItem = false;
            for (int i = 0; i < 5; i++) {
                if (isBuildingItemInList[i] < 0) continue;
                if (i == item->getSubList()) {
                    isBuildingSameSubListItem = true;
                    break;
                }
            }

			if (!item->isAvailable() || isBuildingSameSubListItem || !list->isAcceptsOrders()) {
				set_trans_blender(0,0,0,128);
				fblend_trans((BITMAP *)gfxinter[PROGRESSNA].dat, bmp_screen, iDrawX, iDrawY, 64);
			}

			if (list->getType() == LIST_STARPORT) {
				if (cannotPayIt) {
					set_trans_blender(0,0,0,128);
					fblend_trans((BITMAP *)gfxinter[PROGRESSNA].dat, bmp_screen, iDrawX, iDrawY, 64);
					int errorFadingColor = player[HUMAN].getErrorFadingColor();
					rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
					line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
					line(bmp_screen, iDrawX, iDrawY+47, iDrawX+63, iDrawY, errorFadingColor);
					set_trans_blender(0,0,0,128);
				}
			}

			// last built id
			if (list->getLastClickedId() == i) {
				rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
				rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, selectFadingColor);
			}
		}

		int amountToShow = item->getTimesToBuild();
		if (amountToShow <= 0) {
			amountToShow = item->getTimesOrdered();
		}

		if (amountToShow > 0) {
			// draw number of times to build this thing (queueing)
			int textX = iDrawX + 41;
			int textY = iDrawY + 16;

			if (amountToShow < 10) {
				textX += 10;
			}

			// draw
			char msg[10];
			sprintf(msg, "%d", amountToShow);
            textDrawer->drawText(textX, textY, msg);
		}

		// draw rectangle when mouse hovers over icon
		if (isOverItemCoordinates_Boolean(mouse_x, mouse_y, iDrawX, iDrawY)) {
			rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
			rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, selectFadingColor);
		}

		// update coordinates, 3 icons in a row
		if (rowNr < 2) {
            iDrawX+=66;
            rowNr++;
		} else {
            rowNr = 0;
            iDrawX=getDrawX();
            iDrawY+=50;
        }
	}

	set_clip_rect(bmp_screen, 0, 0, game.screen_x, game.screen_y);
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
	set_trans_blender(0, 0, 0, 192);

	draw_trans_sprite(temp, bmp_screen, x + 43, y + 20);

	draw_sprite(temp, (BITMAP *)gfxinter[GRID_0X0].dat, 0, 0);

	draw_trans_sprite(bmp_screen, temp, x + 43, y + 20);

	draw_sprite(bmp_screen, (BITMAP *)gfxinter[iTile].dat, x + 43, y + 20);

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

//	int iDrawX=drawManager->getSidebarDrawer()->getBuildingListDrawer()->getDrawX();
//	int iDrawY=drawManager->getSidebarDrawer()->getBuildingListDrawer()->getDrawY();
    int iDrawX=getDrawX();
    int iDrawY=getDrawY();

	int end = MAX_ITEMS;

    int rowNr = 0;
	for (int i = 0; i < end; i++) {
		cBuildingListItem * item = list->getItem(i);
		if (item == NULL) break;

		if (isOverItemCoordinates_Boolean(x, y, iDrawX, iDrawY)) {
			return item;
		}

		if (rowNr < 2) {
            rowNr++;
            iDrawX+=66;
		} else {
            iDrawY += 50;
            iDrawX=getDrawX();
            rowNr = 0;
        }
	}

	return NULL;
}

bool cBuildingListDrawer::isOverItem(cBuildingList *list, int x, int y) {
	return isOverItemCoordinates(list, x, y) != NULL;
}
