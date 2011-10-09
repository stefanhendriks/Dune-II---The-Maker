/*
 * cSideBarFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cSideBarFactory *cSideBarFactory::instance = NULL;

cSideBarFactory::cSideBarFactory() {
}

cSideBarFactory *cSideBarFactory::getInstance() {
	if (instance == NULL) {
		instance = new cSideBarFactory();
	}

	return instance;
}

// construct a cSideBar
cSideBar * cSideBarFactory::createSideBar(cPlayer *thePlayer, int techlevel) {
	assert(thePlayer);
	Logger *logger = Logger::getInstance();

	int house = thePlayer->getHouse();

	cSideBar *sidebar = new cSideBar(thePlayer);

	// lists are already constructed with default constructor. Now initialize
	// them according to the techLevel.
	for (int listId = LIST_NONE; listId < LIST_MAX; listId++) {
		cBuildingList *list = cBuildingListFactory::getInstance()->createList(thePlayer, listId, techlevel, house);
		sidebar->setList(listId, list);
	}

	return sidebar;
}

