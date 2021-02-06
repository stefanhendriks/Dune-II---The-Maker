/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

// Computer Opponent variables

// helpers
bool canAIBuildUnit(int iPlayer, int iUnitType);
int AI_STRUCTYPE(int iUnitType);
int AI_RANDOM_STRUCTURE_TARGET(int iPlayer, int iAttackPlayer);
int AI_RANDOM_UNIT_TARGET(int iPlayer, int playerIndexToAttack);

int CLOSE_SPICE_BLOOM(int iCell);

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

    int ID; 

    int TIMER_BuildUnits;       // when to build units?
    int TIMER_attack;           // when to attack
    int TIMER_repair;           // repair


    // harvester reinforcements
    int TIMER_blooms;                 // harv
    void init(int iID);             // initialize
	
    void think();
    void think_worm();
    void think_buildarmy();
    void think_buildbase();
    void think_attack();
    bool think_buildingplacement();
    void think_spiceBlooms();
    
    void think_repair_structure(cAbstractStructure *struc);
    
    void think_repair(); // ai repairing units

	void BUILD_STRUCTURE(int iStrucType);
    bool BUILD_UNIT(int iUnitType);

	int  findCellToPlaceStructure(int iStructureType);

    bool isBuildingUnitType(int iUnitType) const;
    cBuildingListItem * isUpgradingList(int listId, int sublistId) const;
    cBuildingListItem * isUpgradingConstyard() const;
    cBuildingListItem * isBuildingStructure() const;
    bool isBuildingStructureAwaitingPlacement() const;

    int getStructureTypeBeingBuilt();

    bool startBuildingUnit(int iUnitType) const;
    void startBuildingStructure(int iStructureType) const;
    void startUpgrading(int iUpgradeType) const;

    bool isStructureAvailableForBuilding(int iStructureType) const;
    cBuildingListItem * isUpgradeAvailableToGrantStructure(int iStructureType) const;

    cBuildingListItem *getStructureBuildingListItemBeingBuilt() const;
};





