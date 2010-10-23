/*
 * cListUtils.cpp
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cListUtils::cListUtils() {
}

cListUtils::~cListUtils() {
}

int cListUtils::findListTypeByStructureType(int structureType) {
	switch (structureType) {
	case CONSTYARD:
		return LIST_CONSTYARD;
	case LIGHTFACTORY:
		return LIST_LIGHTFC;
	case HEAVYFACTORY:
		return LIST_HEAVYFC;
	case HIGHTECH:
		return LIST_ORNI;
	case PALACE:
		return LIST_PALACE;
	case BARRACKS:
		return LIST_INFANTRY;
	case WOR:
		return LIST_INFANTRY;
	case STARPORT:
		return LIST_STARPORT;
	default:
		return LIST_NONE;
	}
}
