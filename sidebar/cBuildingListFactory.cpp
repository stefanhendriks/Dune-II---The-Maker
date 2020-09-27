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

		// add items
		list->addStructureToList(SLAB1, 0);

		if (techlevel >= 2) {
			if (house == ATREIDES || house == ORDOS) {
				list->addStructureToList(BARRACKS, 0);
			}
			if (house == HARKONNEN) {
				list->addStructureToList(WOR, 0);
			}
		}

		if (techlevel >= 4) {
			//list->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4])); // only available after upgrading
			list->addStructureToList(WALL, 0);
		}

		list->addStructureToList(WINDTRAP,0);
	}

	startX += 27;

	// other lists, have 40 pixels more Y , but the X remains the same
	// now set it up
	if (listId == LIST_FOOT_UNITS) {
        list->setButtonDrawX(startX);
        list->setButtonIconIdPressed(LIST_BTN_INFANTRY);
        list->setButtonIconIdUnpressed(LIST_BTN_INFANTRY);

//		if (house == ATREIDES) {
//			// add items
//			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list, 0));
//		} else if (house == HARKONNEN) {
//			// add items
//			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list, 1));
//		} else if (house == ORDOS) {
//			// add items , since ordos can have both, it is determined on structure place
////			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list, 0));
////			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list, 0));
//		} else {
//			// do nothing
//		}
	}

    startX += 27;

	if (listId == LIST_UNITS) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_LIGHTFCTRY);
		list->setButtonIconIdUnpressed(LIST_BTN_LIGHTFCTRY);

		if (house == ATREIDES) {
			list->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
		} else if (house == ORDOS) {
			list->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
		} else if (house == HARKONNEN) {
			list->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
		} else {
			list->addUnitToList(TRIKE, SUBLIST_LIGHTFCTRY);
			list->addUnitToList(RAIDER, SUBLIST_LIGHTFCTRY);
			list->addUnitToList(QUAD, SUBLIST_LIGHTFCTRY);
		}

		list->addUnitToList(TANK, SUBLIST_HEAVYFCTRY);
		list->addUnitToList(HARVESTER, SUBLIST_HEAVYFCTRY);

		list->addUnitToList(CARRYALL, SUBLIST_HIGHTECH);
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
