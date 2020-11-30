/*
 * cListUtils.cpp
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cListUtils::cListUtils() {
}

cListUtils::~cListUtils() {
}

int cListUtils::findListTypeByStructureType(int structureType) {
	switch (structureType) {
	case CONSTYARD:
		return LIST_CONSTYARD;
	case LIGHTFACTORY:
		return LIST_UNITS;
	case HEAVYFACTORY:
		return LIST_UNITS;
	case HIGHTECH:
		return LIST_UNITS;
	case PALACE:
		return LIST_PALACE;
	case BARRACKS:
		return LIST_FOOT_UNITS;
	case WOR:
		return LIST_FOOT_UNITS;
	case STARPORT:
		return LIST_STARPORT;
	default:
		return LIST_NONE;
	}
}
