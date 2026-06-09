/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "gameobjects/units/cUnit.h"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/map/cMap.h"
#include "utils/common.h"
#include "game/cGameInterface.h"
#include "utils/Log.h"
#include "utils/RNG.hpp"
#include "include/sGameServices.h"
#include "context/GameContext.hpp"
#include <format>
#include "include/cAssert.h"

cUnits::cUnits() {
    // Units will be initialized through default constructors of std::array
}


void cUnits::serviceInit(sGameServices* services)
{
    d2tm_assert(services != nullptr);
    m_infos = services->info;
    d2tm_assert(m_infos != nullptr);
    m_objects = services->objects;
    d2tm_assert(m_objects != nullptr);
    m_interface = services->ctx->getGameInterface();
    d2tm_assert(m_interface != nullptr);
    m_map = m_objects->getMap();
    d2tm_assert(m_map != nullptr);

    d2tm_assert(services != nullptr);
    for (int i = 0; i < MAX_UNITS_CAPACITY; i++) {
        m_units[i].serviceInit(services);
    }
}


cUnit* cUnits::operator[](int index)
{
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        Logger::fatal( eLogComponent::COMP_UNITS , "operator[]", "Unit index {} is out of range", index);
    }
    d2tm_assert(index >= 0 && index < MAX_UNITS_CAPACITY && "Invalid unit index");
    return &m_units[index];
}

const cUnit* cUnits::operator[](int index) const
{
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        Logger::fatal( eLogComponent::COMP_UNITS , "operator[] const", "Unit index {} is out of range", index);
    }
    d2tm_assert(index >= 0 && index < MAX_UNITS_CAPACITY && "Invalid unit index");
    return &m_units[index];
}

cUnit* cUnits::getUnit(int index)
{
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        Logger::fatal( eLogComponent::COMP_UNITS , "getUnit", "Unit index {} is out of range", index);
    }
    d2tm_assert(index >= 0 && index < MAX_UNITS_CAPACITY && "Invalid unit* index");
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        return nullptr;
    }
    return &m_units[index];
}

const cUnit* cUnits::getUnit(int index) const {
    if (index < 0 || index >= MAX_UNITS_CAPACITY) {
        Logger::fatal( eLogComponent::COMP_UNITS , "getUnit const", "Unit index {} is out of range", index);
    }
    d2tm_assert(index >= 0 && index < MAX_UNITS_CAPACITY && "Invalid unit* index");
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
    Logger::warn( eLogComponent::COMP_UNITS , "findAvailableSlot", "No avariable slot");
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

static int unitNewID(cGameObjectContext *objects)
{
    for (int i = 0; i < objects->getUnits()->size(); i++)
        if (!objects->getUnit(i)->isValid())
            return i;

    return -1; // NONE
}

// int cUnits::unitCreate(int iCll, int unitType, int iPlayer, bool bOnStart)
// {
//     return unitCreate(iCll, unitType, iPlayer, bOnStart, false);
// }

int cUnits::unitCreate(cGameObjectContext* objects, cInfoContext* infos, cGameInterface* iface, int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement, float hpPercentage) {
    if (!objects->getMapGeometry()->isValidCell(iCll)) {
        Logger::warn(COMP_UNITS, "cUnits::unitCreate", "Invalid cell as param");
        return -1;
    }

    s_UnitInfo &sUnitType = infos->getUnitInfo(unitType);

    // check if unit already exists on location
    if (!sUnitType.airborn && objects->getMap()->cellGetIdFromLayer(iCll, MAPID_STRUCTURES) > -1) {
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
    if (objects->getMap()->cellGetIdFromLayer(iCll, mapIdIndex) > -1) {
        return -1; // cannot place unit
    }

    // check if placed on invalid terrain type
    if (unitType == SANDWORM) {
        if (!objects->getMap()->isCellPassableForWorm(iCll)) {
            return -1;
        }
    }

    bool validCell = objects->getMap()->canDeployUnitTypeAtCell(iCll, unitType);
    if (!validCell) {
        return -1;
    }

    int iNewId = unitNewID(objects);

    if (iNewId < 0) {
        Logger::warn(COMP_UNITS, "cUnits::unitCreate", "Could not find new unit index");
        return -1;
    }

    cUnit *newUnit = objects->getUnit(iNewId);
    newUnit->init(iNewId);

    newUnit->setCell(iCll);
    newUnit->rendering.iBodyFacing = randomFacing();
    newUnit->rendering.iHeadFacing = randomFacing();

    newUnit->rendering.iBodyShouldFace = newUnit->rendering.iBodyFacing;
    newUnit->rendering.iHeadShouldFace = newUnit->rendering.iHeadFacing;

    newUnit->iType = unitType;
    newUnit->setHp(sUnitType.hp * hpPercentage);
    newUnit->movement.iGoalCell = iCll;

    newUnit->deselect();
    newUnit->rendering.bHovered = false;

    // newUnit->TIMER_bored = RNG::rnd(3000);
    newUnit->boredTimer.reset(RNG::rnd(3000));

    // newUnit->TIMER_guard = 20 + RNG::rnd(70);
    newUnit->guardTimer.reset(20 + RNG::rnd(70));
    newUnit->recreateDimensions();

    // Sandworm units are always owned by AI_WORM. INI files typically specify "Fremen"
    // as the house (player 5), but sandworms must be owned by AI_WORM (player 6).
    if (unitType == SANDWORM) {
        iPlayer = AI_WORM;
    }

    newUnit->iPlayer = iPlayer;

    // AI player immediately moves unit away
    if (iPlayer > 0 && iPlayer < AI_WORM && !sUnitType.airborn && !bOnStart) {
        int iF = cUnit::freeAroundMove(iNewId, objects);

        if (iF > -1) {
            newUnit->log("Order move #2");
            objects->getUnit(iNewId)->move_to(iF);
        }
    }

    // Put on map too!:
    objects->getMap()->cellSetIdForLayer(iCll, mapIdIndex, iNewId);

    newUnit->updateCellXAndY();

    objects->getMap()->clearShroud(iCll, sUnitType.sight, iPlayer);

    const s_GameEvent event {
        .eventType = eGameEventType::GAME_EVENT_CREATED,
        .data = CommonEvent {
            .entityType = eBuildType::UNIT,
            .entityID = iNewId,
            .player = newUnit->getPlayer(),
            .entitySpecificType = unitType,
            .isReinforce = isReinforcement
        }
    };
    iface->onNotifyGameEvent(event);

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

void cUnits::move_to(int icell)
{
    // Implementation for moving units to a specific cell
    for (auto& unit : m_units) {
        if (unit.isValid() && unit.isSelected()) {
            unit.move_to(icell);
        }
    }
}