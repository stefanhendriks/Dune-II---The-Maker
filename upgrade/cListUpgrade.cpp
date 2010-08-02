/*
 * cListUpgrade.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cListUpgrade::cListUpgrade(int theProgressLimit, int theTotalPrice, eUpgradeType theType) {
	progressLimit = theProgressLimit;
	totalPrice = theTotalPrice;
	type = theType;
	TIMER_progress = 0;
	progress = 0;

	// check, to prevent divide by zero
	if (totalPrice > 1 && progressLimit > 1) {
		pricePerTimeUnit = (((float) totalPrice) / ((float) progressLimit));
	}
}

cListUpgrade::~cListUpgrade() {
}
