/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "..\d2tmh.h"

cPlayer::cPlayer() {
	itemBuilder = NULL;
}

cPlayer::~cPlayer() {
	if (itemBuilder) {
		delete itemBuilder;
	}
}

void cPlayer::setSideBar(cSideBar *theSideBar) {
	assert(theSideBar);

	// delete old reference
	if (sidebar) {
		delete sidebar;
	}

	sidebar = theSideBar;
}

void cPlayer::setItemBuilder(cItemBuilder *theItemBuilder) {
	assert(theItemBuilder);

	// delete old reference
	if (itemBuilder) {
		delete itemBuilder;
	}

	itemBuilder = theItemBuilder;
}

void cPlayer::setBuildingListUpgrader(cBuildingListUpdater *theBuildingListUpgrader) {
	assert(theBuildingListUpgrader);

	// delete old reference
	if (buildingListUpgrader) {
		delete buildingListUpgrader;
	}

	buildingListUpgrader = theBuildingListUpgrader;
}

void cPlayer::init()
{
	memcpy(pal, general_palette, sizeof(pal));
	house = GENERALHOUSE;

	difficultySettings = new cPlayerAtreidesDifficultySettings();

	// Reset structures amount
	memset(iStructures, 0, sizeof(iStructures));
	memset(iPrimaryBuilding, -1, sizeof(iPrimaryBuilding));

	credits	=	0;
	max_credits	=	credits;
	focus_cell	=	0;

	use_power=0;
	has_power=0;

    iTeam=-1;

    TIMER_think=rnd(10);        // timer for thinking itself (calling main routine)
    TIMER_attack=-1;			// -1 = determine if its ok to attack, > 0 is , decrease timer, 0 = attack

}


// set house
void cPlayer::setHouse(int iHouse)
{

  house = iHouse;      // use rules of this house

  // copy entire palette
  memcpy (pal, general_palette, sizeof(pal));

  // now set the different colors based uppon house
  if (houses[house].swap_color > -1) {
    int start = houses[house].swap_color;
    int s=144;                // original position (harkonnen)
    for (int j = start; j < (start+7);j++) {
      // swap everything from S with J
       pal[s] = pal[j];
       s++;
     }
  }

}

bool cPlayer::bEnoughPower()
{
    if (game.bSkirmish) {
       if (has_power >= use_power) return true;
    } else {
        int iMyID=-1;

		for (int i=0; i < MAX_PLAYERS; i++) {
            if (&player[i] == this)
            {
                iMyID=i;
                break;
            }
		}

        // AI cheats on power
        if (iMyID > 0) {
            // original dune 2 , the AI cheats. Unfortunatly D2TM has to cheat too, else the game will
            // be unplayable.
            if (iStructures[WINDTRAP] > 0) {
                // always enough power so it seems
                return true;
			} else {
                return false; // not enough power
			}
        }
        else
        {
            if (has_power >= use_power)
                return true;
        }
    }


    // return false on any other case
    return false;
}


/**
	Return the damage done by house type:
	Atreides == 100% (normal)
	Harkonnen = stronger
	Sardaukar = a bit stronger then Harkonnen
	Fremen == Sardaukar
	Ordos = weaker

	How much is determined here.

**/
#ifdef IGNOREMEFORNOW
int cPlayer::iDamage(int iDamage)
{
	float fOriginal = 1;

	if (house == HARKONNEN) fOriginal = 1.2;
	if (house == SARDAUKAR) fOriginal = 1.25;
	if (house == ORDOS || house == FREMEN) fOriginal = 0.8;

	float fDamage = iDamage;					// make float for calculation
	return (int)(fOriginal*fDamage);			// * range , convert back to int and return
}

// return proper speeds
int cPlayer::iMoveSpeed(int iTpe)
{
	// return proper move speed of unit type
	float fOriginal = 100;

	if (house == HARKONNEN || house == SARDAUKAR)
		fOriginal = 120;

	if (house == ORDOS || house == FREMEN)
		fOriginal = 80;

	if (fOriginal <= 1.0) fOriginal = 1.0f;			// fix it
	float fSpeed = fOriginal/100;					// Divide by 100 (so we get in 0-1 range)
	return (int)(units[iTpe].speed*fSpeed);			// * original
}


// return proper speeds
int cPlayer::iBuildSpeed(int iSpeed)
{
	// return proper move speed of unit type
	float fOriginal = 100;

	if (house == HARKONNEN || house == SARDAUKAR)
		fOriginal = 120;

	if (house == ORDOS || house == FREMEN)
		fOriginal = 80;

	if (fOriginal <= 1.0) fOriginal = 1.0f;			// fix it
	float fSpeed = fOriginal/100;					// devide by 100 (so we get in 0-1 range)
	return (int)(iSpeed*fSpeed);					// * original
}

// return proper speeds
int cPlayer::iHarvestSpeed(int iSpeed)
{
    if (DEBUGGING)
        return 0;

	// return proper move speed of unit type
	float fOriginal = 100;

	if (house == HARKONNEN || house == SARDAUKAR)
		fOriginal = 120;

	if (house == ORDOS || house == FREMEN)
		fOriginal = 80;

	if (fOriginal <= 1.0) fOriginal = 1.0f;			// fix it
	float fSpeed = fOriginal/100;					// devide by 100 (so we get in 0-1 range)
	return (int)(iSpeed*fSpeed);					// * original
}

int cPlayer::iDumpSpeed(int iSpeed)
{
	// return proper move speed of unit type
	float fOriginal = 100;

	if (house == HARKONNEN || house == SARDAUKAR)
		fOriginal = 120;

	if (house == ORDOS || house == FREMEN)
		fOriginal = 80;

	if (fOriginal <= 1.0) fOriginal = 1.0f;			// fix it
	float fSpeed = fOriginal/100;					// devide by 100 (so we get in 0-1 range)
	return (int)(iSpeed*fSpeed);					// * original
}


#endif
