/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

#include "player/brains/missions/cPlayerBrainMission.h"
#include "structs.h"
#include "utils/cRectangle.h"

enum class eTransferType {
    NONE,                               // nothing to transfer
    NEW_STAY,                           // bring a new unit, and let the carryall stay
    NEW_LEAVE,                          // bring a new unit, and let the carryall go back (and die)
    PICKUP,                             // carry an existing unit, bring to new location
    DIE,                                // will die when reaching goal-cell (added for proper animation)
};

enum class eActionType {
    GUARD,                              // on guard (scanning for enemy activity)
    MOVE,                               // moving
    CHASE,                              // chasing a unit to attack
    ATTACK_CHASE,                       // attacking (not moving), will chase when target is out of range
    ATTACK,                             // attacking (not moving), will revert to GUARD when target is out of range
};

enum class eFindBestStructureResultReason {
    FREE_STRUCTURE,                             // nobody is heading towards this structure, so we can freely occupy it
    OTHER_UNIT_ALSO_HEADS_TOWARDS_STRUCTURE,    // the structure is found is also being chased by another unit
    OCCUPIED,                                   // structure is occupied (last resort, hoping it will become free soon)
    NO_RESULT,                                  // nothing found...
};

/**
 * A result when finding a best structure.
 */
struct sFindBestStructureResult {
    cAbstractStructure * structure;
    eFindBestStructureResultReason reason;
};

enum class eHeadTowardsStructureResult {
    FAILED_NO_STRUCTURE_AVAILABLE,
    SUCCESS_AWAITING_CARRYALL,
    SUCCESS_RETURNING,
};

// Reinforcement data (loaded from ini file)
struct sReinforcement
{
    int iSeconds;       // FPS based, every second this decreases (0 = deliver)
    int iUnitType;      // what unit?
    int iPlayer;         // to who?
    int iCell;          // Where to?
};

inline std::string eActionTypeString(eActionType actionType) {
    switch (actionType) {
        case eActionType::GUARD: return "GUARD";
        case eActionType::ATTACK_CHASE: return "ATTACK_CHASE";
        case eActionType::ATTACK: return "ATTACK";
        case eActionType::MOVE: return "MOVE";
        case eActionType::CHASE: return "CHASE";
        default:
            assert(false);
            break;
    }
    return "";
}

class cUnit {

public:

    cUnit();
    ~cUnit();

	float fExperience;	// experience gained by unit

    int iID;            // index of unit in the unit array

    int iType;          // type of unit

    int iGroup;         // belongs to group...

    int iTempHitPoints; // temp hold back for 'reinforced' / 'dumping' and 'repairing' units

    int iPlayer;        // belongs to player

    // Movement
    int iNextCell;      // where to move to (next cell)
    int iGoalCell;      // the goal cell (goal of path)    
//    float iOffsetX;       // X offset
//    float iOffsetY;       // Y offset
    int iPath[MAX_PATH_SIZE];    // path of unit
    int iPathIndex;     // where are we?
    int iPathFails;     // failed...
    bool bCalculateNewPath; // calculate new path?	

    // carryall stuff
	bool bCarryMe;		// carry this unit when moving it around?
	int  iCarryAll;		// any carry-all that will pickup this unit... so this unit knows
						// it should wait when moving, etc, etc


    // WHEN ATTACKING
    int iAttackUnit;      // attacking unit id
    int iAttackStructure; // attack structure id
    int iAttackCell;      // attacking a cell (which is force attack)

    // Action given code
    int iUnitID;        // Unit ID to attack/pickup, etc
    int iStructureID;   // structure ID to attack/bring to (refinery)/capture

    // Harvester specific
    int iCredits;       // credits stored in this baby

	// Carry-All specific

    // -1 = none, 0 = new (and stay), 1 = carrying existing unit , 2 = new (and leave)
    // iUnitIDWithinStructure = unit we CARRY (when TransferType == 1)
    // iTempHitPoints = hp of unit when transfertype = 1
    eTransferType m_transferType;

	int iCarryTarget;	// Unit ID to carry, but is not carried yet
	int iBringTarget;	// Where to bring the carried unit (when iUnitIDWithinStructure > -1)
	int iNewUnitType;	// new unit that will be brought, will be this type
	int lastDroppedOffCell; // last cell where we dropepd off a unit

    // Drawing
    int iBodyFacing;    // Body of tanks facing
    int iHeadFacing;    // Head of tanks facing
    int iBodyShouldFace;    // where should the unit body look at?
    int iHeadShouldFace;    // where should the unit look at?
    int iFrame;         // framed (animated stuff)

    bool bSelected;     // selected or not?
    bool bHovered;      // mouse hovers over this unit or not?

	float fExpDamage();	// experience damage by bullet (extra damage that is)

    bool isWithinViewport(cRectangle *viewport) const;

    // ------------
    void init(int i);        // inits units

    int draw_x();
    int draw_y();

    int pos_x();
    int pos_y();

    int pos_x_centered();
    int pos_y_centered();

    int center_draw_x();
    int center_draw_y();

    void draw_health();
	void draw_experience();
	void draw_spice();
    void draw();
	void draw_path() const;
    bool isValid() const;     // valid unit?

    void shoot(int iTargetCell);  // shoot at goalcell

	void die(bool bBlowUp, bool bSquish);			// die!

    void updateCellXAndY();        // updateCellXAndY status

    void think();

    void thinkFast_move_airUnit();  // aircraft specific
    void think_move_foot(); // soldiers specific
    void thinkFast_move();  // thinking about movement (which is called upon a faster rate)
                        // for wheeled/tracked units
    void think_attack();
    void thinkFast_guard();

	void log(const std::string& txt) const;

    void think_hit(int iShotUnit, int iShotStructure);

    int getNextCellToMoveTo(); // what is the next cell to move to

    void move_to_enter_structure(cAbstractStructure *pStructure, eUnitActionIntent intent);
    void move_to(int iGoalCell);
    void move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup, eUnitActionIntent intent);
    void move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup);

    /**
     * Finds best structure type. This is based on distance and on availability.
     *
     * This function:
     * - checks if there is any structure that no unit is heading towards to
     * if failed:
     * - checks if there is any structure with no unit in it
     * if failed:
     * - checks if there is any structure of structure type (closest)
     *
     * Returns pointer to found structure.
     * @param structureType
     * @return
     */
    sFindBestStructureResult findBestStructureCandidateToHeadTo(int structureType);

    /**
     * Finds structureType, and if found it will move unit towards it with the action intent given.
     * If allowCarryallTransfer is given, this function will also check if there is a free carry-all and let the
     * unit be transported via that way to the structure.
     *
     * Best structure candidate is based on distance towards unit. (closest == best)
     *
     * @param structureType
     * @param allowCarryallTransfer
     * @param actionIntent
     * @return
     */
    eHeadTowardsStructureResult findBestStructureCandidateAndHeadTowardsItOrWait(int structureType, bool allowCarryallTransfer,
                                                                                 eUnitActionIntent actionIntent);

    cAbstractStructure * findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(int structureType);
    cAbstractStructure * findClosestAvailableStructureType(int structureType);
    cAbstractStructure * findClosestStructureType(int structureType);

	// carryall-functions:
	void carryall_order(int iuID, eTransferType transferType, int iBring, int iTpe);

    // -------------
	int TIMER_blink;	    // blink

    int TIMER_move;         // movement timer
    int TIMER_movewait;     // wait for move think...
    int TIMER_movedelay;    // if given, it will delay movement

    int TIMER_thinkwait;    // wait with normal thinking..

    float TIMER_turn;       // turning around
    int TIMER_frame;        // frame

    int TIMER_harvest;      // harvesting

    int TIMER_guard;        // guard scanning timer
    int TIMER_bored;        // how long are we bored?

    int TIMER_attack;       // when to shoot?

    int TIMER_wormtrail;    // when to spawn a trail when moving

    s_UnitInfo& getUnitInfo() const;

    /**
     * Return current health normalized (between 0.0 and 1.0)
     * @return
     */
    float getHealthNormalized();

    /**
     * Return temp health hitpoints normalized (between 0.0 and 1.0) (when unit is 'hidden')
     * @return
     */
    float getTempHealthNormalized();

    /**
     * is this unit an airborn unit?
     * @return
     */
    bool isAirbornUnit() const {
        return getUnitInfo().airborn;
    }

    /**
     * An air unit that may be attacked according to generic game rules, ie this does not say anything
     * about if the unit *can* attack it. (use canAttackAirUnits function for that)
     * @return
     */
    bool isAttackableAirUnit() {
        return iType == ORNITHOPTER;
    }

    bool isHarvester() {
        return iType == HARVESTER;
    }

    bool isSandworm() const;

    /**
     * Can this unit attack air units?
     * @return
     */
    bool canAttackAirUnits() {
        return getUnitInfo().canAttackAirUnits;
    }

    bool isInfantryUnit() const;

    int getBmpWidth() const;

    int getBmpHeight() const;

    void recreateDimensions();

    void thinkFast_position();

    bool isMovingBetweenCells();

    bool isIdle() {
        return m_action == eActionType::GUARD;
    }

    void takeDamage(int damage);

    void setHp(int hp) {
        // debug purposes only
        iHitPoints = hp;
    }

    /**
     * Returns true if dead (ie hitpoints <= 0). Also notice, this *ALSO* returns true (for now) when a
     * unit is picked up by a carry-all. It is made 'deadish' by setting the iHitpoints to < 0 and remembering
     * the hitpoints in the tempHitpoints variable.
     * @return
     */
    bool isDead() {
        return iHitPoints <= 0;
    }

    bool isDamaged();

    void restoreFromTempHitPoints();

    void setMaxHitPoints();

    void setCell(int cll);
    int getCell() { return iCell; }
    int getCellX() { return iCellX; }
    int getCellY() { return iCellY; }

    cPlayer *getPlayer();

    void assignMission(int aMission);
    void unAssignMission();
    bool isAssignedAnyMission();
    bool isAssignedMission(int aMission);

    int getPlayerId() const;

    /**
     * Returns type of unit (ie QUAD, TANK, etc)
     *
     * @return
     */
    int getType() const;

    static std::string eUnitActionIntentString(eUnitActionIntent intent);

    bool isUnableToMove();

    void attackUnit(int targetUnit);
    void attackUnit(int targetUnit, bool chaseWhenOutOfRange);
    void attackStructure(int targetStructure);
    void attackCell(int cell);

    /**
     * Figures out if at cell is a unit, structure or nothing, and invokes the appropiate attackUnit/Structure/Cell function.
     * Assumes unit should move closer (chase) to target
     * @param cell
     */
    void attackAt(int cell);

    /**
     * Can squish infantry units
     * @return
     */
    bool canSquishInfantry();

    /**
     * Returns true if this unit can be squished by other units
     * @return
     */
    bool canBeSquished();

    void carryAll_transferUnitTo(int unitIdToTransfer, int destinationCell);

    void hideUnit();

    void actionGuard();

    bool belongsTo(const cPlayer *pPlayer) const;

    bool isMarkedForRemoval();

    void draw_debug();

    void setBoundParticleId(int particleId);

    int getHitPoints() { return iHitPoints; }

    bool isEligibleForRepair();

    void thinkFast();

private:
    eActionType m_action;
    eUnitActionIntent intent;

    int willBePickedUpBy;	// is unit picked up (by carry-all), if so by which one?
    bool bPickedUp;     // did this unit pick up a unit? (this unit is a carry-all or frigate)

    bool bRemoveMe; // if true, then this unit wants to be cleared (ie, is no longer valid)

    int mission; // is this unit assigned to a mission?

    int iHitPoints;     // hitpoints of unit

    bool isReinforcement; // is this a 'real' reinforcement or not? (applies to Carry-All only)

    int unitsEaten; // worm related

    bool isSaboteur();

    void forgetAboutCurrentPathAndPrepareToCreateNewOne();
    void forgetAboutCurrentPathAndPrepareToCreateNewOne(int timeToWait);

    int iCell;          // cell of unit

    int iCellX;         // my cell x
    int iCellY;         // my cell y

    int boundParticleId; // when a unit is damaged, it can spawn a particle on top of it (ie smoke)

    // absolute x, y coordinates (pixel based). Do note, these are oriented at top left of cell, and
    // thus are snapped to the grid.
    float posX, posY;

    void setPosX(float newVal);
    void setPosY(float newVal);

    eUnitMoveToCellResult moveToNextCellLogic();

    cRectangle dimensions;

    int getRange() const;

    int getSight() const;

    void think_turn_to_desired_body_facing();

    void selectTargetForOrnithopter(cPlayer *pPlayer);

    void think_carryAll();

    void thinkActionAgnostic();       // thinking in general

    int determineNewFacing(int currentFacing, int desiredFacing);

    bool setAngleTowardsTargetAndFireBullets(int distance);

    void startChasingTarget();

    int getFaceAngleToCell(int cell) const;

    void attack(int goalCell, int unitId, int structureId, int attackCell, bool chaseWhenOutOfRange);

    int findNewDropLocation(int unitTypeToDrop, int cell) const;

    /**
     * Returns a pointer to a structure the unit wants to enter. If the unit wants to enter an invalid structure,
     * this function will return nullptr.
     * @return
     */
    cAbstractStructure *getStructureUnitWantsToEnter() const;

    cUnit& getUnitToPickupOrDrop() const;

    bool findAndOrderCarryAllToBringMeToStructureAtCell(cAbstractStructure *candidate, int destCell);

    void awaitBeingPickedUpToBeTransferedByCarryAllToStructure(cAbstractStructure *candidate);

    void unitWillNoLongerBeInteractingWithStructure() const;

    void think_attack_sandworm();

    void createExplosionParticle();

    void createSquishedParticle();

    void forgetAboutUnitToPickUp();

    void tellCarryAllThatWouldPickMeUpToForgetAboutMe() const;

    int draw_x(int bmpWidth);

    int draw_y(int bmpHeight);

    bool isAbleToGuard();

    void thinkFast_guard_sandworm();

    int findNearbyStructureToAttack(int range);

    int findNearbyStructureThatCanDamageUnitsToAttack(int range);

    int findNearbyGroundUnitToAttack(int range);

    int findNearbyAirUnitToAttack(int range);


    int getTurnSpeed();

    void think_MVC();
    void think_ornithopter();
    void think_harvester();

    void setAction(eActionType action);
};


int UNIT_CREATE(int iCll, int unitType, int iPlayer, bool bOnStart);

/**
 * creates a unit, the isReinforcement flag is true when the unit is created for / by reinforcements. This
 * flag will make sure to trigger a different event type (not CREATED, but REINFORCED) so that we can distinguish
 * between them.
 *
 * @param iCll
 * @param unitType
 * @param iPlayer
 * @param bOnStart
 * @param isReinforcement
 * @return
 */
int UNIT_CREATE(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement);

int CREATE_PATH(int iUnitId, int iPathCountUnits);

void REINFORCE(int iPlr, int iTpe, int iCll, int iStart);

/**
 * Allows overriding reinforcement flag, ie used when a unit is reinforced by construction or other way, rather
 * than a 'real' reinforcement.
 * @param iPlr
 * @param iTpe
 * @param iCll
 * @param iStart
 * @param isReinforcement
 */
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart, bool isReinforcement);

int RETURN_CLOSE_GOAL(int iCll, int iMyCell, int iID);

int UNIT_find_harvest_spot(int id);

int CARRYALL_FREE_FOR_TRANSFER(int iPlayer);

int CARRYALL_TRANSFER(int iuID, int iGoal);

int UNIT_FREE_AROUND_MOVE(int iUnit);

void SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType);
void INIT_REINFORCEMENT();
