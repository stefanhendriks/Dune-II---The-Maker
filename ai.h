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

// ai specific variables for a player
class cAIPlayer {

private:
    int DELAY_buildbase;    // additional delay when building base
    int TIMER_think;        // timer for thinking itself (calling main routine)

public:

	// SKIRMISH
	bool bPlaying;			// does this AI player play?
	int  iUnits;			// units to start with (min = 1)
    int  iCheckingPlaceStructure; // checking place structure smoothenss (be nice to cpu)

	// ---
	cPlayer *player;

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

};

#endif