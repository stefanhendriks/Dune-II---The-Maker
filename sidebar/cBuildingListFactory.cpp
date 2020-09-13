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
		list->addItemToList(new cBuildingListItem(SLAB1, structures[SLAB1], list));

		if (techlevel >= 2) {
			if (house == ATREIDES || house == ORDOS) {
				list->addItemToList(new cBuildingListItem(BARRACKS, structures[BARRACKS], list));
			}
			if (house == HARKONNEN) {
				list->addItemToList(new cBuildingListItem(WOR, structures[WOR], list));
			}
		}

		if (techlevel >= 4) {
			//list->addItemToList(new cBuildingListItem(SLAB4, structures[SLAB4])); // only available after upgrading
			list->addItemToList(new cBuildingListItem(WALL, structures[WALL], list));
		}

		list->addItemToList(new cBuildingListItem(WINDTRAP, structures[WINDTRAP], list));
	}

	startX += 27;

	// other lists, have 40 pixels more Y , but the X remains the same
	// now set it up
	if (listId == LIST_INFANTRY) {
        list->setButtonDrawX(startX);
        list->setButtonIconIdPressed(LIST_BTN_INFANTRY);
        list->setButtonIconIdUnpressed(LIST_BTN_INFANTRY);
		if (house == ATREIDES) {
			// add items
			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
		} else if (house == HARKONNEN) {
			// add items
			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else if (house == ORDOS) {
			// add items , since ordos can have both, it is determined on structure place
//			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
//			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else {
			// do nothing
		}
	}

    startX += 27;

	if (listId == LIST_LIGHTFC) {
		list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_LIGHTFCTRY);
		list->setButtonIconIdUnpressed(LIST_BTN_LIGHTFCTRY);

		if (house == ATREIDES) {
			list->addItemToList(new cBuildingListItem(TRIKE, units[TRIKE], list));
		} else if (house == ORDOS) {
			list->addItemToList(new cBuildingListItem(RAIDER, units[RAIDER], list));
		} else if (house == HARKONNEN) {
			list->addItemToList(new cBuildingListItem(QUAD, units[QUAD], list));
		} else {
			list->addItemToList(new cBuildingListItem(TRIKE, units[TRIKE], list));
			list->addItemToList(new cBuildingListItem(RAIDER, units[RAIDER], list));
			list->addItemToList(new cBuildingListItem(QUAD, units[QUAD], list));
		}
	}

	startX += 27;

	if (listId == LIST_HEAVYFC) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_HEAVYFCTRY);
		list->setButtonIconIdUnpressed(LIST_BTN_HEAVYFCTRY);

		list->addItemToList(new cBuildingListItem(TANK, units[TANK], list));
		list->addItemToList(new cBuildingListItem(HARVESTER, units[HARVESTER], list));
	}

	startX += 27;

	if (listId == LIST_ORNI) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_HIGHTECH);
		list->setButtonIconIdUnpressed(LIST_BTN_HIGHTECH);

		list->addItemToList(new cBuildingListItem(CARRYALL, units[CARRYALL], list));
	}

	startX += 27;

	if (listId == LIST_STARPORT) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_STARPORT);
		list->setButtonIconIdUnpressed(LIST_BTN_STARPORT);

		list->addItemToList(new cBuildingListItem(INFANTRY, units[INFANTRY], list));
		list->addItemToList(new cBuildingListItem(TROOPERS, units[TROOPERS], list));
		list->addItemToList(new cBuildingListItem(TRIKE, units[TRIKE], list));
		list->addItemToList(new cBuildingListItem(QUAD, units[QUAD], list));
		list->addItemToList(new cBuildingListItem(TANK, units[TANK], list));
		list->addItemToList(new cBuildingListItem(MCV, units[MCV], list));
		list->addItemToList(new cBuildingListItem(HARVESTER, units[HARVESTER], list));
		list->addItemToList(new cBuildingListItem(LAUNCHER, units[LAUNCHER], list));
		list->addItemToList(new cBuildingListItem(SIEGETANK, units[SIEGETANK], list));
		list->addItemToList(new cBuildingListItem(CARRYALL, units[CARRYALL], list));
	}

	startX += 27;

	if (listId == LIST_PALACE) {
        list->setButtonDrawX(startX);
		list->setButtonIconIdPressed(LIST_BTN_PALACE);
		list->setButtonIconIdUnpressed(LIST_BTN_PALACE);

		// special weapons
		switch (house) {
			case ATREIDES:
				list->addItemToList(new cBuildingListItem(UNIT_FREMEN_THREE, units[UNIT_FREMEN_THREE], list));
				break;
			case HARKONNEN:
				list->addItemToList(new cBuildingListItem(MISSILE, units[MISSILE], list));
				break;
			case ORDOS:
				list->addItemToList(new cBuildingListItem(SABOTEUR, units[SABOTEUR], list));
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
