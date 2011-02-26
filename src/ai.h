/* 

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

// Computer Opponent variables

// helpers
bool AI_UNITSTRUCTURETYPE(int iPlayer, int iUnitType);
int AI_STRUCTYPE(int iUnitType);
int AI_RANDOM_STRUCTURE_TARGET(int iPlayer, int iAttackPlayer);
int AI_RANDOM_UNIT_TARGET(int iPlayer, int iAttackPlayer);

// ai specific variables for a player
class cAIPlayer {
	public:

		// SKIRMISH
		bool bPlaying; // does this AI player play?
		int iUnits; // units to start with (min = 1)
		int iCheckingPlaceStructure; // checking place structure smoothenss (be nice to cpu)


		// ---

		int ID;

		int iBuildingUnit[MAX_UNITTYPES]; // > -1 = progress
		int iBuildingStructure[MAX_STRUCTURETYPES]; // > -1 = progress
		int TIMER_BuildUnit[MAX_UNITTYPES]; //
		int TIMER_BuildStructure[MAX_STRUCTURETYPES]; // ONLY ONE BUILDING AT A TIME CAN BE BUILT!

		// building stuff
		int TIMER_BuildUnits; // when to build units?

		// Attacking
		int TIMER_attack; // when to attack
		int TIMER_repair; // repair


		// harvester reinforcements
		int TIMER_harv; // harv
		void init(int iID); // initialize

		void think();
		void think_worm();
		void think_buildarmy();
		void think_buildbase();
		void think_attack();
		void think_building();
		void think_harvester();

		void think_repair_structure(cAbstractStructure *struc);

		void think_repair(); // ai repairing units

		void BUILD_STRUCTURE(int iStrucType);
		void BUILD_UNIT(int iUnitType);

		int iPlaceStructureCell(int iType);
};

