/*
 * cBuildingListFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cBuildingListFactory::cBuildingListFactory() {
}

cBuildingListFactory::~cBuildingListFactory() {
}

cBuildingListFactory *cBuildingListFactory::instance = NULL;

cBuildingListFactory *cBuildingListFactory::getInstance() {
	if (instance == NULL) {
		instance = new cBuildingListFactory();
	}

	return instance;
}

int cBuildingListFactory::getButtonDrawY() {
	return cSideBar::TopBarHeight + 230;
}

int cBuildingListFactory::getButtonDrawXStart() {
	return (game.screen_x - 200) + 2;
}


/**
 * Initialize list according to techlevel. This will also remove all previously set icons.
 *
 * @param list
 * @param listId
 * @param techlevel
 * @param house
 */
void cBuildingListFactory::initializeList(cBuildingList *list, int listId, int techlevel, int house) {
	assert(list != NULL);

	// first clear the list
	list->removeAllItems();

	// Y is the same for all list buttons
	list->setButtonDrawY(getButtonDrawY());

	int startX = getButtonDrawXStart();

	list->setAvailable(false);

	// now set it up
	if (listId == LIST_CONSTYARD) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_CONSTYARD);
		list->setButtonIconIdUnpressed(LIST_BTN_CONSTYARD);
	}

	startX += 27;

	// other lists, have 40 pixels more Y , but the X remains the same
	// now set it up
	if (listId == LIST_FOOT_UNITS) {
        list->setButtonDrawX(startX);
        list->setButtonIconIdPressed(LIST_BTN_INFANTRY);
        list->setButtonIconIdUnpressed(LIST_BTN_INFANTRY);
	}

    startX += 27;

	if (listId == LIST_UNITS) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_LIGHTFCTRY);
		list->setButtonIconIdUnpressed(LIST_BTN_LIGHTFCTRY);
	}

	startX += 27;

	if (listId == LIST_STARPORT) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_STARPORT);
		list->setButtonIconIdUnpressed(LIST_BTN_STARPORT);

		list->addUnitToList(INFANTRY, 0);
		list->addUnitToList(TROOPERS, 0);
		list->addUnitToList(TRIKE, 0);
		list->addUnitToList(QUAD, 0);
		list->addUnitToList(TANK, 0);
		list->addUnitToList(MCV, 0);
		list->addUnitToList(HARVESTER, 0);
		list->addUnitToList(LAUNCHER, 0);
		list->addUnitToList(SIEGETANK, 0);
		list->addUnitToList(CARRYALL, 0);
	}

	startX += 27;

	if (listId == LIST_PALACE) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_PALACE);
		list->setButtonIconIdUnpressed(LIST_BTN_PALACE);

		// special weapons
		switch (house) {
			case ATREIDES:
				list->addUnitToList(UNIT_FREMEN_THREE, 0);
				break;
			case HARKONNEN:
				list->addUnitToList(MISSILE,0);
				break;
			case ORDOS:
				list->addUnitToList(SABOTEUR, 0);
				break;
		}
	}

	startX += 27;

	if (listId == LIST_UPGRADES) {
        list->setButtonDrawX(startX);
        // temp, use CONST YARD
		list->setButtonIconIdPressed(LIST_BTN_CONSTYARD);
		list->setButtonIconIdUnpressed(LIST_BTN_CONSTYARD);

		// the contents of the list is determined elsewhere
	}

}

/**
 * Create new instance of list, initialize and return it.
 *
 * @param listId
 * @param techlevel
 * @return
 */
cBuildingList * cBuildingListFactory::createList(int listId, int techlevel, int house) {
	cBuildingList * list = new cBuildingList(listId);
    initializeList(list, listId, techlevel, house);
	list->setTypeOfList(listId); // list id == type (see cSideBarFactory)
	return list;
}
