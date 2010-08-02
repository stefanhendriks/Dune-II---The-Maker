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

//	// Any house, any mission, start with this
//		list_new_item(LIST_CONSTYARD, ICON_STR_1SLAB, structures[SLAB1].cost, SLAB1, -1);
//
//	if (game.iMission >= 4)
//		list_new_item(LIST_CONSTYARD, ICON_STR_WALL, structures[WALL].cost, WALL, -1);
//
//	list_new_item(LIST_CONSTYARD, ICON_STR_WINDTRAP, structures[WINDTRAP].cost, WINDTRAP, -1);
//
//
//	// Barracks
//	if (game.iMission >= 2)
//		if (iHouse == ATREIDES || iHouse == ORDOS)
//		{
//			list_new_item(LIST_CONSTYARD, ICON_STR_BARRACKS, structures[BARRACKS].cost, BARRACKS, -1);
//
//			if (game.iMission >= 5 && game.iHouse == ORDOS)
//				list_new_item(LIST_CONSTYARD, ICON_STR_WOR, structures[WOR].cost, WOR, -1);
//		}
//		else
//			list_new_item(LIST_CONSTYARD, ICON_STR_WOR, structures[WOR].cost, WOR, -1);
//
//
//	// At start the player does not have other icons to use for building
//
//	// STARTING UNIT LISTS
//
//	// LIGHT FACTORY
//	if (iHouse == ATREIDES)
//		list_new_item(LIST_LIGHTFC, ICON_UNIT_TRIKE, units[TRIKE].cost, -1, TRIKE);
//	else if (iHouse == ORDOS)
//		list_new_item(LIST_LIGHTFC, ICON_UNIT_RAIDER, units[RAIDER].cost, -1, RAIDER);
//	else if (iHouse == HARKONNEN)
//	{
//		list_new_item(LIST_LIGHTFC, ICON_UNIT_QUAD, units[QUAD].cost, -1, QUAD);
//		game.iStructureUpgrade[LIGHTFACTORY]=1;
//	}
//
//	// HEAVY FACTORY
//	list_new_item(LIST_HEAVYFC, ICON_UNIT_TANK, units[TANK].cost, -1, TANK);
//	list_new_item(LIST_HEAVYFC, ICON_UNIT_HARVESTER, units[HARVESTER].cost, -1, HARVESTER);
//
//	// HITECH
//	list_new_item(LIST_ORNI, ICON_UNIT_CARRYALL, units[CARRYALL].cost, -1, CARRYALL);
//
//
//	// BARRACKS OR WOR
//	if (iHouse == HARKONNEN || iHouse == SARDAUKAR)
//	{
//		list_new_item(LIST_INFANTRY, ICON_UNIT_TROOPER, units[TROOPER].cost, -1, TROOPER);
//	}
//	else
//	{
//		list_new_item(LIST_INFANTRY, ICON_UNIT_SOLDIER, units[SOLDIER].cost, -1, SOLDIER);
//
//		// ordos gain trooper(s).
//		if (iHouse == ORDOS && iMission > 5)
//			list_new_item(LIST_INFANTRY, ICON_UNIT_TROOPER, units[TROOPER].cost, -1, TROOPER);
//	}
//
//
//	// PALACE
//
//
//	// STARPORT

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

	// X is the same for all list buttons
	list->setButtonDrawX(513);

	list->setAvailable(true);

	// now set it up
	if (listId == LIST_CONSTYARD) {
		list->setButtonDrawY(45);
		list->setButtonIcon(BTN_CONST_PRESSED);

		// add items
		list->addItemToList(new cBuildingListItem(SLAB1, structures[SLAB1], list));
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
			// add items (ordos has both)
			list->addItemToList(new cBuildingListItem(SOLDIER, units[SOLDIER], list));
			list->addItemToList(new cBuildingListItem(TROOPER, units[TROOPER], list));
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
		list->addItemToList(new cBuildingListItem(SIEGETANK, units[SIEGETANK], list));
		list->addItemToList(new cBuildingListItem(LAUNCHER, units[LAUNCHER], list));
		list->addItemToList(new cBuildingListItem(SONICTANK, units[SONICTANK], list));
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
		list->addItemToList(new cBuildingListItem(MISSILE, units[MISSILE], list));
		list->addItemToList(new cBuildingListItem(UNIT_FREMEN_THREE, units[UNIT_FREMEN_THREE], list));
		list->addItemToList(new cBuildingListItem(SABOTEUR, units[SABOTEUR], list));
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
	return list;
}
