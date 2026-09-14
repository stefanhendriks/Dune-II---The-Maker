/**
 * @file cSideBarFactory.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cSideBarFactory.h"
#include "cSideBar.h"
#include "gameobjects/players/cPlayer.h"
#include "cBuildingListFactory.h"
#include "include/sGameServices.h"

#include "include/cAssert.h"

cSideBarFactory::cSideBarFactory(cBuildingListFactory* buildingListFactory, sGameServices* services)
    : m_buildingListFactory(buildingListFactory), m_services(services)
{}

cSideBarFactory::~cSideBarFactory()
{}

// construct a cSideBar
cSideBar *cSideBarFactory::createSideBar(cPlayer *thePlayer)
{
    d2tm_assert(thePlayer!=nullptr);
    cSideBar *sidebar = new cSideBar(thePlayer);

    for (const auto listType : AllListTypes) {
        cBuildingList *list = m_buildingListFactory->createList(listType);
        sidebar->setList(listType, list);
        list->setItemBuilder(thePlayer->getItemBuilder()); // TODO: this should be easier!?
        list->serviceInit(m_services);
    }
    return sidebar;
}
