/*
 * cListUpgrade.h
 *
 *  Created on: Sep 5, 2009
 *      Author: Stefan
 */

/**
 * Representation of an upgrade, belonging to a list. Is constructed by a cListUpgradeFactory
 * where the 'type' of upgrade is given.
 *
 */

#ifndef CLISTUPGRADE_H_
#define CLISTUPGRADE_H_

class cListUpgrade {
public:
	cListUpgrade(int theProgressLimit, int theTotalPrice, eUpgradeType theType);
	virtual ~cListUpgrade();

	int getTimerProgress() { return TIMER_progress; }
	int getProgress() { return progress; }
	int getProgressLimit() { return progressLimit; }
	int getTotalPrice() { return totalPrice; }
	float getPricePerTimeUnit() { return pricePerTimeUnit; }

	void setProgress(int value) { progress = value; }
	void setProgressLimit(int value) { progressLimit = value; }
	void setTotalPrice(int value) { totalPrice = value; }
	void setPricePerTimeUnit(float value) { pricePerTimeUnit = value; }
	void setTimerProgress(int value) { TIMER_progress = value; }

private:

	int TIMER_progress;		// timer to increase progress
	int progress;			// how much progress is made
	int progressLimit;		// how much progress is needed?
	int totalPrice;			// price for upgrade
	float pricePerTimeUnit;	// on construction of this class, determined what it would
							// be for one time unit (ie, one slice of progress)

	eUpgradeType type;		// type of upgrade
};

#endif /* CLISTUPGRADE_H_ */
