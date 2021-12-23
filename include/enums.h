#ifndef ENUMS_H_
#define ENUMS_H_

#include <assert.h>

enum eAnimationDirection {
    ANIM_NONE, // not animating
    ANIM_OPEN, // open the dome / open door, etc
    ANIM_SPAWN_UNIT, // this is where we actually should spawn the unit... (which we don't)
    ANIM_CLOSE, // now close the dome / door
};

enum eCantBuildReason {
	/**
     * Not enough money to build it
     */
	NOT_ENOUGH_MONEY,

	/**
     * The thing to build requires an upgrade
     */
	REQUIRES_UPGRADE,

	/**
     * Already building the thing (does not take queueing into account)
     */
	ALREADY_BUILDING,

	/**
     * Requires a structure to build this (producing structure)
     */
	REQUIRES_STRUCTURE,

	/**
     * Requires an additional structure to build this (ie, IX for special units)
     */
	REQUIRES_ADDITIONAL_STRUCTURE,

	/**
     * The required thing to build is not available
     */
	NOT_AVAILABLE,

	/**
     * There is no reason we can't build it (ie SUCCESS)
     */
	NONE
};

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

// what is the intent of the action given to the unit?
enum eUnitActionIntent {
    INTENT_NONE, // none
    INTENT_MOVE, // move to target
    INTENT_ATTACK, // attack target
    INTENT_REPAIR, // repair at target
    INTENT_CAPTURE, // capture target
    INTENT_UNLOAD_SPICE // deposit spice
};

enum eUnitMoveToCellResult {

    /**
     * still busy moving between cells (offsets != 0)
     */
    MOVERESULT_BUSY_MOVING,

    /**
     * arrived at a cell (but it is not the end-goal)
     */
    MOVERESULT_AT_CELL,

    /**
     * arrived at the GOAL cell
     */
    MOVERESULT_AT_GOALCELL,

    MOVERESULT_WAIT_FOR_CARRYALL,

    /**
     * the unit has to 'wait' (ie its slowdown is in effect)
     */
    MOVERESULT_SLOWDOWN,
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

static const char* eBuildTypeString(const eBuildType &buildType) {
    switch (buildType) {
        case eBuildType::SPECIAL: return "SPECIAL";
        case eBuildType::UNIT: return "UNIT";
        case eBuildType::STRUCTURE: return "STRUCTURE";
        case eBuildType::BULLET: return "BULLET";
        case eBuildType::UPGRADE: return "UPGRADE";
        default:
            assert(false && "Unknown buildType?");
            break;
    }
    return "";
}

namespace buildOrder {
	enum eBuildOrderState {
		PROCESSME,
		BUILDING,
		REMOVEME
	};

    static const char* eBuildOrderStateString(const eBuildOrderState &state) {
        switch (state) {
            case eBuildOrderState::PROCESSME: return "PROCESSME";
            case eBuildOrderState::REMOVEME: return "REMOVEME";
            case eBuildOrderState::BUILDING: return "BUILDING";
            default:
                assert(false && "Unknown eBuildOrderState");
                break;
        }
        return "";
    }
}
enum eDeployTargetType {
    TARGET_NONE,

    /**
     * Player specifies exactly cell where to deploy.
     */
    TARGET_SPECIFIC_CELL,

    /**
     * Player specifies cell where to deploy, but the actual cell to deploy
     * is determined by make it more inaccurate. The inaccuracy is determined by
     * a different variable.
     */
    TARGET_INACCURATE_CELL,
};

/**
 * Used to determine how deployment is arranged. Usually AT_STRUCTURE is default behavior (ie for spawning next
 * of a structure. AT_RANDOM_CELL is used for Fremen Super Weapon.
 */
enum eDeployFromType {
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
    LIST_UPGRADES, // possible sUpgradeInfo
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

enum eMouseEventType {
	MOUSE_NONE,
	MOUSE_MOVED_TO,
	MOUSE_RIGHT_BUTTON_CLICKED,
	MOUSE_LEFT_BUTTON_CLICKED,
	MOUSE_RIGHT_BUTTON_PRESSED,
	MOUSE_LEFT_BUTTON_PRESSED,
	MOUSE_SCROLLED_UP,
	MOUSE_SCROLLED_DOWN
};

enum eKeyboardEventType {
	KEY_NONE,
	KEY_HOLD,   // key is being pressed down (hold)
//	KEY_RELEASED, // key was pressed down, but lifted (released)
    KEY_PRESSED, // the combination of the two events above (hold, then release) makes a key "pressed".
};

enum eGameEventType {
	GAME_EVENT_NONE,
    // an entity was created
    GAME_EVENT_DESTROYED,
    // an entity was deviated (switched player ownership)
    GAME_EVENT_DEVIATED,
    // an entity was discovered
    GAME_EVENT_DISCOVERED,
    // an entity was destroyed
	GAME_EVENT_CREATED,
    // damaged by projectile
    GAME_EVENT_DAMAGED,
    // damaged by decay
    GAME_EVENT_DECAY,
    // special weapon READY (ie deathhand can be launched)
    GAME_EVENT_SPECIAL_SELECT_TARGET,
    // list item is ready for placement (structure)
    GAME_EVENT_LIST_ITEM_PLACE_IT,
    // LAUNCHes special weapon (ie deathhand)
    GAME_EVENT_SPECIAL_LAUNCH,
    // special weapon has been launched! (warning)
    GAME_EVENT_SPECIAL_LAUNCHED,
    // cannot build requested thing
    GAME_EVENT_CANNOT_BUILD,
    // cannot create a path to destination, help!
    GAME_EVENT_CANNOT_CREATE_PATH,
    // list became available
    GAME_EVENT_LIST_BECAME_AVAILABLE,
    // list became unavailable
    GAME_EVENT_LIST_BECAME_UNAVAILABLE,
    // list item has been added (not necessarily available)
    GAME_EVENT_LIST_ITEM_ADDED,
    /**
     * list item has been constructed & deployed, and became available again for construction.
     * Also fired when item is an upgrade and it is finished.
     */
    GAME_EVENT_LIST_ITEM_FINISHED,
    // list item has been cancelled
	GAME_EVENT_LIST_ITEM_CANCELLED,
    // we're about to play the mission! (kind of init state)
	GAME_EVENT_ABOUT_TO_BEGIN,
    // spice bloom has been spawned on the map
	GAME_EVENT_SPICE_BLOOM_SPAWNED,
    // spice bloom has been blown up
	GAME_EVENT_SPICE_BLOOM_BLEW,
    // a player got defeated
	GAME_EVENT_PLAYER_DEFEATED,
};

// Fading action for transitioning between states
enum eFadeAction {
    FADE_NONE,  // no fading
    FADE_IN,    // fade in
    FADE_OUT    // fade out
};

#endif /* ENUMS_H_ */
