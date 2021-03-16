/*
 * enums.h
 *
 *  Created on: Aug 5, 2009
 *      Author: Stefan
 */

#ifndef ENUMS_H_
#define ENUMS_H_

enum eLogComponent {
	COMP_UNITS,
	COMP_STRUCTURES,
	COMP_GAMEINI,
	COMP_SCENARIOINI,
	COMP_PARTICLE,
	COMP_BULLET,
	COMP_AI,
	COMP_UPGRADE_LIST,
	COMP_BUILDING_LIST_UPDATER,
	COMP_SIDEBAR,
	COMP_MAP,
	COMP_NONE,
	COMP_INIT,
	COMP_ALLEGRO,		/** Use for allegro specific calls **/
	COMP_SETUP,
	COMP_VERSION,		/** version specific loggin messages **/
	COMP_SKIRMISHSETUP, /** When skirmish game is being set up **/
	COMP_ALFONT, 		/** ALFONT library specific **/
	COMP_SOUND, 		/** Sound related **/
	COMP_REGIONINI		/** Used for regions **/
};

enum eLogOutcome {
	OUTC_SUCCESS,
	OUTC_FAILED,
	OUTC_NONE,
	OUTC_UNKNOWN,
	OUTC_IGNOREME /** will not be printed **/
};

enum eLogLevel {
	LOG_INFO,
	LOG_TRACE,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};

// 0x01 ==   1 == "00000001"
// 0x02 ==   2 == "00000010"
// 0x04 ==   4 == "00000100"
// 0x08 ==   8 == "00001000"
// 0x10 ==  16 == "00010000"
// 0x20 ==  32 == "00100000"
// 0x40 ==  64 == "01000000"
// 0x80 == 128 == "10000000"
enum eHouseBitFlag {
    Atreides = 0x01,
    Ordos = 0x02,
    Harkonnen = 0x04,
    Sardaukar = 0x08,
    Fremen = 0x10,
    Unknown = 0x20
};


// what type of thing do we build?
// a unit/structure of something special (super weapon)

// BE MINDFUL when changing order of this enum, as it is used. If somewhere the 0/1 (ordinal) is stored and assumes
// it reflects the values below, it might break when changing the order.
enum eBuildType {
    STRUCTURE, // 0
    UNIT,      // 1
    UPGRADE,   // 2
    SPECIAL,   // 3
    BULLET     // 4 (ie, used for super weapon)
};

enum eDeployType {
    /**
     * Player specifies cell where to deploy.
     */
    AT_SPECIFIC_CELL,

    /**
     * Random cell
     */
    AT_RANDOM_CELL,

    /**
     * Deploy at structure
     */
    AT_STRUCTURE
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

#endif /* ENUMS_H_ */
