/*
 * cOrderProcesser.cpp
 *
 *  Created on: 8-aug-2010
 *      Author: Stefan
 */
#include "cOrderProcesser.h"
#include "utils/RNG.hpp"
#include "managers/cDrawManager.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/d2tm_math.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "gameobjects/map/cMap.h"
#include <format>
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "game/cGameInterface.h"
#include "utils/cStructureUtils.h"
#include "data/gfxaudio.h"

cOrderProcesser::cOrderProcesser(cPlayer *thePlayer)
{
    d2tm_assert(thePlayer);
    m_player = thePlayer;
    m_orderPlaced = false;
    m_frigateSent = false;
    m_secondsUntilArrival = -1;
    m_secondsUntilNewPricesWillBeCalculated = 1; // initially randomize prices immediately
    memset(m_pricePaidForItem, -1, sizeof(m_pricePaidForItem));
    removeAllItems();
    m_unitIdOfFrigateSent = -1;
}

cOrderProcesser::~cOrderProcesser()
{
    removeAllItems();
    m_player = nullptr;
}

void cOrderProcesser::serviceInit(sGameServices *services)
{
    m_objects = services->objects;
    m_info = services->info;
    m_structureUtils = services->structureUtils;
    m_interface = services->ctx->getGameInterface();
    updatePricesForStarport();
}

cBuildingListItem *cOrderProcesser::getItemToDeploy()
{
    for (int i = 0; i < kMaxItemsToOrder; i++) {
        if (m_orderedItems[i] != nullptr) {
            return m_orderedItems[i];
        }
    }
    return nullptr;
}

bool cOrderProcesser::hasOrderedAnything()
{
    // the id is either > 0 (meaning slot 0 is taken, or more)
    // or the id is lower than 0, (meaning, all slots are taken)
    return getFreeSlot() > 0 || getFreeSlot() < 0;
}

void cOrderProcesser::markOrderAsDeployed(cBuildingListItem *item)
{
    d2tm_assert(item);
    int slot = getSlotForItem(item);
    d2tm_assert(slot > -1);
    // remove item from the ordered items
    m_orderedItems[slot] = nullptr;
    m_pricePaidForItem[slot] = -1;
}

bool cOrderProcesser::acceptsOrders()
{
    int freeSlot = getFreeSlot();
    bool result = m_frigateSent == false && m_orderPlaced == false && freeSlot > -1;
    result = result && m_player->hasAtleastOneStructure(STARPORT);
    return result;
}

void cOrderProcesser::playTMinusSound(int seconds)
{
    int soundIdToPlay = -1;

    if (seconds == 0) {
        if (m_player->getHouse() == ATREIDES) {
            soundIdToPlay = SOUND_VOICE_06_ATR;
        }
        else if (m_player->getHouse() == HARKONNEN) {
            soundIdToPlay = SOUND_VOICE_06_HAR;
        }
        else if (m_player->getHouse() == ORDOS) {
            soundIdToPlay = SOUND_VOICE_06_ORD;
        }
    }
    else {
        if (m_player->getHouse() == ATREIDES) {
            soundIdToPlay = (SOUND_ATR_S1 + (seconds - 1));
        }
        else if (m_player->getHouse() == HARKONNEN) {
            soundIdToPlay = (SOUND_HAR_S1 + (seconds - 1));
        }
        else if (m_player->getHouse() == ORDOS) {
            soundIdToPlay = (SOUND_ORD_S1 + (seconds - 1));
        }
    }

    if (soundIdToPlay > -1) {
        m_interface->playSound(soundIdToPlay);
    }
}


// time based (per second)
void cOrderProcesser::think()
{
    if (m_secondsUntilArrival > 0) {
        m_secondsUntilArrival--;
        std::string msg = std::format("T-{} before Frigate arrival.", m_secondsUntilArrival);

        if (m_secondsUntilArrival <= 5 && m_player->getId() == HUMAN) {
            playTMinusSound(m_secondsUntilArrival);
        }

        if (m_secondsUntilArrival == 0) {
            sendFrigate();
            m_interface->getDrawManager()->setMessage("Frigate is arriving...");
        }
        else {
            m_interface->getDrawManager()->setMessage(msg);
        }
    }

    if (m_secondsUntilNewPricesWillBeCalculated > 0) {
        m_secondsUntilNewPricesWillBeCalculated--;
    }
    else {
        updatePricesForStarport();
        m_secondsUntilNewPricesWillBeCalculated = getRandomizedSecondsToWait();
    }
}

void cOrderProcesser::updatePricesForStarport()
{
    cBuildingList *list = m_player->getSideBar()->getList(eListType::LIST_STARPORT);
    d2tm_assert(list);
    for (int i = 0; i < MAX_ICONS; i++) {
        cBuildingListItem *item = list->getItem(i);
        if (item) {
            int id = item->getBuildId();
            int originalPrice = m_info->getUnitInfo(id).cost;
            int slice = originalPrice / 2;
            int newPrice = (originalPrice - slice) + (RNG::rnd(slice * 2));
            item->setBuildCost(newPrice);
        }
    }
}

void cOrderProcesser::addOrder(cBuildingListItem *item)
{
    d2tm_assert(item);
    int freeSlot = getFreeSlot();
    if (freeSlot > -1) {
        m_orderedItems[freeSlot] = item;
        // store the original paid price, so in case the player decides
        // to undo an order, he gets the same amount of money back that he paid for
        // (since the prices can still fluctuate during the order process)
        m_pricePaidForItem[freeSlot] = item->getBuildCost();
    }
}

void cOrderProcesser::removeOrder(cBuildingListItem *item)
{
    d2tm_assert(item);
    int slot = getSlotForItem(item);
    removeItem(slot);
}

int cOrderProcesser::getFreeSlot()
{
    for (int i = 0; i < kMaxItemsToOrder; i++) {
        if (m_orderedItems[i] == nullptr) {
            return i;
        }
    }
    return -1;
}

int cOrderProcesser::getSlotForItem(cBuildingListItem *item)
{
    for (int i = 0; i < kMaxItemsToOrder; i++) {
        if (m_orderedItems[i] == item) {
            return i;
        }
    }
    return -1;
}

void cOrderProcesser::removeAllItems()
{
    for (int i = 0; i < kMaxItemsToOrder; i++) {
        removeItem(i);
    }
}

void cOrderProcesser::removeItem(int slot)
{
    d2tm_assert(slot >= 0);
    d2tm_assert(slot < kMaxItemsToOrder);
    m_orderedItems[slot] = nullptr;
    // give money back to player
    if (m_pricePaidForItem[slot] > 0) {
        m_player->giveCredits(m_pricePaidForItem[slot]);
    }
    // and reset the amount
    m_pricePaidForItem[slot] = -1;
}

void cOrderProcesser::placeOrder()
{
    m_orderPlaced = true;
    m_frigateSent = false;
    m_secondsUntilArrival = 10; // wait 10 seconds
}

int cOrderProcesser::getRandomizedSecondsToWait()
{
    return (45 + RNG::rnd(360));
}

void cOrderProcesser::sendFrigate()
{
    // iCll = structure start cell (up left), since we must go to the center
    // of the cell:
    int structureId = m_structureUtils->findStarportToDeployUnit(m_player);

    if (structureId > -1) {
        // found structure
        m_objects->getStructures()[structureId]->setAnimating(true);
        int destinationCell = m_objects->getStructures()[structureId]->getCell();

        int iStartCell = m_objects->getMap()->findCloseMapBorderCellRelativelyToDestinationCel(destinationCell);

        if (iStartCell < 0) {
            logbook("cOrderProcesser::sendFrigate : unable to find start cell to spawn frigate");
            setOrderHasBeenProcessed();
        }
        else {
            // STEP 2: create frigate
            int unitId = cUnits::unitCreate(iStartCell, FRIGATE, m_player->getId(), true);
            if (unitId < 0) {
                logbook("cOrderProcesser::sendFrigate : unable to spawn frigate");
                return;
            }

            // STEP 2b: make sure its facing the starport directly
            int iCellX = m_objects->getMapGeometry()->getCellX(iStartCell);
            int iCellY = m_objects->getMapGeometry()->getCellY(iStartCell);
            int cx = m_objects->getMapGeometry()->getCellX(destinationCell);
            int cy = m_objects->getMapGeometry()->getCellY(destinationCell);

            int d = fDegrees(iCellX, iCellY, cx, cy);
            Facing f = facingFromInt(faceAngle(d)); // get the angle

            m_objects->getUnit(unitId)->rendering.iBodyShouldFace = f;
            m_objects->getUnit(unitId)->rendering.iBodyFacing = f;
            m_objects->getUnit(unitId)->rendering.iHeadShouldFace = f;
            m_objects->getUnit(unitId)->rendering.iHeadFacing = f;

            // STEP 3: assign order to frigate (use carryall order function)
            m_objects->getUnit(unitId)->carryall_order(-1, eTransferType::NEW_LEAVE, destinationCell, -1);
            m_unitIdOfFrigateSent = unitId;
            m_frigateSent = true;
        }
    }
    else {
        // no starport available
        setOrderHasBeenProcessed();
    }
}

void cOrderProcesser::setOrderHasBeenProcessed()
{
    m_orderPlaced = false;
    m_frigateSent = false;
    m_secondsUntilArrival = -1;
    removeAllItems();
    m_unitIdOfFrigateSent = -1;
}

/**
 * Called when order processor should cancel and clear state, ie when all starports are destroyed
 */
void cOrderProcesser::clear()
{
    setOrderHasBeenProcessed();
}
