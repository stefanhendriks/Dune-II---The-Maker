#pragma once

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
