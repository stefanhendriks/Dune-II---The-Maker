/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#ifndef AIH_H
#define AIH_H

// Computer Opponent variables

// helpers
int CLOSE_SPICE_BLOOM(int iCell);

enum cantBuildReason {
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
     * Requires a structure to build this (??) - this should not happen (anymore) though
     */
    REQUIRES_STRUCTURE,

    /**
     * There is no reason we can't build it (ie SUCCESS)
     */
    NONE
};

// ai specific variables for a m_Player
class cAIPlayer {

private:
    int DELAY_buildbase;    // additional delay when building base
    int TIMER_think;        // timer for thinking itself (calling main routine)

public:

	// SKIRMISH
	bool bPlaying;			// does this AI m_Player play?
	int  iUnits;			// units to start with (min = 1)
    int  iCheckingPlaceStructure; // checking place structure smoothenss (be nice to cpu)

	// ---
	cPlayer *_player;

    int TIMER_BuildUnits;       // when to build units?
    int TIMER_Upgrades;         //
    int TIMER_attack;           // when to attack
    int TIMER_repair;           // repair


    // harvester reinforcements
    int TIMER_blooms;                 // harv
    void init(cPlayer *thePlayer);             // initialize
	
    void think();
    void think_buildarmy();
    void think_buildbase();
    void think_upgrades();
    void think_attack();
    bool think_buildingplacement();
    void think_spiceBlooms();
    void think_skirmishBuildBase();
    
    void think_repair_structure(cAbstractStructure *struc);
    
    void think_repair(); // ai repairing units

	void buildStructureIfAllowed(int iStrucType);
    bool BUILD_UNIT(int iUnitType);
    cantBuildReason canBuildUnit(int iUnitType);

	int  findCellToPlaceStructure(int iStructureType);

    bool isBuildingUnitType(int iUnitType) const;
    cBuildingListItem * isUpgradingList(int listId, int sublistId) const;
    cBuildingListItem * isUpgradingConstyard() const;
    cBuildingListItem * isBuildingStructure() const;
    bool isBuildingStructureAwaitingPlacement() const;


    bool startBuildingUnit(int iUnitType) const;
    void startBuildingStructure(int iStructureType) const;
    void startUpgrading(int iUpgradeType) const;

    bool isUnitAvailableForBuilding(int iUnitType) const;
    cBuildingListItem * isUpgradeAvailableToGrantUnit(int iUnitType) const;

    bool isStructureAvailableForBuilding(int iStructureType) const;
    cBuildingListItem * isUpgradeAvailableToGrantStructure(int iStructureType) const;

    int getStructureTypeBeingBuilt() const;
    cBuildingListItem *getStructureBuildingListItemBeingBuilt() const;

    int findRandomUnitTarget(int playerIndexToAttack);

    int findRandomStructureTarget(int iAttackPlayer);

};

#endif