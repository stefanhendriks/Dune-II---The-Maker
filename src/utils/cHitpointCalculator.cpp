#include "../include/d2tmh.h"

cHitpointCalculator::cHitpointCalculator() {
}
/**
	Return the x percent of fMax. Note, fPercent is from 1.0 (100%) to 0.0 (0%)
**/
float cHitpointCalculator::getByPercent(float fMax, float fPercent) {
	float fHealth = fMax;	// max
	fHealth *= fPercent;	// actual
	return fHealth;
}
