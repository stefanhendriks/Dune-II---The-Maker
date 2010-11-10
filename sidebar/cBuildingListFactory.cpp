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

int cBuildingListFactory::getButtonDrawX() {
	return game.screen_x - 127;
}

int cBuildingListFactory::getButtonDrawYStart() {
	return 45;
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
	list->setButtonDrawX(getButtonDrawX());

	list->setAvailable(false);

	// now set it up
	if (listId == LIST_CONSTYARD) {
		list->setButtonDrawY(45);
		list->setButtonIconIdPressed(BTN_CONST_PRESSED);
		list->setButtonIconIdUnpressed(BTN_CONST_UNPRESSED);

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
			list->setButtonIconIdPressed(BTN_INFANTRY_PRESSED);
			list->setButtonIconIdUnpressed(BTN_INFANTRY_UNPRESSED);
			// add items
			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
		} else if (house == HARKONNEN) {
			list->setButtonIconIdPressed(BTN_TROOPER_PRESSED);
			list->setButtonIconIdUnpressed(BTN_TROOPER_UNPRESSED);
			// add items
			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else if (house == ORDOS) {
			list->setButtonIconIdPressed(BTN_TRPINF_PRESSED);
			list->setButtonIconIdUnpressed(BTN_TRPINF_UNPRESSED);
			// add items , since ordos can have both, it is determined on structure place
//			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
//			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
		} else {
			// do nothing
		}
	}

	if (listId == LIST_LIGHTFC) {
		list->setButtonDrawY(125);
		list->setButtonIconIdPressed(BTN_LIGHTFC_PRESSED);
		list->setButtonIconIdUnpressed(BTN_LIGHTFC_UNPRESSED);

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
		list->setButtonIconIdPressed(BTN_HEAVYFC_PRESSED);
		list->setButtonIconIdUnpressed(BTN_HEAVYFC_UNPRESSED);

		list->addItemToList(new cBuildingListItem(TANK, units[TANK], list));
		list->addItemToList(new cBuildingListItem(HARVESTER, units[HARVESTER], list));
	}

	if (listId == LIST_ORNI) {
		list->setButtonDrawY(205);
		list->setButtonIconIdPressed(BTN_ORNI_PRESSED);
		list->setButtonIconIdUnpressed(BTN_ORNI_UNPRESSED);

		list->addItemToList(new cBuildingListItem(CARRYALL, units[CARRYALL], list));
	}

	if (listId == LIST_STARPORT) {
		list->setButtonDrawY(245);
		list->setButtonIconIdPressed(BTN_STARPORT_PRESSED);
		list->setButtonIconIdUnpressed(BTN_STARPORT_UNPRESSED);

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

	if (listId == LIST_PALACE) {
		list->setButtonDrawY(285);
		list->setButtonIconIdPressed(BTN_PALACE_PRESSED);
		list->setButtonIconIdUnpressed(BTN_PALACE_UNPRESSED);

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
