/*
 * cSimpleCalculator.cpp
 *
 *  Created on: 2-nov-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cSimpleCalculator::cSimpleCalculator() {
}

cSimpleCalculator::~cSimpleCalculator() {
}

// substract *amountToSubstract* from *original*. While respecting the absolute bottom limit
// given. (ie, 10-11, limit 5 will return 5)
int cSimpleCalculator::substract(int original, int amountToSubstract, int bottomLimit) {
	int result = original-amountToSubstract;
	if (result < bottomLimit) {
		return bottomLimit;
	}
	return result;
}
