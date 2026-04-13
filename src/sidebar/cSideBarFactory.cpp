#include "cSideBarFactory.h"
#include "cSideBar.h"
#include "player/cPlayer.h"
#include "cBuildingListFactory.h"

#include <cassert>

cSideBarFactory::cSideBarFactory()
{}

cSideBarFactory::~cSideBarFactory()
{}

// construct a cSideBar
cSideBar *cSideBarFactory::createSideBar(cPlayer *thePlayer)
{
    assert(thePlayer!=nullptr);
    cSideBar *sidebar = new cSideBar(thePlayer);

    for (const auto listType : AllListTypes) {
        cBuildingList *list = cBuildingListFactory::getInstance()->createList(listType);
        sidebar->setList(listType, list);
        list->setItemBuilder(thePlayer->getItemBuilder()); // TODO: this should be easier!?
    }
    return sidebar;
}
