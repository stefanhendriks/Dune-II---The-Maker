#include "../include/d2tmh.h"
#include "cSideBar.h"


cSideBar::cSideBar(cPlayer * thePlayer) : player(thePlayer) {
    assert(thePlayer != nullptr && "Expected player to be not null!");
	selectedListID = -1; // nothing is selected
	isMouseOverSidebarValue = false;
	memset(lists, 0, sizeof(lists));
}

cSideBar::~cSideBar() {
	for (int i = 0; i < LIST_MAX; i++) {
        cBuildingList *pList = lists[i];
        if (pList) {
            pList->removeAllItems();
			delete pList; // delete list (also removes pointers within the list)
			lists[i] = nullptr;
		}
	}
}

void cSideBar::setList(int listId, cBuildingList* list) {
	assert(listId > -1);
	assert(listId < LIST_MAX);
	if (lists[listId]) {
		logbook("WARNING: Setting list, while list already set. Deleting old entry before assigning new.");
		delete lists[listId];
	}

	lists[listId] = list;
}

/**
 * Thinking for sidebar - timer based
 */
void cSideBar::think() {
	thinkAvailabilityLists();
	thinkProgressAnimation();
}

/**
 * Think about the availability of lists.
 * TODO: move this to the cBuildingListUpdater? (event driven??)
 */
void cSideBar::thinkAvailabilityLists() {
	// CONSTYARD LIST
	cBuildingList * constyardList = getList(LIST_CONSTYARD);
	assert(constyardList);

	constyardList->setAvailable(player->hasAtleastOneStructure(CONSTYARD));

	// INFANTRY LIST
	cBuildingList * infantryList = getList(LIST_FOOT_UNITS);

    infantryList->setAvailable(player->hasBarracks() || player->hasWor());

	// LIGHTFC LIST
	cBuildingList * listUnits = getList(LIST_UNITS);
    listUnits->setAvailable(player->hasAtleastOneStructure(LIGHTFACTORY) ||
                            player->hasAtleastOneStructure(HEAVYFACTORY) ||
                            player->hasAtleastOneStructure(HIGHTECH)
    );

	// PALACE LIST
	cBuildingList * palaceList = getList(LIST_PALACE);
	palaceList->setAvailable(player->hasAtleastOneStructure(PALACE));

	// STARPORT LIST
	cBuildingList * starportList = getList(LIST_STARPORT);
	starportList->setAvailable(player->hasAtleastOneStructure(STARPORT));

	// when available, check if we accept orders
	if (starportList->isAvailable()) {
		cOrderProcesser * orderProcesser = player->getOrderProcesser();
		bool acceptsOrders = orderProcesser->acceptsOrders();
		starportList->setAcceptsOrders(acceptsOrders);
	}

    // UPGRADES LIST (for now is always available - later should be depending on items in list)
    cBuildingList * upgradesList = getList(LIST_UPGRADES);
    upgradesList->setAvailable(true);
}

void cSideBar::drawMessageBarWithItemInfo(cBuildingList *list, cBuildingListItem *item) const {
    char msg[255];
    if (list->isAcceptsOrders()) {
        // build time is in global time units , using a timer cap of 35 * 5 miliseconds = 175 miliseconds
        int buildTimeInMs = item->getTotalBuildTime() * 175;
        // now we have in miliseconds, we know the amount of seconds too.
        int seconds = buildTimeInMs / 1000;

        if (item->isTypeStructure()) {
            s_Structures structureType = item->getS_Structures();
            sprintf(msg, "$%d | %s | %d Power | %d Secs", item->getBuildCost(), structureType.name, (structureType.power_give - structureType.power_drain), seconds);
        } else if (item->isTypeUnit()) {
            s_UnitP unitType = item->getS_UnitP();
            if (item->getBuildCost() > 0) {
                sprintf(msg, "$%d | %s | %d Secs", item->getBuildCost(), unitType.name, seconds);
            } else {
                sprintf(msg, "%s", unitInfo[item->getBuildId()].name);
            }
        } else if (item->isTypeUpgrade()){
            s_Upgrade upgrade = item->getS_Upgrade();
            sprintf(msg, "UPGRADE: $%d | %s | %d Secs", item->getBuildCost(), upgrade.description, seconds);
        } else if (item->isTypeSpecial()) {
            s_Special special = item->getS_Special();
            sprintf(msg, "$%d | %s | %d Secs", item->getBuildCost(), special.description, seconds);
        } else {
            sprintf(msg, "UNKNOWN BUILD TYPE");
        }

        drawManager->getMessageDrawer()->setMessage(msg);
    }
}

bool cSideBar::startBuildingItemIfOk(cBuildingListItem *item) const {
    if (item == nullptr) {
        return false;
    }

    if (item->shouldPlaceIt()) {
        logbook("Attempting to build an item that is in the \"Place it\" mode - which should not happen - ignoring!");
        return false;
    }

    cBuildingList *list = item->getList();
    if (item->isAvailable()) {
        // Item should not be placed, so it can be built
        cItemBuilder *itemBuilder = player->getItemBuilder();
        bool firstOfItsListType = itemBuilder->isBuildListItemTheFirstOfItsListType(item);

        if (item->isQueuable()) {
            itemBuilder->addItemToList(item);
        } else if (firstOfItsListType) { // may only build if there is nothing else in the list type being built
            itemBuilder->addItemToList(item);
        } else {
            // fail
            return false;
        }

        list->setLastClickedId(item->getSlotId());
        return true;
    }
    return false;
}

/**
 * Starts building item of type <b>buildId</b>, returns false if the construction cannot commence.
 * @param listId
 * @param buildId
 * @return
 */
bool cSideBar::startBuildingItemIfOk(int listId, int buildId) const {
    cBuildingListItem *pItem = getBuildingListItem(listId, buildId);
    if (pItem) {
        return startBuildingItemIfOk(pItem);
    } else {
        char msg[255];
        sprintf(msg, "ERROR: startBuildingItemIfOk with listId[%d] and buildId[%d] did not find an item to build!", listId, buildId);
        logbook(msg);
    }
    return false;
}

cBuildingListItem * cSideBar::getBuildingListItem(int listId, int buildId) const {
    if (listId < 0) return nullptr;
    if (listId >= LIST_MAX) return nullptr;

    cBuildingList *pList = lists[listId];
    if (pList == nullptr) return nullptr;

    return pList->getItemByBuildId(buildId);
}

void cSideBar::thinkProgressAnimation() {
    for (int i = LIST_CONSTYARD; i < LIST_MAX; i++) {
        cBuildingList *list = getList(i);
        if (list == nullptr) continue;
        if (!list->isAvailable()) continue; // not available, so no interaction possible

        for (int j = 0; j < MAX_ITEMS; j++) {
            cBuildingListItem *item = list->getItem(j);
            if (item == nullptr) continue;
            if (!item->isBuilding()) continue;

            int frameToBecome = item->calculateBuildProgressFrameBasedOnBuildProgress();

            if (item->getBuildProgressFrame() < frameToBecome) {
                item->decreaseProgressFrameTimer();
                if (item->getProgressFrameTimer() < 0) {
                    item->increaseBuildProgressFrame();
                    item->resetProgressFrameTimer();
                }
            }
        }
    }
}

void cSideBar::onMouseAt(const s_MouseEvent &event) {
    isMouseOverSidebarValue = event.x > (game.screen_x - cSideBar::SidebarWidth);

    if (selectedListID < 0) return;

    // when mouse is selecting a list, and over an item, then draw message bar...!?
    cBuildingList *list = getList(selectedListID);
    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.x, event.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    // TODO: move this whole 'draw something' out of the onMouseAt function
    drawMessageBarWithItemInfo(list, item);
}

void cSideBar::onMouseClickedLeft(const s_MouseEvent &event) {

    // button interaction
    for (int i = LIST_CONSTYARD; i < LIST_MAX; i++) {
        if (i == selectedListID) continue; // skip selected list for button interaction
        cBuildingList *list = getList(i);
        if (list == nullptr) continue;
        if (!list->isAvailable()) continue; // not available, so no interaction possible

        // interaction is possible.
        if (list->isOverButton(event.x, event.y)) {
            // clicked on it. Set focus on this one
            selectedListID = i;
            char msg[255];
            sprintf(msg, "selectedListID becomes [%d], m_PlayerId = [%d]", i, player->getId());
            logbook(msg);
            play_sound_id(SOUND_BUTTON, 64); // click sound
            break;
        }
    }

    if (selectedListID < 0) return;

    // when mouse pressed, build item if over item
    cBuildingList *list = getList(selectedListID);

    if (list == nullptr) return;

    if (!list->isAvailable()) {
        // unselect this list
        player->getSideBar()->setSelectedListId(-1);
        return;
    }

    cOrderDrawer * orderDrawer = drawManager->getOrderDrawer();

    // allow clicking on the order button, send event through...
    if (list->getType() == LIST_STARPORT) {
        orderDrawer->onNotify(event);
    }

    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.x, event.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    drawMessageBarWithItemInfo(list, item);

    if (list->getType() != LIST_STARPORT) {
        // icon is in "Place it" mode, meaning if clicked the "place the thing" state should be set
        if (item->shouldPlaceIt()) {
            game.bPlaceIt = true;
        } else if (item->shouldDeployIt()) {
            game.bDeployIt = true; // this puts the "deploy/launch it at mode" state active.
        } else {
            startBuildingItemIfOk(item);
        }
    } else {
        cOrderProcesser * orderProcesser = player->getOrderProcesser();

        // add orders
        if (orderProcesser->acceptsOrders()) {
            if (player->hasEnoughCreditsFor(item->getBuildCost())) {
                item->increaseTimesOrdered();
                orderProcesser->addOrder(item);
                player->substractCredits(item->getBuildCost());
            }
        }
    }
}

void cSideBar::onMouseClickedRight(const s_MouseEvent &event) {
    if (selectedListID < 0) return;

    // when mouse pressed, build item if over item
    cBuildingList *list = getList(selectedListID);

    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.x, event.y);
    if (item == nullptr) return;

    // anything but the starport can 'build' things
    if (list->getType() != LIST_STARPORT) {
        cancelBuildingListItem(item);
    } else {
        cOrderProcesser * orderProcesser = player->getOrderProcesser();

        assert(orderProcesser);
        if (!orderProcesser->isOrderPlaced()) {
            if (item->getTimesOrdered() > 0) {
                item->decreaseTimesOrdered();
                orderProcesser->removeOrder(item);
            }
        }
    }
}

void cSideBar::cancelBuildingListItem(cBuildingListItem *item) {
    if (item->getTimesToBuild() > 0) {
        item->decreaseTimesToBuild();
        item->setPlaceIt(false);
        item->setDeployIt(false);

        if (item->getTimesToBuild() == 0) {
            cLogger::getInstance()->log(LOG_INFO, COMP_SIDEBAR, "Cancel construction", "Item is last item in queue, will give money back.");
            // only give money back for item that is being built
            if (item->isBuilding()) {
                // calculate the amount of money back:
                player->giveCredits(item->getRefundAmount());
                player->getBuildingListUpdater()->onBuildItemCancelled(item);
            }
            item->setIsBuilding(false);
            item->resetProgress();
            cItemBuilder *itemBuilder = player->getItemBuilder();
            itemBuilder->removeItemFromList(item);
        }
        // else, only the number is decreased (used for queueing)
    }
}

void cSideBar::onNotify(const s_MouseEvent &event) {
    cGameControlsContext *pContext = player->getGameControlsContext();

    if (!pContext->isMouseOnSidebarOrMinimap()) {
        // ignore these events because mouse is not on sidebar or minimap
        return;
    }

    switch (event.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseAt(event);
            return;
        case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED:
            onMouseClickedLeft(event);
            return;
        case eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseClickedRight(event);
            return;
    }
}
