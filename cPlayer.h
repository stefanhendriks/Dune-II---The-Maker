/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */


class cPlayer
{

public:

	PALETTE pal;		// each player has its own palette

	void set_house(int iHouse);
	void init();

	// player specific speeds/settings
	int iMoveSpeed(int iTpe);
	int iBuildSpeed(int iSpeed);
	int iHarvestSpeed(int iSpeed);
	int iDumpSpeed(int iSpeed);
    int iDamage(int iDamage);

    // Score stuff
    long lHarvested;    // how much harvested in total this mission
    int iKills[2];        // Kills (0 units, 1 = struc)
    int iLost[2];         // Casualties

    int iTeam;               // what team are we on?

	int use_power;
	int has_power;

	float credits;		// the credits this player has
	float max_credits;	// max to-be-stored credits of this player
	int focus_cell;
	int house;			// house

	int iPrimaryBuilding[MAX_STRUCTURETYPES];	// [] = Type, holds Structure ID (of structure class)
	int iStructures[MAX_STRUCTURETYPES];		// Hold structure types...

	bool bEnoughPower();

    int TIMER_think;        // timer for thinking itself (calling main routine)
    int TIMER_attack;       // -1 = determine if its ok to attack, > 0 is , decrease timer, 0 = attack


private:



};

