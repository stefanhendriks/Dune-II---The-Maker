/*
 * eBuildType.h
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef EBUILDTYPE_H_
#define EBUILDTYPE_H_

// what type of thing do we build?
// a unit/structure of something special (super weapon)

// BE MINDFUL when changing order of this enum, as it is used. If somewhere the 0/1 (ordinal) is stored and assumes
// it reflects the values below, it might break when changing the order.
enum eBuildType {
	STRUCTURE, // 0
	UNIT,      // 1
	UPGRADE,   // 2
	SPECIAL    // 3
};

enum eListType {
    LIST_NONE,
    LIST_CONSTYARD,
    LIST_FOOT_UNITS, // infantry, soldier, trooper, troopers, etc.
    LIST_UNITS, // quad, trike, tanks, air units
    LIST_STARPORT, // special list to order stuff
    LIST_PALACE, // REMOVE? (ie super weapon icon should always show top left/right?)
    LIST_UPGRADES, // possible upgrades
    LIST_MAX // max amount of lists
};

static const eListType AllListTypes[] = {
        LIST_NONE,
        LIST_CONSTYARD,
        LIST_FOOT_UNITS,
        LIST_UNITS,
        LIST_STARPORT,
        LIST_PALACE,
        LIST_UPGRADES
};

// the BuildingListItemState
enum eBuildingListItemState {
	AVAILABLE,
	BUILDING,
	PENDING_UPGRADE,    // an upgrade is blocking this item to be built
	PENDING_BUILDING,   // another item being built is blocking this item to be upgraded
	UNAVAILABLE
};

#endif /* EBUILDTYPE_H_ */
