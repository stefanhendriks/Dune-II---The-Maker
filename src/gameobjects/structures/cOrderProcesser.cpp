/*
 * cOrderProcesser.cpp
 *
 *  Created on: 8-aug-2010
 *      Author: Stefan
 */
#include "../../include/d2tmh.h"

cOrderProcesser::cOrderProcesser(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	orderPlaced = false;
	frigateSent = false;
	secondsUntilArrival = -1;
	secondsUntilNewPricesWillBeCalculated = getRandomizedSecondsToWait();
	memset(pricePaidForItem, -1, sizeof(pricePaidForItem));
	removeAllItems();
	updatePricesForStarport();
	unitIdOfFrigateSent = -1;
}

cOrderProcesser::~cOrderProcesser() {
	removeAllItems();
	player = NULL;
}

cBuildingListItem * cOrderProcesser::getItemToDeploy() {
	for (int i = 0; i < MAX_ITEMS_TO_ORDER; i++) {
		if (orderedItems[i] != NULL) {
			return orderedItems[i];
		}
	}
	return NULL;
}

bool cOrderProcesser::hasOrderedAnything() {
	// the id is either > 0 (meaning slot 0 is taken, or more)
	// or the id is lower than 0, (meaning, all slots are taken)
	return getFreeSlot() > 0 || getFreeSlot() < 0;
}

void cOrderProcesser::markOrderAsDeployed(cBuildingListItem * item) {
	assert(item);
	int slot = getSlotForItem(item);
	assert(slot > -1);
	// remove item from the ordered items
	orderedItems[slot] = NULL;
	pricePaidForItem[slot] = -1;
}

bool cOrderProcesser::acceptsOrders() {
	int freeSlot = getFreeSlot();
	bool result = frigateSent == false && orderPlaced == false && freeSlot > -1;
	return result;
}

void cOrderProcesser::playTMinusSound(int seconds) {
	int soundIdToPlay = -1;

	if (seconds == 0) {
		if (player->getHouse() == ATREIDES) {
			soundIdToPlay = SOUND_VOICE_06_ATR;
		} else if (player->getHouse() == HARKONNEN) {
			soundIdToPlay = SOUND_VOICE_06_HAR;
		} else if (player->getHouse() == ORDOS) {
			soundIdToPlay = SOUND_VOICE_06_ORD;
		}
	} else {
		if (player->getHouse() == ATREIDES) {
			soundIdToPlay = (SOUND_ATR_S1 + (seconds - 1));
		} else if (player->getHouse() == HARKONNEN) {
			soundIdToPlay = (SOUND_HAR_S1 + (seconds - 1));
		} else if (player->getHouse() == ORDOS) {
			soundIdToPlay = (SOUND_ORD_S1 + (seconds - 1));
		}
	}

	if (soundIdToPlay > -1) {
		play_sound_id(soundIdToPlay, -1);
	}
}


// time based (per second)
void cOrderProcesser::think() {
	if (secondsUntilArrival > 0) {
		secondsUntilArrival--;
		char msg[255];
		sprintf(msg, "T-%d before Frigate arrival.", secondsUntilArrival);

		if (secondsUntilArrival <= 5 && player->getId() == HUMAN) {
			playTMinusSound(secondsUntilArrival);
		}

		cMessageDrawer * messageDrawer = gameDrawer->getMessageDrawer();
		if (secondsUntilArrival == 0) {
			sendFrigate();
			messageDrawer->setMessage("Frigate is arriving...");
		} else {
			messageDrawer->setMessage(msg);
		}
	}

	if (secondsUntilNewPricesWillBeCalculated > 0) {
		secondsUntilNewPricesWillBeCalculated--;
	} else {
		updatePricesForStarport();
		secondsUntilNewPricesWillBeCalculated = getRandomizedSecondsToWait();
	}
}

void cOrderProcesser::updatePricesForStarport() {
	cBuildingList * list = player->getSideBar()->getList(LIST_STARPORT);
	assert(list);
	for (int i = 0; i < MAX_ICONS; i++) {
		cBuildingListItem * item = list->getItem(i);
		if (item) {
			int id = item->getBuildId();
			int originalPrice = units[id].cost;
			int slice = originalPrice / 2;
			int newPrice = (originalPrice - slice) + (rnd(slice * 2));
			item->setBuildCost(newPrice);
		}
	}
}

void cOrderProcesser::addOrder(cBuildingListItem *item) {
	assert(item);
	int freeSlot = getFreeSlot();
	if (freeSlot > -1) {
		orderedItems[freeSlot] = item;
		// store the original paid price, so in case the player decides
		// to undo an order, he gets the same amount of money back that he paid for
		// (since the prices can still fluctuate during the order process)
		pricePaidForItem[freeSlot] = item->getBuildCost();
	}
}

void cOrderProcesser::removeOrder(cBuildingListItem *item) {
	assert(item);
	int slot = getSlotForItem(item);
	removeItem(slot);
}

int cOrderProcesser::getFreeSlot() {
	for (int i = 0; i < MAX_ITEMS_TO_ORDER; i++) {
		if (orderedItems[i] == NULL) {
			return i;
		}
	}
	return -1;
}

int cOrderProcesser::getSlotForItem(cBuildingListItem *item) {
	for (int i = 0; i < MAX_ITEMS_TO_ORDER; i++) {
		if (orderedItems[i] == item) {
			return i;
		}
	}
	return -1;
}

void cOrderProcesser::removeAllItems() {
	for (int i = 0; i < MAX_ITEMS_TO_ORDER; i++) {
		removeItem(i);
	}
}

void cOrderProcesser::removeItem(int slot) {
	assert(slot >= 0);
	assert(slot < MAX_ITEMS_TO_ORDER);
	orderedItems[slot] = NULL;
	// give money back to player
	if (pricePaidForItem[slot] > 0) {
		player->credits += pricePaidForItem[slot];
	}
	// and reset the amount
	pricePaidForItem[slot] = -1;
}

void cOrderProcesser::placeOrder() {
	orderPlaced = true;
	frigateSent = false;
	secondsUntilArrival = 10; // wait 10 seconds
}

int cOrderProcesser::getRandomizedSecondsToWait() {
	return (45 + rnd(360));
}

void cOrderProcesser::sendFrigate() {
	// iCll = structure start cell (up left), since we must go to the center
	// of the cell:
	cStructureUtils structureUtils;
	int structureId = structureUtils.findStarportToDeployUnit(player);

	if (structureId > -1) {
		// found structure
		structure[structureId]->setAnimating(true);
		int destinationCell = structure[structureId]->getCell();

		cCellCalculator * cellCalculator = new cCellCalculator(map);
		int iStartCell = cellCalculator->findCloseMapBorderCellRelativelyToDestinationCel(destinationCell);
		delete cellCalculator;

		if (iStartCell < 0) {
			logbook("cOrderProcesser::sendFrigate : unable to find start cell to spawn frigate");
			setOrderHasBeenProcessed();
		} else {
			// STEP 2: create frigate
			int unitId = UNIT_CREATE(iStartCell, FRIGATE, player->getId(), true);
			// STEP 3: assign order to frigate (use carryall order function)
			unit[unitId].carryall_order(-1, TRANSFER_NEW_LEAVE, destinationCell, -1);
			unitIdOfFrigateSent = unitId;
			frigateSent = true;
		}
	} else {
		// no starport available
		setOrderHasBeenProcessed();
	}
}

void cOrderProcesser::setOrderHasBeenProcessed() {
	orderPlaced = false;
	frigateSent = false;
	removeAllItems();
	unitIdOfFrigateSent = -1;
}
