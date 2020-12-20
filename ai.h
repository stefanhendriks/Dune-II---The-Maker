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

// ai specific variables for a m_Player
class cAIPlayer {

public:

	// SKIRMISH
	bool bPlaying;			// does this AI m_Player play?
	int  iUnits;			// units to start with (min = 1)
    int  iCheckingPlaceStructure; // checking place structure smoothenss (be nice to cpu)
    

	// --- 

    int ID; 

    int iBuildingUnit[MAX_UNITTYPES];           // > -1 = progress
    int iBuildingStructure[MAX_STRUCTURETYPES]; // > -1 = progress
    int TIMER_BuildUnit[MAX_UNITTYPES];         // 
    int TIMER_BuildStructure[MAX_STRUCTURETYPES]; // ONLY ONE BUILDING AT A TIME CAN BE BUILT!
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
    void think_building();
    void think_spiceBlooms();
    
    void think_repair_structure(cAbstractStructure *struc);
    
    void think_repair(); // ai repairing units

	void BUILD_STRUCTURE(int iStrucType);
    void BUILD_UNIT(int iUnitType);

	int  findCellToPlaceStructure(int iType);

    bool isBuildingUnitType(int iUnitType) const;
};





