#include "../include/d2tmh.h"
#include "cSideBar.h"

#include "managers/cDrawManager.h"
#include "utils/cLog.h"
#include "utils/cSoundPlayer.h"

#include <fmt/core.h>

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

void cSideBar::setList(eListType listType, cBuildingList* list) {
    int listId = eListTypeAsInt(listType);
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
	thinkProgressAnimation();
}

void cSideBar::drawMessageBarWithItemInfo(cBuildingListItem *item) const {
    drawManager->getMessageDrawer()->setMessage(item->getInfo().c_str());
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
bool cSideBar::startBuildingItemIfOk(eListType listType, int buildId) const {
    cBuildingListItem *pItem = getBuildingListItem(listType, buildId);
    if (pItem) {
        return startBuildingItemIfOk(pItem);
    } else {
        logbook(fmt::format("ERROR: startBuildingItemIfOk with listType[{}] and buildId[{}] did not find an item to build!",
        eListTypeAsInt(listType), buildId ));
    }
    return false;
}

cBuildingListItem * cSideBar::getBuildingListItem(eListType listType, int buildId) const {
    int listId = eListTypeAsInt(listType);

    cBuildingList *pList = lists[listId];
    if (pList == nullptr) return nullptr;

    return pList->getItemByBuildId(buildId);
}

void cSideBar::thinkProgressAnimation() {
    int startPos = eListTypeAsInt(eListType::LIST_CONSTYARD);
    for (int i = startPos; i < LIST_MAX; i++) {
        cBuildingList *list = getList(i);
        if (list == nullptr) continue;
        if (!list->isAvailable()) continue; // not available, so no interaction possible

        list->think();
    }
}

void cSideBar::onMouseAt(const s_MouseEvent &event) {
    isMouseOverSidebarValue = event.coords.x > (game.m_screenX - cSideBar::SidebarWidth);
    drawManager->getMessageDrawer()->setKeepMessage(isMouseOverSidebarValue);

    if (selectedListID < 0) return;

    // when mouse is selecting a list, and over an item, then draw message bar...!?
    cBuildingList *list = getList(selectedListID);
    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    // TODO: move this whole 'draw something' out of the onMouseMovedTo function
    drawMessageBarWithItemInfo(item);
}

void cSideBar::onMouseClickedLeft(const s_MouseEvent &event) {
    int startPos = eListTypeAsInt(eListType::LIST_CONSTYARD);

    // button interaction
    for (int i = startPos; i < LIST_MAX; i++) {
        if (i == selectedListID) continue; // skip selected list for button interaction
        cBuildingList *list = getList(i);
        if (list == nullptr) continue;
        if (!list->isAvailable()) continue; // not available, so no interaction possible

        // interaction is possible.
        if (list->isOverButton(event.coords.x, event.coords.y)) {
            // clicked on it. Set focus on this one
            setSelectedListId(eListTypeFromInt(i));
            game.playSound(SOUND_BUTTON, 64); // click sound
            break;
        }
    }

    if (selectedListID < 0) return;

    // when mouse pressed, build item if over item
    cBuildingList *list = getList(selectedListID);

    if (list == nullptr) return;

    if (!list->isAvailable()) {
        // unselect this list
        player->getSideBar()->setSelectedListId(eListType::LIST_NONE);
        return;
    }

    cOrderDrawer * orderDrawer = drawManager->getOrderDrawer();

    // allow clicking on the order button, send event through...
    if (list->getType() == eListType::LIST_STARPORT) {
        orderDrawer->onNotify(event);
    }

    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    drawMessageBarWithItemInfo(item);

    if (list->getType() != eListType::LIST_STARPORT) {
        // icon is in "Place it" mode, meaning if clicked the "place the thing" state should be set
        if (item->shouldPlaceIt()) {
            player->setContextMouseState(eMouseState::MOUSESTATE_PLACE);
        } else if (item->shouldDeployIt()) {
            player->setContextMouseState(eMouseState::MOUSESTATE_DEPLOY);
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
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // anything but the starport can 'build' things
    if (list->getType() != eListType::LIST_STARPORT) {
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

            // notify game that the item just has been cancelled, just before the actual removal
            s_GameEvent newEvent {
                    .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED,
                    .entityType = item->getBuildType(),
                    .entityID = -1,
                    .player = player,
                    .entitySpecificType = item->getBuildId(),
                    .atCell = -1,
                    .isReinforce = false,
                    .buildingListItem = item
            };

            game.onNotifyGameEvent(newEvent);
            // else, only the number is decreased (used for queueing)

            cItemBuilder *itemBuilder = player->getItemBuilder();
            itemBuilder->removeItemFromList(item);
        }
    }
}

void cSideBar::onNotifyMouseEvent(const s_MouseEvent &event) {
    cGameControlsContext *pContext = player->getGameControlsContext();

    if (!pContext->isMouseOnSidebarOrMinimap()) {
        // we're only interested in mouse movement
        if (event.eventType == eMouseEventType::MOUSE_MOVED_TO) {
            onMouseAt(event);
        }
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
        default:
            return;
    }
}

void cSideBar::onNotify(const s_GameEvent &event) {
    // event is for specific player, and we are not that player...
    if (event.player != nullptr && event.player != player) {
        return;
    }

    // for us, or for no player in particular
    switch(event.eventType) {
        case eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET:
            onSpecialReadyToDeployEvent(event);
            break;
        case eGameEventType::GAME_EVENT_LIST_ITEM_PLACE_IT:
            onListItemReadyToPlaceEvent(event);
            break;
        case eGameEventType::GAME_EVENT_LIST_BECAME_AVAILABLE:
            onListBecameAvailableEvent(event);
            break;
        case eGameEventType::GAME_EVENT_LIST_BECAME_UNAVAILABLE:
            onListBecameUnavailableEvent(event);
            break;
        case eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN:
            findFirstActiveListAndSelectIt();
            break;
        default:
            break;
    }
}

void cSideBar::onListItemReadyToPlaceEvent(const s_GameEvent &event) const {
    event.buildingListItem->getList()->startFlashing();
}

void cSideBar::onSpecialReadyToDeployEvent(const s_GameEvent &event) const {
    event.buildingListItem->getList()->startFlashing();
}

void cSideBar::onListBecameAvailableEvent(const s_GameEvent &event) {
    event.buildingList->startFlashing();
}

void cSideBar::onListBecameUnavailableEvent(const s_GameEvent &event) {
    cBuildingList *pList = getSelectedList();
    if (pList == event.buildingList) {
        findFirstActiveListAndSelectIt();
    }
}

cBuildingList *cSideBar::getSelectedList() const {
    if (selectedListID < 0) return nullptr;
    return lists[selectedListID];
}

void cSideBar::findFirstActiveListAndSelectIt() {
    logbook(fmt::format("cSideBar::findFirstActiveListAndSelectIt - current selectedListID is [{}]", selectedListID));
    for (int i = 0; i < LIST_MAX; i++) {
        cBuildingList *pList = lists[i];
        if (pList && pList->isAvailable()) {
            setSelectedListId(eListTypeFromInt(i));
            break;
        }
    }
    logbook(fmt::format("cSideBar::findFirstActiveListAndSelectIt - new selectedListID is [{}]", selectedListID));
}

void cSideBar::setSelectedListId(eListType value) {
    logbook(fmt::format("cSideBar::setSelectedListId -  m_PlayerId = [{}] - old value [{}], new [{}]", 
        player->getId(), selectedListID, eListTypeAsInt(value)));

    int oldListId = selectedListID;
    selectedListID = eListTypeAsInt(value);
    if (oldListId > -1 && oldListId < LIST_MAX) {
        lists[oldListId]->setSelected(false);
    }

    if (selectedListID > -1 && selectedListID < LIST_MAX) {
        lists[selectedListID]->setSelected(true);
    }
}
