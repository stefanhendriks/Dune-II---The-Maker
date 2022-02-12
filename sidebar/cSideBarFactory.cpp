/*
 * cSideBarFactory.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cSideBarFactory *cSideBarFactory::instance = nullptr;

cSideBarFactory::cSideBarFactory() {
}

cSideBarFactory::~cSideBarFactory() {
}

cSideBarFactory *cSideBarFactory::getInstance() {
	if (instance == NULL) {
		instance = new cSideBarFactory();
	}

	return instance;
}

// construct a cSideBar
cSideBar * cSideBarFactory::createSideBar(cPlayer * thePlayer) {
	//assert(&thePlayer);
	cSideBar *sidebar = new cSideBar(thePlayer);

    for (const auto listType : AllListTypes) {
        cBuildingList *list = cBuildingListFactory::getInstance()->createList(listType);
        sidebar->setList(listType, list);
        list->setItemBuilder(thePlayer->getItemBuilder()); // TODO: this should be easier!?
    }

	return sidebar;
}

void cSideBarFactory::destroy() {
    if (instance) {
        delete instance;
    }
}

