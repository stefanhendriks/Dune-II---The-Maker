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

	protected:
		void removeAllItems();
		void removeItem(int slot);
		int getFreeSlot();
		int getSlotForItem(cBuildingListItem * item);
		int getRandomizedSecondsToWait();

	private:
		cBuildingListItem * orderedItems[MAX_ITEMS_TO_ORDER];

		cPlayer * player;

		bool orderPlaced;

		// sent frigate to deliver order
		bool frigateSent;

		// when above zero, this will be substracted by one (by the think function).
		int secondsUntilArrival;

		// this is randomly set to 60 or more seconds. After the time runs out, new prices
		// will be set in the LIST_STARPORT
		int secondsUntilNewPricesWillBeCalculated;
};

#endif /* CORDERPROCESSER_H_ */
