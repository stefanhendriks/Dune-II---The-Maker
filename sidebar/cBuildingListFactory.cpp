/*
 * cBuildingListFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

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

/**
 * Initialize list according to techlevel. This will also remove all previously set icons.
 *
 * @param list
 * @param listId
 * @param techlevel
 * @param house
 */
void cBuildingListFactory::initializeList(cPlayer *player, cBuildingList *list, int listId, int techlevel, int house) {
	assert(list != NULL);

	// first clear the list
	list->removeAllItems();

	// X is the same for all list buttons
	list->setButtonDrawX(513);

	list->setAvailable(false);

	// now set it up
	if (listId == LIST_CONSTYARD) {

		list->setButtonDrawY(45);
		list->setButtonIcon(BTN_CONST_PRESSED);

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

	// other lists, have 40 pixels more Y , but the X remains the same
	// now set it up
	if (listId == LIST_INFANTRY) {
		list->setButtonDrawY(85);
		if (house == ATREIDES) {
			list->setButtonIcon(BTN_INFANTRY_PRESSED);
			// add items
			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
		} else if (house == HARKONNEN) {
			list->setButtonIcon(BTN_TROOPER_PRESSED);
			// add items
			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else if (house == ORDOS) {
			list->setButtonIcon(BTN_TRPINF_PRESSED);
			// add items , since ordos can have both, it is determined on structure place
//			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
//			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else {
			// do nothing
		}
	}

	if (listId == LIST_LIGHTFC) {
		list->setButtonDrawY(125);
		list->setButtonIcon(BTN_LIGHTFC_PRESSED);

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

	if (listId == LIST_HEAVYFC) {
		list->setButtonDrawY(165);
		list->setButtonIcon(BTN_HEAVYFC_PRESSED);

		list->addItemToList(new cBuildingListItem(TANK, units[TANK], list));
		list->addItemToList(new cBuildingListItem(HARVESTER, units[HARVESTER], list));
	}

	if (listId == LIST_ORNI) {
		list->setButtonDrawY(205);
		list->setButtonIcon(BTN_ORNI_PRESSED);

		list->addItemToList(new cBuildingListItem(CARRYALL, units[CARRYALL], list));
	}

	if (listId == LIST_STARPORT) {
		list->setButtonDrawY(245);
		list->setButtonIcon(BTN_STARPORT_PRESSED);

		// a lot of units are in this list
	}

	if (listId == LIST_PALACE) {
		list->setButtonDrawY(285);
		list->setButtonIcon(BTN_PALACE_PRESSED);

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
cBuildingList * cBuildingListFactory::createList(cPlayer * player, int listId, int techlevel, int house) {
	cBuildingList * list = new cBuildingList(listId);
	initializeList(player, list, listId, techlevel, house);
	list->setTypeOfList(listId); // list id == type (see cSideBarFactory)
	return list;
}
