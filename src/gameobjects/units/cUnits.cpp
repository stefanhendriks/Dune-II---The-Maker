/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "gameobjects/units/cUnit.h"
#include "gameobjects/units/cUnits.h"
#include "include/d2tmc.h"
#include "game/cGame.h"
#include "utils/cLog.h"
#include "utils/RNG.hpp"
#include <format>

cUnits::cUnits() {
    // Units will be initialized through default constructors of std::array
}

cUnit& cUnits::operator[](int index) {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMEOBJECTS, "cUnits::operator[]", 
            std::format("Invalid unit index: {}", index));
        return m_units[0];  // Return first unit as fallback
    }
    return m_units[index];
}

const cUnit& cUnits::operator[](int index) const {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_GAMEOBJECTS, "cUnits::operator[] const", 
            std::format("Invalid unit index: {}", index));
        return m_units[0];  // Return first unit as fallback
    }
    return m_units[index];
}

cUnit* cUnits::getUnit(int index) {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        return nullptr;
    }
    return &m_units[index];
}

const cUnit* cUnits::getUnit(int index) const {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        return nullptr;
    }
    return &m_units[index];
}

int cUnits::findAvailableSlot() const {
    for (int i = 0; i < MAX_UNITS_CAPACITY; i++) {
        if (!m_units[i].isValid()) {
            return i;
        }
    }
    return -1; // No available slot
}

int cUnits::getValidUnitsCount() const {
    int count = 0;
    for (int i = 0; i < MAX_UNITS_CAPACITY; i++) {
        if (m_units[i].isValid()) {
            count++;
        }
    }
    return count;
}

// return new valid ID
static int unitNewID()
{
    for (int i = 0; i < game.m_Units.size(); i++)
        if (!game.getUnit(i).isValid())
            return i;

    return -1; // NONE
}

// int cUnits::unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart)
// {
//     return unitCreate(iCll, unitType, iPlayer, bOnStart, false);
// }

int cUnits::unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement, float hpPercentage) {
    if (!game.m_map.isValidCell(iCll)) {
        logbook("UNIT_CREATE: Invalid cell as param");
        return -1;
    }

    s_UnitInfo &sUnitType = game.unitInfos[unitType];

    // check if unit already exists on location
    if (!sUnitType.airborn && game.m_map.cellGetIdFromLayer(iCll, MAPID_STRUCTURES) > -1) {
        return -1; // cannot place unit, structure exists at location
    }

    int mapIdIndex = MAPID_UNITS;
    if (sUnitType.airborn) {
        mapIdIndex = MAPID_AIR;
    }
    else if (unitType == SANDWORM) {
        mapIdIndex = MAPID_WORMS;
    }

    // check if unit already exists on location
    if (game.m_map.cellGetIdFromLayer(iCll, mapIdIndex) > -1) {
        return -1; // cannot place unit
    }

    // check if placed on invalid terrain type
    if (unitType == SANDWORM) {
        if (!game.m_map.isCellPassableForWorm(iCll)) {
            return -1;
        }
    }

    bool validCell = game.m_map.canDeployUnitTypeAtCell(iCll, unitType);
    if (!validCell) {
        return -1;
    }

    int iNewId = unitNewID();

    if (iNewId < 0) {
        logbook("UNIT_CREATE:Could not find new unit index");
        return -1;
    }

    cUnit &newUnit = game.getUnit(iNewId);
    newUnit.init(iNewId);

    newUnit.setCell(iCll);
    newUnit.rendering.iBodyFacing = RNG::rnd(8);
    newUnit.rendering.iHeadFacing = RNG::rnd(8);

    newUnit.rendering.iBodyShouldFace = newUnit.rendering.iBodyFacing;
    newUnit.rendering.iHeadShouldFace = newUnit.rendering.iHeadFacing;

    newUnit.iType = unitType;
    newUnit.setHp(sUnitType.hp * hpPercentage);
    newUnit.movement.iGoalCell = iCll;

    newUnit.deselect();
    newUnit.rendering.bHovered = false;

    newUnit.TIMER_bored = RNG::rnd(3000);
    // newUnit.TIMER_guard = 20 + RNG::rnd(70);
    newUnit.guardTimer.reset(20 + RNG::rnd(70));
    newUnit.recreateDimensions();

    // set (Correct!?) player id, when type is SANDWORM (!?)
    if (unitType == SANDWORM) {
        if (iPlayer != AI_WORM) {
            newUnit.log("ERROR: Wanted to create sandworm for player other than AI_WORM!?");
        }
        iPlayer = AI_WORM;
    }

    newUnit.iPlayer = iPlayer;

    // AI player immediately moves unit away
    if (iPlayer > 0 && iPlayer < AI_WORM && !sUnitType.airborn && !bOnStart) {
        int iF = cUnit::freeAroundMove(iNewId);

        if (iF > -1) {
            newUnit.log("Order move #2");
            game.getUnit(iNewId).move_to(iF);
        }
    }

    // Put on map too!:
    game.m_map.cellSetIdForLayer(iCll, mapIdIndex, iNewId);

    newUnit.updateCellXAndY();

    game.m_map.clearShroud(iCll, sUnitType.sight, iPlayer);

    s_GameEvent event {
        .eventType = eGameEventType::GAME_EVENT_CREATED,
        .entityType = eBuildType::UNIT,
        .entityID = iNewId,
        .player = newUnit.getPlayer(),
        .entitySpecificType = unitType,
        .atCell = -1,
        .isReinforce = isReinforcement
    };

    game.onNotifyGameEvent(event);

    return iNewId;
}

/**
 * Creates a new unit, when bOnStart is true, it will prevent AI players from moving a unit immediately a bit.
 *
 *
 * @param iCll
 * @param unitType
 * @param iPlayer
 * @param bOnStart
 * @param isReinforement flag to set on event
 * @return
 */
// int cUnits::unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement)
// {
//     return unitCreate(iCll, unitType, iPlayer, bOnStart, isReinforcement, 1.0f);
// }
