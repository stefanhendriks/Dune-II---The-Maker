/*
 * cListUpgrade.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: Stefan
 */

#include "../d2tmh.h"

cListUpgrade::cListUpgrade(int theProgressLimit, int theTotalPrice, eUpgradeType theType, cBuildingListItem *theItem) {
	assert(theItem);
	progressLimit = theProgressLimit;
	totalPrice = theTotalPrice;
	type = theType;
	TIMER_progress = 0;
	progress = 0;
	item = theItem;

	// check, to prevent divide by zero
	if (totalPrice > 1 && progressLimit > 1) {
		pricePerTimeUnit = (((float) totalPrice) / ((float) progressLimit));
	}
}

cListUpgrade::~cListUpgrade() {
	// delete created item
	if (item) {
		delete item;
	}
}

int cListUpgrade::getProgressAsPercentage() {
	if (progressLimit > 0) {
		if (progress == 0) {
			return 0;
		}
		return (int)health_bar(100, progress, progressLimit);
	}
	return 100;
}
