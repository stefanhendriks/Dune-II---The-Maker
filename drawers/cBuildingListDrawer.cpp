#include "../include/d2tmh.h"


cBuildingListDrawer::cBuildingListDrawer(cPlayer *thePlayer) : player(thePlayer) {
    textDrawer = new cTextDrawer(game_font);
}

cBuildingListDrawer::~cBuildingListDrawer() {
    delete textDrawer;
}

void cBuildingListDrawer::drawList(cBuildingList *list, int listIDToDraw) {
	if (listIDToDraw == eListTypeAsInt(eListType::LIST_CONSTYARD)) {
        drawListWithStructures(list);
	} else {
        drawListWithUnitsOrAbilities(list);
	}
}

void cBuildingListDrawer::drawListWithStructures(cBuildingList *list) {
    drawList(list, true);
}

void cBuildingListDrawer::drawListWithUnitsOrAbilities(cBuildingList *list) {
    drawList(list, false);
}

void cBuildingListDrawer::drawButtonHoverRectangle(cBuildingList *list) {
    assert(list != NULL);

    int x = list->getButtonDrawX();
    int y = list->getButtonDrawY();
    int id = list->getButtonIconIdUnpressed();

    int width = 33;
    int height = ((BITMAP *)gfxinter[id].dat)->h;

    int color = player->getSelectFadingColor();

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

//	int width = ((BITMAP *)gfxinter[id].dat)->w;
	int width = 33;
	int height = ((BITMAP *)gfxinter[id].dat)->h;
//	rectfill(bmp_screen, x, y, x+width, y+height, makecol(255, list->getType() * (255/LIST_MAX), list->getType() * (255/LIST_MAX)));

    // clear
	draw_sprite(bmp_screen, (BITMAP *)gfxinter[list->getButtonIconIdUnpressed()].dat, x, y);		// draw pressed button version (unpressed == default in gui)

    // set blender
    set_trans_blender(0,0,0,128);
	draw_sprite(bmp_screen, (BITMAP *)gfxinter[id].dat, x, y);

    if (!list->isAvailable()) {
    	fblend_rect_trans(bmp_screen, x, y, width, height, makecol(0,0,0), 96);
    }

    if (pressed) {
        list->stopFlashing();

        int color = player->getHouseFadingColor();

        allegroDrawer->drawRectangle(bmp_screen, x, y, width, height, color);
        allegroDrawer->drawRectangle(bmp_screen, x + 1, y + 1, width - 2, height - 2, color);
    } else {
        if (list->isFlashing()) {
            int color = list->getFlashingColor();

            allegroDrawer->drawRectangle(bmp_screen, x, y, width, height, color);
            allegroDrawer->drawRectangle(bmp_screen, x + 1, y + 1, width - 2, height - 2, color);
            allegroDrawer->drawRectangle(bmp_screen, x + 2, y + 2, width - 3, height - 3, color);
        }
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
void cBuildingListDrawer::drawList(cBuildingList *list, bool shouldDrawStructureSize) {
	// starting draw coordinates
	int iDrawX=getDrawX();
	int iDrawY=getDrawY();

    int selectFadingColor = player->getSelectFadingColor();

    int end = MAX_ITEMS;

	// is building an item in the list?
	std::array<int, 5> isBuildingItemInList = list->isBuildingItem();

	// Start drawing the 'icons grid'
    int withOfIcon = 63;
    int heightOfIcon = 47;

    // draw the icons, in rows of 3
	int rowNr = 0;
	for (int i = 0; i < end; i++) {
		cBuildingListItem * item = list->getItem(i);

		if (item == nullptr) {
			continue; // allow gaps in the list data structure (just not with rendering)
		}

        int iDrawXEnd = iDrawX + withOfIcon;
        int iDrawYEnd = iDrawY + heightOfIcon;

        // asumes drawing for human player
        bool cannotPayIt = !player->hasEnoughCreditsFor(item->getBuildCost());

        // icon id must be set , assert it.
		assert(item->getIconId() > -1);

		draw_sprite(bmp_screen, (BITMAP *)gfxinter[item->getIconId()].dat, iDrawX, iDrawY);

		if (shouldDrawStructureSize) {
			drawStructureSize(item->getBuildId(), iDrawX, iDrawY);
		}

		// when this item is being built.
		if (item->isBuilding()) {
            int iFrame = item->getBuildProgressFrame();

			if (!item->isDoneBuilding() || iFrame < 31) {
				// draw the other progress stuff
				set_trans_blender(0, 0, 0, 128);
				draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[PROGRESSFIX].dat, iDrawX+2, iDrawY+2);
				draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[PROGRESS001+iFrame].dat, iDrawX+2, iDrawY+2);
			} else {
			    if (item->shouldPlaceIt()) {
                    // TODO: draw white/red (flicker)
                    int icon = READY01;
                    if (player->isContextMouseState(eMouseState::MOUSESTATE_PLACE)) {
                        icon = READY02;
                    }
                    draw_sprite(bmp_screen, (BITMAP *) gfxinter[icon].dat, iDrawX + 3, iDrawY + 16);
				} else if (item->shouldDeployIt()) {
                    // TODO: draw white/red (flicker)
                    // TODO: draw DEPLOY
                    int icon = READY01;
                    if (player->bDeployIt) {
                        icon = READY02;
                    }
                    draw_sprite(bmp_screen, (BITMAP *) gfxinter[icon].dat, iDrawX + 3, iDrawY + 16);
                }
			}
		} else { // not building
			// this item is not being built. So we do not draw a progress indicator.
			// however, it could be that an other item is being built.

			// draw the item 'unavailable' when:
			// - is not available (doh)
			// - we cant pay it
			// - some other item is being built
			// - list is being upgraded, so you cannot build items
            bool isBuildingSameSubListItem = false;
            for (int i2 = 0; i2 < 5; i2++) {
                if (isBuildingItemInList[i2] < 0) continue;
                if (i2 == item->getSubList()) {
                    isBuildingSameSubListItem = true;
                    break;
                }
            }

			if (!item->isAvailable() || isBuildingSameSubListItem) {
				set_trans_blender(0,0,0,128);
				fblend_trans((BITMAP *)gfxinter[PROGRESSNA].dat, bmp_screen, iDrawX, iDrawY, 64);

				// Pending upgrading (ie: an upgrade is progressing, blocking the construction of these items)
				if (item->isPendingUpgrading()) {
                    int errorFadingColor = player->getErrorFadingColor();
                    rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    line(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);

                    int red = makecol(255, 0, 0);
				    textDrawer->setFont(small_font);
				    textDrawer->drawTextCenteredInBox("Upgrading", iDrawX, iDrawY, withOfIcon, heightOfIcon, red);
				    textDrawer->setFont(game_font);
				}

				// Pending building (ie: a build is progressing, blocking the upgrade)
				if (item->isPendingBuilding()) {
                    int errorFadingColor = player->getErrorFadingColor();
                    rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
                    line(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);

                    int red = makecol(255, 0, 0);
				    textDrawer->setFont(small_font);
                    int height = heightOfIcon / 3;
                    textDrawer->drawTextCenteredInBox("Building", iDrawX, iDrawY, withOfIcon, height, red);
                    textDrawer->drawTextCenteredInBox("in", iDrawX, iDrawY+height, withOfIcon, height, red);
                    textDrawer->drawTextCenteredInBox("progress", iDrawX, iDrawY+(height*2), withOfIcon, height, red);
				    textDrawer->setFont(game_font);
				}
			}

			if (list->getType() == eListType::LIST_STARPORT) {
			    // only for starport show you can't pay it, as we allow building units when you cannot pay it (ie partial
			    // payment/progress)
				if (cannotPayIt) {
					set_trans_blender(0,0,0,128);
					fblend_trans((BITMAP *)gfxinter[PROGRESSNA].dat, bmp_screen, iDrawX, iDrawY, 64);
					int errorFadingColor = player->getErrorFadingColor();
					rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
					line(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, errorFadingColor);
					line(bmp_screen, iDrawX, iDrawY + heightOfIcon, iDrawX + withOfIcon, iDrawY, errorFadingColor);
					set_trans_blender(0,0,0,128);
				}
			}

			// last built id
			if (list->getLastClickedId() == i) {
				rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
				rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, selectFadingColor);
			}
		}

		// if item is queuable, then it will show the amount of
		// items being built. Meaning, with 1 item to build, it will show a 1.
        int amountToShow = item->getTimesToBuild();
		bool showAmount = item->isQueuable() && amountToShow > 0;

        bool listIsStarport = item->getList()->getType() == eListType::LIST_STARPORT;
        if (listIsStarport) {
            amountToShow = item->getTimesOrdered();
            showAmount = true; // starport shows always all amounts
		}

		if (showAmount) {
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

        if (DEBUGGING) {
            if (key[KEY_TAB] && key[KEY_D]) {
                int textX = iDrawX + 41;
                int textY = iDrawY + 40;

                textDrawer->drawTextWithOneInteger(textX, textY, "%d", item->getSubList());
            }
        }

		// draw rectangle when mouse hovers over icon
		if (isOverItemCoordinates_Boolean(mouse_x, mouse_y, iDrawX, iDrawY)) {
			rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), selectFadingColor);
			rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, selectFadingColor);
		} else {
            int color = list->getFlashingColor();
            if (item->isFlashing()) {
                rect(bmp_screen, (iDrawX + 1), (iDrawY + 1), (iDrawXEnd - 1), (iDrawYEnd - 1), color);
                rect(bmp_screen, iDrawX, iDrawY, iDrawXEnd, iDrawYEnd, color);
            }
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
	int iW= sStructureInfo[structureId].bmp_width / 32;
	int iH= sStructureInfo[structureId].bmp_height / 32;
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

void cBuildingListDrawer::setPlayer(cPlayer *thePlayer) {
    this->player = thePlayer;
}
