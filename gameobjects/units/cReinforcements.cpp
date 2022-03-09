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

    reinforcements[iIndex].iCell = iCll;
    reinforcements[iIndex].iPlayer = iPlyr;
    reinforcements[iIndex].iUnitType = iUType;
    reinforcements[iIndex].iSeconds = iTime;
}

void cReinforcements::spendASecond()
{
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        if (reinforcements[i].iCell > 0)
            reinforcements[i].iSeconds = reinforcements[i].iSeconds-1;            
    } 
}

bool cReinforcements::hasReinforcement()
{
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        if (reinforcements[i].iCell > 0 && reinforcements[i].iSeconds < 0)
            return true;            
    }
    return false;
}

void cReinforcements::getReinforcementAndDestroy(int &iCll, int &iPlyr, int &iTime, int &iUType)
{
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        if (reinforcements[i].iCell > 0 && reinforcements[i].iSeconds < 0) {
            iCll = reinforcements[i].iCell;
            iPlyr = reinforcements[i].iPlayer;
            iUType = reinforcements[i].iUnitType;
            iTime = reinforcements[i].iSeconds;
            // we destroy it because it's be use.
            reinforcements[i].iCell = -1;
            reinforcements[i].iPlayer = -1;
            reinforcements[i].iSeconds = -1;
            reinforcements[i].iUnitType = -1;
        }
        return ;            
    }
}
