/*
 * cOrderProcesser.cpp
 *
 *  Created on: 8-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cOrderProcesser::cOrderProcesser(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	orderPlaced = false;
	frigateSent = false;
	secondsUntilArrival = -1;
	secondsUntilNewPricesWillBeCalculated = 45 + rnd(360);
	removeAllItems();
	updatePricesForStarport();
}

cOrderProcesser::~cOrderProcesser() {
	removeAllItems();
	player = NULL;
}


bool cOrderProcesser::acceptsOrders() {
//	int freeSlot = getFreeSlot();
//	return frigateSent == false && orderPlaced == false && freeSlot > -1;
	return true;
}

// time based (per second)
void cOrderProcesser::think() {
	if (secondsUntilArrival > 0) {
		secondsUntilArrival--;
		if (secondsUntilArrival == 0) {
			// TODO: spawn frigate
			logbook("spawn frigate");
		} else {
			char msg[255];
			sprintf(msg, "T-%d before Frigate arrival.", secondsUntilArrival);
			game.set_message(msg);
		}
	}


	if (secondsUntilNewPricesWillBeCalculated > 0) {
		secondsUntilNewPricesWillBeCalculated--;
		// wait
	} else {
		updatePricesForStarport();
		secondsUntilNewPricesWillBeCalculated = 45 + rnd(360);
	}
}

void cOrderProcesser::updatePricesForStarport() {
	logbook("updatePricesForStarport");
//	cBuildingList * list = player->getSideBar()->getList(LIST_STARPORT);
//	assert(list);
//	for (int i = 0; i < MAX_ICONS; i++) {
//		cBuildingListItem * item = list->getItem(i);
//		if (item) {
//			int id = item->getBuildId();
//			int originalPrice = structures[id].cost;
//			int slice = originalPrice / 2;
//			int newPrice = (originalPrice - slice) + (rnd(slice * 2));
//			item->setBuildCost(newPrice);
//		}
//	}
}

void cOrderProcesser::addOrder(cBuildingListItem *item) {
	assert(item);
	int freeSlot = getFreeSlot();
	if (freeSlot > -1) {
		logbook("add order");
		orderedItems[freeSlot] = item;
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
}

void cOrderProcesser::placeOrder() {
	orderPlaced = true;
	frigateSent = false;
	secondsUntilArrival = 10; // wait 10 seconds
}
