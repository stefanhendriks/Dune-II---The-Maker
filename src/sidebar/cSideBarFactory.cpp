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
