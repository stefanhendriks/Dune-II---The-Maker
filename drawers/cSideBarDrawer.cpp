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
		int heightInPixels = (game.screen_y - cSideBar::TopBarHeight);
		candybar = create_bitmap_ex(8, 24, heightInPixels);
		clear_to_color(candybar, makecol(0, 0, 0));

		// ball first
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BALL].dat, 0, 0); // height of ball = 25
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_TOP].dat, 0, 26); // height of top = 10
	    // now draw pieces untill the end (height of piece is 23 pixels)
	    int startY = 26 + 10; // end of ball (26) + height of top candybar (=10) , makes 36
        int heightMinimap = cSideBar::HeightOfMinimap;
        set_clip_rect(candybar, 0, 0, 24, heightMinimap - (6 + 1)); // (add 1 pixel for room between ball and bar)
	    for (int y = startY; y < (heightMinimap); y += 24) {
		    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_PIECE].dat, 0, y);
	    }
        set_clip_rect(candybar, 0, 0, candybar->w, candybar->h);

	    // note: no need to take top bar into account because 'candybar' is a separate bitmap so coords start at 0,0
	    // ball is 6 pixels higher than horizontal candybar
	    int ballY = (heightMinimap) - 6;

	    // draw bottom candybar
        draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BOTTOM].dat, 0, ballY - 10); // height of bottom = 9

	    // draw ball
        draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BALL].dat, 0, ballY); // height of ball = 25

	    // draw top candybar again
        draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_TOP].dat, 0, ballY + 26); // height of top = 10

        startY = ballY + 26 + 10;
	    for (int y = startY; y < (heightInPixels + 23); y += 24) {
		    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_PIECE].dat, 0, y);
	    }
	    // draw bottom
	    draw_sprite(candybar, (BITMAP *)gfxinter[BMP_GERALD_CANDYBAR_BOTTOM].dat, 0, heightInPixels-10); // height of top = 10
	}

	// 200 + 24 (width of candy bar) = 224
	int drawX = game.screen_x - cSideBar::SidebarWidth;
	int drawY = 40;
	draw_sprite(bmp_screen, candybar, drawX, drawY);
}

void cSideBarDrawer::drawHouseGui(const cPlayer & thePlayer) {
	assert(&thePlayer);
	set_palette(thePlayer.pal);

	// black out sidebar
	rectfill(bmp_screen, (game.screen_x-cSideBar::SidebarWidth), 0, game.screen_x, game.screen_y, makecol(0,0,0));

    // upper bar
    rectfill(bmp_screen, 0, 0, game.screen_x, 42, makecol(0,0,0));

    drawCandybar();

    // draw the list background of the icons
    drawIconsListBackground();

    // minimap at bottom right
    drawMinimap(thePlayer);

    // draw options bar (todo: move to own options drawer and delegate in drawManager)
    drawOptionsBar();
}

void cSideBarDrawer::drawBuildingLists(const cPlayer & thePlayer) {
	// draw the sidebar itself (the backgrounds, borders, etc)
	cSideBar *sidebar = thePlayer.getSideBar();

	// draw the buildlist icons
	int selectedListId = sidebar->getSelectedListID();

	for (int listId = LIST_CONSTYARD; listId < LIST_MAX; listId++) {
		cBuildingList *list = sidebar->getList(listId);
		bool isListIdSelectedList = (selectedListId == listId);
		buildingListDrawer->drawButton(list, isListIdSelectedList);
	}

    // draw background of buildlist
    BITMAP * backgroundSprite = (BITMAP *)gfxinter[BMP_GERALD_ICONLIST_BACKGROUND].dat;

    int heightOfListButton = 28 + 6;
    int drawX = game.screen_x - cSideBar::SidebarWidthWithoutCandyBar + 1;

    int startY = cSideBar::TopBarHeight + cSideBar::HeightOfMinimap + cSideBar::HorizontalCandyBarHeight +
                 heightOfListButton;

    for (; drawX < game.screen_x; drawX += backgroundSprite->w) {
        for (int drawY=startY; drawY < game.screen_y; drawY += backgroundSprite->h) {
            draw_sprite(bmp_screen, backgroundSprite, drawX, drawY);
        }
    }

    // draw the 'lines' between the icons    // draw the buildlist itself (take scrolling into account)
	cBuildingList *selectedList = NULL;

	if (selectedListId > -1) {
		selectedList = sidebar->getList(selectedListId);
        buildingListDrawer->drawList(selectedList, selectedListId);
	}

    // button interaction
    for (int i = LIST_CONSTYARD; i < LIST_MAX; i++) {
        if (i == selectedListId) continue; // skip selected list for button interaction
        cBuildingList *list = sidebar->getList(i);

        if (list->isAvailable() == false) continue; // not available, so no interaction possible

        // render hover over border
        if (list->isOverButton(mouse_x, mouse_y)) {
            buildingListDrawer->drawButtonHoverRectangle(list);
        }
    }

    cOrderDrawer * orderDrawer = drawManager->getOrderDrawer();

    // allow clicking on the order button
    if (selectedList && selectedList->getType() == LIST_STARPORT) {
        orderDrawer->drawOrderButton(thePlayer);
        if (orderDrawer->isMouseOverOrderButton()) {
            orderDrawer->drawRectangleOrderButton();
        }
    }
}

// draws the sidebar on screen
void cSideBarDrawer::drawSideBar(const cPlayer & player) {
	drawHouseGui(player);
	drawBuildingLists(player);
//	drawCapacities();
//	drawScrollButtons();
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

void cSideBarDrawer::drawMinimap(const cPlayer & player) {
	BITMAP * sprite = (BITMAP *)gfxinter[HORIZONTAL_CANDYBAR].dat;
	int drawX = (game.screen_x - sprite->w) + 1;
	// 128 pixels (each pixel is a cell) + 8 margin
    int heightMinimap = cSideBar::HeightOfMinimap;
	int drawY = cSideBar::TopBarHeight + heightMinimap;
	draw_sprite(bmp_screen, sprite, drawX, drawY);

	// 11, 10
	// 78, 60
	// ------
	// 67, 50 (width/height)

    bool hasRadarAndEnoughPower = player.hasRadarAndEnoughPower();

    if (hasRadarAndEnoughPower){
        return; // bail, because we render the minimap
    }

    // else, we render the house emblem
	rectfill(bmp_screen, drawX + 1, cSideBar::TopBarHeight + 1, game.screen_x, drawY, player.getEmblemBackgroundColor());

	if (player.isHouse(ATREIDES) || player.isHouse(HARKONNEN) || player.isHouse(ORDOS)) {
	    int bitmapId = BMP_SELECT_HOUSE_ATREIDES;

        int srcX = 11;
        int srcY = 10;

        // draw house emblem, which is the same as the 'select house x' picture, but partially
        int emblemWidth = 68;
        int emblemHeight = 50;

        if (player.isHouse(HARKONNEN)) {
            bitmapId = BMP_SELECT_HOUSE_HARKONNEN;
        }

        if (player.isHouse(ORDOS)) {
            bitmapId = BMP_SELECT_HOUSE_ORDOS;
            emblemHeight = 49;
            srcY = 11;
        }

        int emblemDesiredWidth = (emblemWidth * 2);
        int emblemDesiredHeight = (emblemHeight * 2);

        drawX += (sprite->w / 2) - (emblemDesiredWidth / 2);
        drawY = cSideBar::TopBarHeight + ((heightMinimap / 2) - (emblemDesiredHeight / 2));

        allegroDrawer->stretchBlit((BITMAP *) gfxinter[bitmapId].dat, bmp_screen, srcX, srcY, emblemWidth,
                                   emblemHeight, drawX, drawY, emblemDesiredWidth, emblemDesiredHeight);
    }
}

void cSideBarDrawer::drawOptionsBar() {
	if (optionsBar == NULL) {
		optionsBar = create_bitmap(game.screen_x, 40);
		clear_to_color(optionsBar, sidebarColor);

//		// credits
//		draw_sprite(optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOPBAR_CREDITS].dat, (game.screen_x - 240), 0);

		for (int w = 0; w < (game.screen_x + 800); w += 789) {
			draw_sprite(optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOP_BAR].dat, w, 31);
		}

	}
	draw_sprite(bmp_screen, optionsBar, 0, 0);
}

void cSideBarDrawer::drawIconsListBackground() {
	// fill
//	rectfill(bmp_screen, (game.screen_x-130), 40, game.screen_x, (game.screen_y - 160), sidebarColor);

	int heightInPixels = (game.screen_y - 315) - 45;
//	BITMAP * backgroundList = create_bitmap_ex(8, 66, heightInPixels);

	// left 'lines'
//	vline(bmp_screen, game.screen_x-132, 40, game.screen_y - 276, makecol(255, 198, 93));
//	vline(bmp_screen, game.screen_x-131, 40, game.screen_y - 276, makecol(60, 36, 0));
//	vline(bmp_screen, game.screen_x-130, 40, game.screen_y - 276, makecol(255, 210, 125));
//	vline(bmp_screen, game.screen_x-129, 40, game.screen_y - 276, makecol(255, 190, 76));

//	// the list pieces
//	draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_TOP].dat, 0, 45); // 2 high
//
//	for (int w = 0; w < heightInPixels; w += 84) { // pieces are 84 pixels high
//		draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_PIECE].dat, 0, w);
//	}
//
//	draw_sprite(backgroundList, (BITMAP *)gfxinter[BMP_GERALD_LIST_BOTTOM].dat, 0, (heightInPixels - 2));
//
//	int x = game.screen_x - 69;
//	draw_sprite(bmp_screen, backgroundList, x, 45);

//	// at the right lines
//	vline(bmp_screen, game.screen_x-1, 44, game.screen_y - 276, makecol(153, 105, 0));
//	vline(bmp_screen, game.screen_x-2, 44, game.screen_y - 276, makecol(182, 125, 12));
//	vline(bmp_screen, game.screen_x-3, 44, game.screen_y - 276, makecol(202, 141, 16));

//	destroy_bitmap(backgroundList);

//	draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GERALD_SIDEBAR_PIECE].dat, (game.screen_x - 130), 42);
}
