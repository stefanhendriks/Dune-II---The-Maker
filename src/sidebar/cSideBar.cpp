#include "cSideBar.h"

#include "building/cItemBuilder.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "managers/cDrawManager.h"
#include "player/cPlayer.h"
#include "utils/cLog.h"
#include "utils/cSoundPlayer.h"
#include "controls/cGameControlsContext.h"
#include "gameobjects/structures/cOrderProcesser.h"
#include "data/gfxaudio.h"

#include <format>
#include <cassert>



cSideBar::cSideBar(cPlayer *thePlayer) : m_player(thePlayer)
{
    assert(thePlayer != nullptr && "Expected player to be not null!");
    m_selectedListID = -1; // nothing is selected
    m_isMouseOverSidebarValue = false;
    memset(m_lists, 0, sizeof(m_lists));
}

cSideBar::~cSideBar()
{
    for (int i = 0; i < LIST_MAX; i++) {
        cBuildingList *pList = m_lists[i];
        if (pList) {
            pList->removeAllItems();
            delete pList; // delete list (also removes pointers within the list)
            m_lists[i] = nullptr;
        }
    }
}

void cSideBar::setList(eListType listType, cBuildingList *list)
{
    int listId = eListTypeAsInt(listType);
    if (m_lists[listId]) {
        logbook("WARNING: Setting list, while list already set. Deleting old entry before assigning new.");
        delete m_lists[listId];
    }

    m_lists[listId] = list;
}

/**
 * Thinking for sidebar - timer based
 */
void cSideBar::think()
{
    thinkProgressAnimation();
}

void cSideBar::drawMessageBarWithItemInfo(cBuildingListItem *item) const
{
    game.m_drawManager->setMessage(item->getInfo());
}

bool cSideBar::startBuildingItemIfOk(cBuildingListItem *item) const
{
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
        cItemBuilder *itemBuilder = m_player->getItemBuilder();
        bool firstOfItsListType = itemBuilder->isBuildListItemTheFirstOfItsListType(item);

        if (item->isQueuable()) {
            itemBuilder->addItemToList(item);
        }
        else if (firstOfItsListType) {   // may only build if there is nothing else in the list type being built
            itemBuilder->addItemToList(item);
        }
        else {
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
bool cSideBar::startBuildingItemIfOk(eListType listType, int buildId) const
{
    cBuildingListItem *pItem = getBuildingListItem(listType, buildId);
    if (pItem) {
        return startBuildingItemIfOk(pItem);
    }
    else {
        logbook(std::format("ERROR: startBuildingItemIfOk with listType[{}] and buildId[{}] did not find an item to build!",
                            eListTypeAsInt(listType), buildId ));
    }
    return false;
}

cBuildingListItem *cSideBar::getBuildingListItem(eListType listType, int buildId) const
{
    int listId = eListTypeAsInt(listType);

    cBuildingList *pList = m_lists[listId];
    if (pList == nullptr) return nullptr;

    return pList->getItemByBuildId(buildId);
}

void cSideBar::thinkProgressAnimation()
{
    int startPos = eListTypeAsInt(eListType::LIST_CONSTYARD);
    for (int i = startPos; i < LIST_MAX; i++) {
        cBuildingList *list = getList(i);
        if (list == nullptr) continue;
        if (!list->isAvailable()) continue; // not available, so no interaction possible

        list->think();
    }
}

void cSideBar::onMouseAt(const s_MouseEvent &event)
{
    m_isMouseOverSidebarValue = event.coords.x > (game.m_screenW - cSideBar::SidebarWidth);
    game.m_drawManager->setKeepMessage(m_isMouseOverSidebarValue);

    if (m_selectedListID < 0) return;

    // when mouse is selecting a list, and over an item, then draw message bar...!?
    cBuildingList *list = getList(m_selectedListID);
    cBuildingListDrawer *buildingListDrawer = game.m_drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    // TODO: move this whole 'draw something' out of the onMouseMovedTo function
    drawMessageBarWithItemInfo(item);
}

void cSideBar::onMouseClickedLeft(const s_MouseEvent &event)
{
    int startPos = eListTypeAsInt(eListType::LIST_CONSTYARD);

    // button interaction
    for (int i = startPos; i < LIST_MAX; i++) {
        if (i == m_selectedListID) continue; // skip selected list for button interaction
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

    if (m_selectedListID < 0) return;

    // when mouse pressed, build item if over item
    cBuildingList *list = getList(m_selectedListID);

    if (list == nullptr) return;

    if (!list->isAvailable()) {
        // unselect this list
        m_player->getSideBar()->setSelectedListId(eListType::LIST_NONE);
        return;
    }

    cOrderDrawer *orderDrawer = game.m_drawManager->getOrderDrawer();

    // allow clicking on the order button, send event through...
    if (list->getType() == eListType::LIST_STARPORT) {
        orderDrawer->onNotify(event);
    }

    cBuildingListDrawer *buildingListDrawer = game.m_drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    drawMessageBarWithItemInfo(item);

    if (list->getType() != eListType::LIST_STARPORT) {
        // icon is in "Place it" mode, meaning if clicked the "place the thing" state should be set
        if (item->shouldPlaceIt()) {
            m_player->setContextMouseState(eMouseState::MOUSESTATE_PLACE);
        }
        else if (item->shouldDeployIt()) {
            m_player->setContextMouseState(eMouseState::MOUSESTATE_DEPLOY);
        }
        else {
            startBuildingItemIfOk(item);
        }
    }
    else {
        cOrderProcesser *orderProcesser = m_player->getOrderProcesser();

        // add orders
        if (orderProcesser->acceptsOrders()) {
            if (m_player->hasEnoughCreditsFor(item->getBuildCost())) {
                item->increaseTimesOrdered();
                orderProcesser->addOrder(item);
                m_player->substractCredits(item->getBuildCost());
            }
        }
    }
}

void cSideBar::onMouseClickedRight(const s_MouseEvent &event)
{
    if (m_selectedListID < 0) return;

    // when mouse pressed, build item if over item
    cBuildingList *list = getList(m_selectedListID);

    cBuildingListDrawer *buildingListDrawer = game.m_drawManager->getBuildingListDrawer();
    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, event.coords.x, event.coords.y);
    if (item == nullptr) return;

    // anything but the starport can 'build' things
    if (list->getType() != eListType::LIST_STARPORT) {
        cancelBuildingListItem(item);
    }
    else {
        cOrderProcesser *orderProcesser = m_player->getOrderProcesser();

        assert(orderProcesser);
        if (!orderProcesser->isOrderPlaced()) {
            if (item->getTimesOrdered() > 0) {
                item->decreaseTimesOrdered();
                orderProcesser->removeOrder(item);
            }
        }
    }
}

void cSideBar::cancelBuildingListItem(cBuildingListItem *item)
{
    if (item->getTimesToBuild() > 0) {
        item->decreaseTimesToBuild();
        item->setPlaceIt(false);
        item->setDeployIt(false);

        if (item->getTimesToBuild() == 0) {
            cLogger::getInstance()->log(LOG_TRACE, COMP_SIDEBAR, "Cancel construction", "Item is last item in queue, will give money back.");
            // only give money back for item that is being built
            if (item->isBuilding()) {
                // calculate the amount of money back:
                m_player->giveCredits(item->getRefundAmount());
                m_player->getBuildingListUpdater()->onBuildItemCancelled(item);
            }
            item->setIsBuilding(false);
            item->resetProgress();

            // notify game that the item just has been cancelled, just before the actual removal
            s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED,
                .entityType = item->getBuildType(),
                .entityID = -1,
                .player = m_player,
                .entitySpecificType = item->getBuildId(),
                .atCell = -1,
                .isReinforce = false,
                .buildingListItem = item
            };

            game.onNotifyGameEvent(event);
            // else, only the number is decreased (used for queueing)

            cItemBuilder *itemBuilder = m_player->getItemBuilder();
            itemBuilder->removeItemFromList(item);
        }
    }
}

void cSideBar::onNotifyMouseEvent(const s_MouseEvent &event)
{
    cGameControlsContext *pContext = m_player->getGameControlsContext();

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

void cSideBar::onNotify(const s_GameEvent &event)
{
    // event is for specific player, and we are not that player...
    if (event.player != nullptr && event.player != m_player) {
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

void cSideBar::onListItemReadyToPlaceEvent(const s_GameEvent &event) const
{
    event.buildingListItem->getList()->startFlashing();
}

void cSideBar::onSpecialReadyToDeployEvent(const s_GameEvent &event) const
{
    event.buildingListItem->getList()->startFlashing();
}

void cSideBar::onListBecameAvailableEvent(const s_GameEvent &event)
{
    event.buildingList->startFlashing();
}

void cSideBar::onListBecameUnavailableEvent(const s_GameEvent &event)
{
    cBuildingList *pList = getSelectedList();
    if (pList == event.buildingList) {
        findFirstActiveListAndSelectIt();
    }
}

cBuildingList *cSideBar::getSelectedList() const
{
    if (m_selectedListID < 0) return nullptr;
    return m_lists[m_selectedListID];
}

void cSideBar::findFirstActiveListAndSelectIt()
{
    logbook(std::format("cSideBar::findFirstActiveListAndSelectIt - current m_selectedListID is [{}]", m_selectedListID));
    for (int i = 0; i < LIST_MAX; i++) {
        cBuildingList *pList = m_lists[i];
        if (pList && pList->isAvailable()) {
            setSelectedListId(eListTypeFromInt(i));
            break;
        }
    }
    logbook(std::format("cSideBar::findFirstActiveListAndSelectIt - new m_selectedListID is [{}]", m_selectedListID));
}

void cSideBar::setSelectedListId(eListType value)
{
    logbook(std::format("cSideBar::setSelectedListId -  m_PlayerId = [{}] - old value [{}], new [{}]",
                        m_player->getId(), m_selectedListID, eListTypeAsInt(value)));

    int oldListId = m_selectedListID;
    m_selectedListID = eListTypeAsInt(value);
    if (oldListId > -1 && oldListId < LIST_MAX) {
        m_lists[oldListId]->setSelected(false);
    }

    if (m_selectedListID > -1 && m_selectedListID < LIST_MAX) {
        m_lists[m_selectedListID]->setSelected(true);
    }
}
