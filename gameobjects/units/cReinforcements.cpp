#include "d2tmc.h"
#include "cReinforcements.h"
#include "utils/common.h"
#include "player/cPlayer.h"


#include <fmt/core.h>

cReinforcements::cReinforcements() {
    init();
}

void cReinforcements::init() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        sReinforcement &reinforcement = reinforcements[i];
        reinforcement.iCell = -1;
        reinforcement.iPlayer = -1;
        reinforcement.iSeconds = -1;
        reinforcement.iUnitType = -1;
    }
}

// returns next free reinforcement index
int cReinforcements::findNextUnusedId() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        // new one yey
        if (reinforcements[i].iCell < 0)
            return i;
    }

    return -1;
}

// set reinforcement
void cReinforcements::SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType, int iPlyrGetHouse) {
    int iIndex = findNextUnusedId();

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

    logbook(fmt::format("[{}] Reinforcement: Controller = {}, House {}, Time {}, Type = {}",
                        iIndex, iPlyr, iPlyrGetHouse, iTime, iUType));

    sReinforcement &reinforcement = reinforcements[iIndex];

    reinforcement.iCell = iCll;
    reinforcement.iPlayer = iPlyr;
    reinforcement.iUnitType = iUType;
    reinforcement.iSeconds = iTime;
}

void cReinforcements::substractSecondFromValidReinforcements() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        sReinforcement &reinforcement = reinforcements[i];
        // valid reinforcement
        if (reinforcement.iCell > 0 && reinforcement.iSeconds > -1) {
            reinforcement.iSeconds = reinforcement.iSeconds - 1;
        }
    }
}

void cReinforcements::thinkSlow() {
    substractSecondFromValidReinforcements();
    const sReinforcement &reinforcement = getReinforcementAndDestroy();
    REINFORCE(reinforcement);
}

//bool cReinforcements::hasReinforcement() {
//    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
//        sReinforcement &reinforcement = reinforcements[i];
//        if (reinforcement.iCell > 0 && reinforcement.iSeconds < 0) {
//            return true;
//        }
//    }
//    return false;
//}

sReinforcement cReinforcements::getReinforcementAndDestroy() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        sReinforcement &reinforcement = reinforcements[i];

        // valid reinforcement & no need to wait anymore
        if (reinforcement.iCell > 0 && reinforcement.iSeconds < 0) {
            sReinforcement copy = reinforcements[i]; // remember

            // make invalid
            reinforcement.iCell = -1;
            reinforcement.iPlayer = -1;
            reinforcement.iSeconds = -1;
            reinforcement.iUnitType = -1;
            return copy;
        }
    }

    sReinforcement invalid;
    return invalid;
}


/**
 * Reinforce:
 * Assumes this is a 'real' reinforcement. (ie triggered by map)
 * create a new unit by sending it.
 *
 * @param iPlr player index
 * @param iTpe unit type
 * @param iCll location where to bring it
 * @param iStart where to start from

 */
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart) {
    REINFORCE(iPlr, iTpe, iCll, iStart, true);
}

/**
 * Same as above REINFORCE function, but takes sReinforcement. if iCell is < 0 then it will do nothing.
 * @param reinforcement
 */
void REINFORCE(const sReinforcement &reinforcement) {
    if (reinforcement.iCell < 0) return; // bail, invalid reinforcement given

    int focusCell = players[reinforcement.iPlayer].getFocusCell();
    REINFORCE(reinforcement.iPlayer, reinforcement.iUnitType, reinforcement.iCell, focusCell, true);
}

/**
 * Spawns a carryAll that brings a unit (part of a reinforcement) to the map. It has a flag 'reinforcement' so that
 * we can distinguish (for now) between "mission reinforcements" and "carry-all brings harvester by building refinery"
 * use-cases.
 *
 * @param iPlr
 * @param iTpe
 * @param iCll
 * @param iStart
 * @param isReinforcement
 */
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart, bool isReinforcement) {
    // handle invalid arguments
    if (iPlr < 0 || iTpe < 0)
        return;

    if (map.isValidCell(iCll) == false)
        return;

    if (iStart < 0)
        iStart = iCll;

    int iStartCell = iFindCloseBorderCell(iStart);

    if (iStartCell < 0) {
        iStart += rnd(64);
        if (iStart >= map.getMaxCells())
            iStart -= 64;

        iStartCell = iFindCloseBorderCell(iStart);
    }

    if (iStartCell < 0) {
        logbook("ERROR (reinforce): Could not figure a startcell");
        return;
    }

    logbook(fmt::format("REINFORCE: Bringing unit type {} for player {}. Starting from cell {}, going to cell {}",
                        iTpe, iPlr, iStartCell, iCll));

    // STEP 2: create carryall
    int iUnit = UNIT_CREATE(iStartCell, CARRYALL, iPlr, true, isReinforcement);
    if (iUnit < 0) {
        // cannot create carry-all!
        logbook("ERROR (reinforce): Cannot create CARRYALL unit.");
        return;
    }

    // STEP 3: assign order to carryall
    int iCellX = map.getCellX(iStartCell);
    int iCellY = map.getCellY(iStartCell);
    int cx = map.getCellX(iCll);
    int cy = map.getCellY(iCll);

    int d = fDegrees(iCellX, iCellY, cx, cy);
    int f = faceAngle(d); // get the angle

    cUnit &carryall = unit[iUnit];
    carryall.iBodyShouldFace = f;
    carryall.iBodyFacing = f;
    carryall.iHeadShouldFace = f;
    carryall.iHeadFacing = f;

    carryall.carryall_order(-1, eTransferType::NEW_LEAVE, iCll, iTpe);
}