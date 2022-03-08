#include "cReinforcements.h"
#include "utils/common.h"


#include <fmt/core.h>



///////////////////////////////////////////////////////////////////////
// REINFORCEMENT STUFF
///////////////////////////////////////////////////////////////////////
void cReinforcements::INIT_REINFORCEMENT() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        reinforcements[i].iCell = -1;
        reinforcements[i].iPlayer = -1;
        reinforcements[i].iSeconds = -1;
        reinforcements[i].iUnitType = -1;
    }
}

// returns next free reinforcement index
int cReinforcements::NEXT_REINFORCEMENT() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        // new one yey
        if (reinforcements[i].iCell < 0)
            return i;
    }

    return -1;
}

// set reinforcement
void cReinforcements::SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType) {
    int iIndex = this->NEXT_REINFORCEMENT();

    // do not allow falsy indexes.
    if (iIndex < 0)
        return;

    if (iCll < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid cell given");
        return;
    }

    if (iPlyr < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid plyr");
        return;
    }

    if (iTime < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid time given");
        return;
    }

    if (iUType < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid unit type given");
        return;
    }

    // logbook(fmt::format("[{}] Reinforcement: Controller = {}, House {}, Time {}, Type = {}",
    //                     iIndex, iPlyr, players[iPlyr].getHouse(), iTime, iUType));

    // DEBUG DEBUG
    // if (!game.isDebugMode())
    //     iTime *= 15;

    reinforcements[iIndex].iCell = iCll;
    reinforcements[iIndex].iPlayer = iPlyr;
    reinforcements[iIndex].iUnitType = iUType;
    reinforcements[iIndex].iSeconds = iTime;
}

