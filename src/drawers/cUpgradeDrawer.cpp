/*
 * cUpgradeDrawer.cpp
 *
 *  Created on: 3-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cUpgradeDrawer::cUpgradeDrawer() {
}

cUpgradeDrawer::~cUpgradeDrawer() {

}

bool cUpgradeDrawer::shouldDrawButtonForSelectedList(cPlayer * thePlayer, cBuildingList * theSelectedList) {
	assert(thePlayer);
	assert(theSelectedList);
	int techLevel = thePlayer->getTechLevel();
	int upgradeLevel = theSelectedList->getUpgradeLevel();

	cUpgradeUtils upgradeUtils;
	return upgradeUtils.canUpgradeList(thePlayer, theSelectedList->getType(), techLevel, upgradeLevel);
}

void cUpgradeDrawer::drawUpgradeButtonForSelectedListIfNeeded(cPlayer * thePlayer, cBuildingList * theSelectedList) {
	assert(thePlayer);
	if (theSelectedList == NULL)
		return;

	bool shouldDrawButton = shouldDrawButtonForSelectedList(thePlayer, theSelectedList);

	if (shouldDrawButton) {
		drawUpgradeButton(thePlayer, theSelectedList);

		if (theSelectedList->isUpgrading()) {
			drawUpgradeProgress(thePlayer, theSelectedList);
		}
	}
}

void cUpgradeDrawer::drawUpgradeButton(cPlayer * thePlayer, cBuildingList * theSelectedList) {
	assert(thePlayer);
	assert(theSelectedList);
	int techLevel = thePlayer->getTechLevel();
	int upgradeLevel = theSelectedList->getUpgradeLevel();

	cUpgradeUtils upgradeUtils;
	bool canPayForUpgrade = upgradeUtils.canPlayerPayForUpgradeForList(thePlayer, theSelectedList->getType(), techLevel, upgradeLevel);
	bool isBuildingItem = theSelectedList->isBuildingItem();

	if (canPayForUpgrade && !isBuildingItem) {
		draw_sprite(bmp_screen, (BITMAP *) gfxinter[BTN_UPGRADE].dat, 29, 0);
	} else {
		// draw a dark version
		// TODO: optimize this, by making a dark version once and then re-draw that one, instead of constructing
		// it over and over again
		BITMAP *bmp_trans = create_bitmap(((BITMAP *) gfxinter[BTN_UPGRADE].dat)->w, ((BITMAP *) gfxinter[BTN_UPGRADE].dat)->h);
		clear_to_color(bmp_trans, makecol(255, 0, 255));

		// copy
		draw_sprite(bmp_trans, (BITMAP *) gfxinter[BTN_UPGRADE].dat, 0, 0);

		// make black
		rectfill(bmp_screen, 29, 0, 187, 29, makecol(0, 0, 0));

		// set blender
		set_trans_blender(0, 0, 0, 128);

		// trans (makes upgrade button show like it is disabled)
		draw_trans_sprite(bmp_screen, bmp_trans, 29, 0);

		// destroy
		destroy_bitmap(bmp_trans);
	}
}

void cUpgradeDrawer::drawUpgradeProgress(cPlayer * thePlayer, cBuildingList * theSelectedList) {
	assert(thePlayer);
	assert(theSelectedList);

	int listId = theSelectedList->getType();
	cListUpgrade * upgrade = thePlayer->getUpgradeBuilder()->getListUpgrade(listId);
	assert(upgrade);

	if (upgrade) {
		int iDrawXLimit = (int) 157 - health_bar(157, upgrade->getProgress(), upgrade->getProgressLimit());

		if (iDrawXLimit > -1) {
			int iColor = makecol(255, 255, 255);
			BITMAP *temp = create_bitmap(157, 28);
			clear_to_color(temp, makecol(255, 0, 255));

			// TODO: make util function for this (duplicate code!)
			if (thePlayer->getHouse() == ATREIDES)
				iColor = makecol(0, 0, 255);
			if (thePlayer->getHouse() == HARKONNEN)
				iColor = makecol(255, 0, 0);
			if (thePlayer->getHouse() == ORDOS)
				iColor = makecol(0, 255, 0);
			if (thePlayer->getHouse() == SARDAUKAR)
				iColor = makecol(255, 0, 255);

			rectfill(temp, 0, 0, (157 - iDrawXLimit), 30, iColor);

			draw_trans_sprite(bmp_screen, temp, 30, 1);
			destroy_bitmap(temp);
		}
	}
}
