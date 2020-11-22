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

// the BuildingListItemState
enum eBuildingListItemState {
	AVAILABLE,
	BUILDING,
	PENDING_UPGRADE,    // an upgrade is blocking this item to be built
	UNAVAILABLE
};

#endif /* EBUILDTYPE_H_ */
