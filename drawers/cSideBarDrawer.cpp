/*
 * cSideBarDrawer.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cSideBarDrawer::cSideBarDrawer() {
	buildingListDrawer = new cBuildingListDrawer();
}

cSideBarDrawer::~cSideBarDrawer() {
	delete buildingListDrawer;
}

void cSideBarDrawer::drawHouseGui(cPlayer * thePlayer) {
	assert(thePlayer);
	set_palette(thePlayer->pal);

	// black out
	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));

    // upper bar
    rectfill(bmp_screen, 0, 0, game.screen_x, 42, makecol(0,0,0));

    int iHouse = thePlayer->getHouse();

    draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_800X600].dat, 0, 0);

//    if (iHouse == ATREIDES)
//        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_ATR].dat, 0, 0);
//    else if (iHouse == HARKONNEN)
//        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_HAR].dat, 0, 0);
//    else if (iHouse == ORDOS)
//        draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_ORD].dat, 0, 0);
//    else {
//    	assert(false);
//    }
//	else
  //      draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD].dat, 0, 0); // draw interface skeleton (THIS IS BAD IF THIS IS SHOWN)

    // TODO: support fremen, mercenary and sardaukar
}

void cSideBarDrawer::drawBuildingLists() {
	// draw the sidebar itself (the backgrounds, borders, etc)
	cSideBar *sidebar = player->getSideBar();

	// draw the buildlist icons
	int selectedListId = sidebar->getSelectedListID();

	for (int listId = LIST_CONSTYARD; listId < LIST_MAX; listId++) {
		cBuildingList *list = sidebar->getList(listId);
		bool isListIdSelectedList = (selectedListId == listId);
		buildingListDrawer->drawButton(list, isListIdSelectedList);
	}

	// draw the buildlist itself (take scrolling into account)
	cBuildingList *selectedList = NULL;

	if (selectedListId > -1) {
		selectedList = sidebar->getList(selectedListId);
		buildingListDrawer->drawList(selectedList, selectedListId, selectedList->getScrollingOffset());
	}
}

// draws the sidebar on screen
void cSideBarDrawer::drawSideBar(cPlayer * player) {

	drawHouseGui(player);
	drawBuildingLists();
//	drawCapacities();
	drawScrollButtons();
}

void cSideBarDrawer::drawCapacities() {
	// Draw the bars
	int iHeight = 79 - health_bar(79, player[0].use_power,player[0].has_power);

	int step = (255/79);
	int r = 255-(iHeight*step);
	int g = iHeight*step;

	if (g > 255) {
		g = 255;
	}

	if (r < 0) {
		r = 0;
	}

	// power use bar
	rectfill(bmp_screen, 488,442, 490, 442-iHeight, makecol(r,g,0));

	// spice/credits bar
	iHeight=health_bar(79, player[0].credits, player[0].max_credits);
	rectfill(bmp_screen, 497,442, 499, 442-iHeight, makecol(0,0,255));
}

void cSideBarDrawer::drawScrollButtons() {
	int buttonUpX = game.screen_x - 68;
	int buttonUpY = game.screen_y - 310;

	cMouse * mouse = cMouse::getInstance();

	bool leftScrollButtonPressed=false;
	bool rightScrollButtonPressed=false;

	if (mouse->isMouseScrolledUp()) {
		leftScrollButtonPressed = true;
	}
	if (mouse->isMouseScrolledDown()) {
		rightScrollButtonPressed = true;
	}

	cSideBar *sidebar = player->getSideBar();

	// when mouse pressed, a list is selected, and that list is still available
	int selectedListID = sidebar->getSelectedListID();
	if (selectedListID > -1 && sidebar->getList(selectedListID)->isAvailable()) {
		cBuildingList *list = sidebar->getList(selectedListID);

		bool mouseOverUp = isMouseOverScrollUp();
		bool mouseOverDown = isMouseOverScrollDown();
		assert(!(mouseOverUp == true && mouseOverDown == true));// can never be both.

		cMouse * mouse = cMouse::getInstance();
		if (mouseOverUp) {
			leftScrollButtonPressed = mouse->isLeftButtonPressed();
		} else if (mouseOverDown) {
			rightScrollButtonPressed = mouse->isLeftButtonPressed();
		}
	}

	if (leftScrollButtonPressed) {
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_SCROLL_UP_PRESSED].dat, buttonUpX, buttonUpY);
	} else {
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_SCROLL_UP_UNPRESSED].dat, buttonUpX, buttonUpY);
	}

	buttonUpX = game.screen_x - 34;

	if (rightScrollButtonPressed) {
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_SCROLL_DOWN_PRESSED].dat, buttonUpX, buttonUpY);
	} else {
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_SCROLL_DOWN_UNPRESSED].dat, buttonUpX, buttonUpY);
	}


}


int cSideBarDrawer::getScrollButtonUpX() {
	return game.screen_x - 68;
}

int cSideBarDrawer::getScrollButtonDownX() {
	return game.screen_x - 34;
}

int cSideBarDrawer::getScrollButtonY() {
	return game.screen_y - 310;
}

bool cSideBarDrawer::isOverScrollButton(int buttonX, int buttonY) {
	if ((mouse_x >= buttonX && mouse_y >= buttonY) && (mouse_x < (buttonX + 30) && mouse_y < (buttonY + 30))) {
			return true;
	}
	return false;
}

bool cSideBarDrawer::isMouseOverScrollUp() {
	return isOverScrollButton(getScrollButtonUpX(), getScrollButtonY());
}

bool cSideBarDrawer::isMouseOverScrollDown() {
	return isOverScrollButton(getScrollButtonDownX(), getScrollButtonY());
}
