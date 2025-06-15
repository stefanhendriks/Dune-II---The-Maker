#include "d2tmc.h"
#include "cReinforcements.h"
#include "utils/common.h"
#include "player/cPlayer.h"
#include <algorithm>

#include <fmt/core.h>


/// Reinforcement class
cReinforcement::cReinforcement(int delayInSeconds, int unitType, int playerId, int targetCell, bool repeat) :
        m_delayInSeconds(delayInSeconds),
        m_unitType(unitType),
        m_playerId(playerId),
        m_cell(targetCell),
        m_repeat(repeat),
        m_removeMe(false),
        m_originalDelay(delayInSeconds)
{

}

cReinforcement::cReinforcement() :
        m_delayInSeconds(-1),
        m_unitType(-1),
        m_playerId(-1),
        m_cell(-1),
        m_removeMe(false)
{
    // default constructor creates invalid reinforcement
}

void cReinforcement::substractSecondIfApplicable() {
    if (m_delayInSeconds > -1) {
        m_delayInSeconds--;
    }
}

bool cReinforcement::isValid() const {
    return m_cell > 0;
}

bool cReinforcement::canBeRemoved() const {
    return m_removeMe;
}

void cReinforcement::repeatOrMarkForDeletion() {
    if (m_repeat) {
        m_delayInSeconds = m_originalDelay;
    } else {
        m_removeMe = true;
    }
}

bool cReinforcement::isReady() const {
    return m_delayInSeconds < 0;
}

void cReinforcement::execute() const {
    int focusCell = players[m_playerId].getFocusCell();
    REINFORCE(m_playerId, m_unitType, m_cell, focusCell, true);
}

/// Reinforcements container class


cReinforcements::cReinforcements() {
    init();
}

void cReinforcements::init() {
    reinforcements.clear();
}

void cReinforcements::addReinforcement(int playerId, int unitType, int targetCell, int delayInSeconds, bool repeat) {
    logbook(fmt::format("Add reinforcement: PlayerId = {}, DelayInSeconds {}, UnitType = {}, Repeat = {}", playerId, delayInSeconds, unitType, repeat));

    cReinforcement reinforcement(delayInSeconds, unitType, playerId, targetCell, repeat);
    reinforcements.push_back(reinforcement);
}

void cReinforcements::substractSecondFromValidReinforcements() {
    for (auto & reinforcement : reinforcements) {
        reinforcement.substractSecondIfApplicable();
    }
}

void cReinforcements::thinkSlow() {
    substractSecondFromValidReinforcements();
    for (auto & reinforcement : reinforcements) {
        if (reinforcement.isReady()) {
            reinforcement.repeatOrMarkForDeletion();
            REINFORCE(reinforcement);
        }
    }

    // delete invalid reinforcements
    reinforcements.erase(
            std::remove_if(
                    reinforcements.begin(),
                    reinforcements.end(),
                    [](cReinforcement r) { return r.canBeRemoved(); }),
            reinforcements.end()
    );
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
void REINFORCE(const cReinforcement &reinforcement) {
    if (!reinforcement.isValid()) return; // bail, invalid reinforcement given
    reinforcement.execute();
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