/*
 * cBuildingListFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"
#include "cBuildingListFactory.h"


cBuildingListFactory *cBuildingListFactory::instance = NULL;

cBuildingListFactory::cBuildingListFactory() {
}

cBuildingListFactory::~cBuildingListFactory() {
}


cBuildingListFactory *cBuildingListFactory::getInstance() {
	if (instance == nullptr) {
		instance = new cBuildingListFactory();
	}

	return instance;
}

int cBuildingListFactory::getButtonDrawY() {
    // 6 pixels margin below horizontal candybar, so it lines up nice horizontally with the sphere
    // of the vertical candybar at the left of the building list icons
	return cSideBar::TopBarHeight + cSideBar::HeightOfMinimap + cSideBar::HorizontalCandyBarHeight + 6;
}

int cBuildingListFactory::getButtonDrawXStart() {
	return (game.screen_x - 200) + 2;
}


/**
 * Initialize list according to techlevel. This will also remove all previously set icons.
 *
 * @param list
 * @param listType
 * @param techlevel
 * @param house
 */
void cBuildingListFactory::initializeList(cBuildingList *list, eListType listType) {
	assert(list != NULL);

	// first clear the list
	list->removeAllItems();

	// Y is the same for all list buttons
	list->setButtonDrawY(getButtonDrawY());

    int widthOfButtonIncludingMargin = 33;
    list->setButtonWidth(widthOfButtonIncludingMargin);
//    list->setButtonHeight()

	int startX = getButtonDrawXStart();

	// now set it up
	if (listType == eListType::LIST_CONSTYARD) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_CONSTYARD);
		list->setButtonIconIdUnpressed(LIST_BTN_CONSTYARD);
	}


    startX += widthOfButtonIncludingMargin;

	// other lists, have 40 pixels more Y , but the X remains the same
	// now set it up
	if (listType == eListType::LIST_FOOT_UNITS) {
        list->setButtonDrawX(startX);
        list->setButtonIconIdPressed(LIST_BTN_INFANTRY);
        list->setButtonIconIdUnpressed(LIST_BTN_INFANTRY);
	}

    startX += widthOfButtonIncludingMargin;

	if (listType == eListType::LIST_UNITS) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_FACTORY);
		list->setButtonIconIdUnpressed(LIST_BTN_FACTORY);
	}

	startX += widthOfButtonIncludingMargin;

	if (listType == eListType::LIST_STARPORT) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_STARPORT);
		list->setButtonIconIdUnpressed(LIST_BTN_STARPORT);
	}

	startX += widthOfButtonIncludingMargin;

	if (listType == eListType::LIST_PALACE) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_PALACE);
		list->setButtonIconIdUnpressed(LIST_BTN_PALACE);
	}

	startX += widthOfButtonIncludingMargin;

	if (listType == eListType::LIST_UPGRADES) {
        list->setButtonDrawX(startX);
        // temp, use CONST YARD
		list->setButtonIconIdPressed(LIST_BTN_UPGRADE);
		list->setButtonIconIdUnpressed(LIST_BTN_UPGRADE);

		// the contents of the list is determined elsewhere
	}

}

/**
 * Create new instance of list, initialize and return it.
 *
 * @param listType
 * @param techlevel
 * @return
 */
cBuildingList * cBuildingListFactory::createList(eListType listType) {
	cBuildingList * list = new cBuildingList(listType);
    initializeList(list, listType);
	return list;
}

void cBuildingListFactory::destroy() {
    if (instance) {
        delete instance;
    }
}
