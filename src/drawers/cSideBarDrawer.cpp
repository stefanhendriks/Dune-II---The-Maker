/*
 * cSideBarDrawer.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cSideBarDrawer::cSideBarDrawer() {
	buildingListDrawer = new cBuildingListDrawer();
	candybar = NULL;
	optionsBar = NULL;
	sidebarColor = makecol(214, 149, 20);
}

cSideBarDrawer::~cSideBarDrawer() {
	delete buildingListDrawer;
	if (candybar) {
		destroy_bitmap(candybar);
	}
	if (optionsBar) {
		destroy_bitmap(optionsBar);
	}
}

void cSideBarDrawer::drawCandybar() {
	if (candybar == NULL) {
		// startpos = 40
		// end pos = height - 156
		// height = start pos - end pos
		int heightInPixels = (game.screen_y - 156) - 40;
		candybar = create_bitmap_ex(8, 24, heightInPixels);
		clear_to_color(candybar, makecol(0, 0, 0));

		// ball first
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BALL].dat, 0, 0); // height of ball = 25
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_TOP].dat, 0, 26); // height of top = 10
	    // now draw pieces untill the end (height of piece is 23 pixels)
	    for (int y = 36; y < (heightInPixels + 23); y += 24) {
		    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_PIECE].dat, 0, y);
	    }
	    // draw bottom
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BOTTOM].dat, 0, heightInPixels-10); // height of top = 10
	}

	int drawX = game.screen_x - 158;
	int drawY = 40;
	draw_sprite(bmp_screen, candybar, drawX, drawY);
}

void cSideBarDrawer::drawHouseGui(cPlayer * thePlayer) {
	assert(thePlayer);
	set_palette(thePlayer->pal);

	// black out
	rectfill(bmp_screen, (game.screen_x-160), 0, game.screen_x, game.screen_y, makecol(0,0,0));

    // upper bar
    rectfill(bmp_screen, 0, 0, game.screen_x, 42, makecol(0,0,0));

    //draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_800X600].dat, 0, 0);
    drawCandybar();

    // draw the list background of the icons
    drawIconsListBackground();

    // minimap at bottom right
    drawMinimap();

    // draw options bar (todo: move to own options drawer and delegate in gameDrawer)
    drawOptionsBar();

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

void cSideBarDrawer::drawMinimap() {
	BITMAP * sprite = (BITMAP *)gfxinter[BMP_GERALD_MINIMAP_BOTTOMRIGHT].dat;
	int drawX = game.screen_x - sprite->w;
	int drawY = game.screen_y - sprite->h;
	draw_sprite(bmp_screen, sprite, drawX, drawY);
}

void cSideBarDrawer::drawOptionsBar() {
	if (optionsBar == NULL) {
		optionsBar = create_bitmap(game.screen_x, 40);
		clear_to_color(optionsBar, sidebarColor);

		// credits
		draw_sprite(optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOPBAR_CREDITS].dat, (game.screen_x - 240), 0);

		for (int w = 0; w < (game.screen_x + 800); w += 789) {
			draw_sprite(optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOP_BAR].dat, w, 31);
		}

	}
	draw_sprite(bmp_screen, optionsBar, 0, 0);
}

void cSideBarDrawer::drawIconsListBackground() {
	// fill
	rectfill(bmp_screen, (game.screen_x-130), 40, game.screen_x, (game.screen_y - 160), sidebarColor);

	int heightInPixels = (game.screen_y - 315) - 45;
	BITMAP * backgroundList = create_bitmap_ex(8, 66, heightInPixels);

	// left 'lines'
	vline(bmp_screen, game.screen_x-132, 40, game.screen_y - 276, makecol(255, 198, 93));
	vline(bmp_screen, game.screen_x-131, 40, game.screen_y - 276, makecol(60, 36, 0));
	vline(bmp_screen, game.screen_x-130, 40, game.screen_y - 276, makecol(255, 210, 125));
	vline(bmp_screen, game.screen_x-129, 40, game.screen_y - 276, makecol(255, 190, 76));

	// the list pieces
	draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_TOP].dat, 0, 45); // 2 high

	for (int w = 0; w < heightInPixels; w += 84) { // pieces are 84 pixels high
		draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_PIECE].dat, 0, w);
	}

	draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_BOTTOM].dat, 0, (heightInPixels - 2));

	int x = game.screen_x - 69;
	draw_sprite(bmp_screen, backgroundList, x, 45);

	// at the right lines
	vline(bmp_screen, game.screen_x-1, 44, game.screen_y - 276, makecol(153, 105, 0));
	vline(bmp_screen, game.screen_x-2, 44, game.screen_y - 276, makecol(182, 125, 12));
	vline(bmp_screen, game.screen_x-3, 44, game.screen_y - 276, makecol(202, 141, 16));

	destroy_bitmap(backgroundList);

	draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_SIDEBAR_PIECE].dat, (game.screen_x - 130), 42);
}
