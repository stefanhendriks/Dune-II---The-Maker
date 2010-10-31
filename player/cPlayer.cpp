/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

#include "../d2tmh.h"

cPlayer::cPlayer() {
	itemBuilder = NULL;
	orderProcesser = NULL;
	sidebar = NULL;
	structurePlacer = NULL;
	upgradeBuilder = NULL;
	buildingListUpdater = NULL;
	gameControlsContext = NULL;
}

cPlayer::~cPlayer() {
	if (itemBuilder) {
		delete itemBuilder;
	}
	if (orderProcesser) {
		delete orderProcesser;
	}
	if (sidebar) {
		delete sidebar;
	}
	if (structurePlacer) {
		delete structurePlacer;
	}
	if (upgradeBuilder) {
		delete upgradeBuilder;
	}
	if (buildingListUpdater) {
		delete buildingListUpdater;
	}
	if (gameControlsContext) {
		delete gameControlsContext;
	}
}

void cPlayer::setId(int theId) {
	assert(theId >= HUMAN);
	assert(theId <= MAX_PLAYERS);
	assert(&player[theId] == this); // check if the reference in the array is the same!
	id = theId;
}

void cPlayer::setUpgradeBuilder(cUpgradeBuilder *theUpgradeBuilder) {
	assert(theUpgradeBuilder);

	// delete old reference
	if (upgradeBuilder) {
		delete upgradeBuilder;
	}

	upgradeBuilder = theUpgradeBuilder;
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

void cPlayer::setOrderProcesser(cOrderProcesser * theOrderProcesser) {
	assert(theOrderProcesser);

	if (orderProcesser) {
		delete orderProcesser;
	}

	orderProcesser = theOrderProcesser;
}

void cPlayer::setStructurePlacer(cStructurePlacer * theStructurePlacer) {
	assert(theStructurePlacer);

	if (structurePlacer) {
		delete structurePlacer;
	}

	structurePlacer = theStructurePlacer;
}

void cPlayer::setBuildingListUpdater(cBuildingListUpdater *theBuildingListUpgrader) {
	assert(theBuildingListUpgrader);

	// delete old reference
	if (buildingListUpdater) {
		delete buildingListUpdater;
	}

	buildingListUpdater = theBuildingListUpgrader;
}

void cPlayer::setGameControlsContext(cGameControlsContext *theGameControlsContext) {
	assert(theGameControlsContext);

	// delete old reference
	if (gameControlsContext) {
		delete gameControlsContext;
	}

	gameControlsContext = theGameControlsContext;
}

void cPlayer::init()
{
	memcpy(pal, general_palette, sizeof(pal));
	house = GENERALHOUSE;

	difficultySettings = new cPlayerAtreidesDifficultySettings();

	// Reset structures amount
	for (int i = 0 ; i < MAX_STRUCTURETYPES; i++) {
		iStructures[i] = 0;
		iPrimaryBuilding[i] = -1;
	}

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
void cPlayer::setHouse(int iHouse) {
  house = iHouse;      // use rules of this house

  if (difficultySettings) {
	  delete difficultySettings;
  }

  if (iHouse == ATREIDES) {
	  difficultySettings = new cPlayerAtreidesDifficultySettings();
  } else if (iHouse == ORDOS) {
	  difficultySettings = new cPlayerOrdosDifficultySettings();
  } else if (iHouse == HARKONNEN) {
	  difficultySettings = new cPlayerHarkonnenDifficultySettings();
  } else {
	  // for now default is atreides
	  // TODO: create for other houses difficultysettings
	  difficultySettings = new cPlayerAtreidesDifficultySettings();
  }

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

  minimapColor = getRGBColorForHouse(house);
}

int cPlayer::getRGBColorForHouse(int houseId) {
	switch(houseId) {
		case ATREIDES:
			return makecol(0, 0, 255);
		case HARKONNEN:
			return makecol(255, 0, 0);
		case ORDOS:
			return makecol(0, 255, 0);
		case SARDAUKAR:
			return makecol(255, 0, 255);
		default:
			return makecol(100, 255, 100);
	}
}

bool cPlayer::bEnoughPower() {

	if (game.bSkirmish) {
       return has_power >= use_power;
    } else {
        // AI cheats on power
        if (id > 0) {
            // original dune 2 , the AI cheats.
        	// Unfortunatly D2TM has to cheat too, else the game will
            // be unplayable.
            if (iStructures[WINDTRAP] > 0) {
                // always enough power so it seems
                return true;
			} else {
                return false; // not enough power
			}
        } else {
            return has_power >= use_power;
        }
    }


    // return false on any other case
    return false;
}

int cPlayer::getAmountOfStructuresForType(int structureType) {
	assert(structureType >= 0);
	assert(structureType <= RTURRET);
	return iStructures[structureType];
}
