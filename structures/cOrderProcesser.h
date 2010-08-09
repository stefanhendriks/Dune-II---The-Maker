/*
 * cOrderProcesser.h
 *
 *  Created on: 8-aug-2010
 *      Author: Stefan
 *
 *  processes orders for starport
 *
 */

#ifndef CORDERPROCESSER_H_
#define CORDERPROCESSER_H_

#define MAX_ITEMS_TO_ORDER	8

class cOrderProcesser {

	public:
		cOrderProcesser(cPlayer * thePlayer);
		~cOrderProcesser();

		void think();	// time based (per second)
		void updatePricesForStarport();
		void addOrder(cBuildingListItem *item);
		bool acceptsOrders();	// able to accept orders?
		void removeOrder(cBuildingListItem *item);
		void placeOrder();

		bool isOrderPlaced() { return orderPlaced; }
		bool isFrigateSent() { return frigateSent; }
		bool hasOrderedAnything();
		bool hasFreeSlot() { return getFreeSlot() >= 0;	}
		cBuildingListItem * getItemToDeploy();
		void markOrderAsDeployed(cBuildingListItem * item);
		void setOrderHasBeenProcessed();

		void sendFrigate();

	protected:
		void removeAllItems();
		void removeItem(int slot);
		int getFreeSlot();
		int getSlotForItem(cBuildingListItem * item);
		int getRandomizedSecondsToWait();

	private:
		cBuildingListItem * orderedItems[MAX_ITEMS_TO_ORDER];
		int pricePaidForItem[MAX_ITEMS_TO_ORDER];

		cPlayer * player;

		bool orderPlaced;

		// sent frigate to deliver order
		bool frigateSent;
		int unitIdOfFrigateSent;

		// when above zero, this will be substracted by one (by the think function).
		int secondsUntilArrival;

		// this is randomly set to 60 or more seconds. After the time runs out, new prices
		// will be set in the LIST_STARPORT
		int secondsUntilNewPricesWillBeCalculated;
};

#endif /* CORDERPROCESSER_H_ */
