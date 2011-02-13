#include "../include/d2tmh.h"


cSimpleCalculator::cSimpleCalculator() {
}

cSimpleCalculator::~cSimpleCalculator() {
}

int cSimpleCalculator::substractWithFloor(int original, int amountToSubstract, int floor) {
	int result = original-amountToSubstract;
	if (result < floor) {
		return floor;
	}
	return result;
}
