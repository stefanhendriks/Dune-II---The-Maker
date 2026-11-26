/**
 * @file cPlayerBrain.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cPlayerBrain.h"
#include "context/cGameObjectContext.h"
#include "context/cInfoContext.h"
#include "gameobjects/players/cPlayer.h"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "include/sGameServices.h"
#include "include/cAssert.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/units/cUnit.h"

#include <algorithm>
#include <cstdlib>

namespace brains {

cPlayerBrain::~cPlayerBrain() = default;

cPlayerBrain::cPlayerBrain(cPlayer *player) : player(player)
{
    d2tm_assert(player != nullptr);
    // brains created in cGame::init() come before the player has its infos; they are replaced before a game starts
    if (player->getInfos() != nullptr) {
        m_aiParameters = player->getInfos()->getAiParameters();
    }
}

void cPlayerBrain::serviceInit(sGameServices* services)
{
    m_objects = services->objects;
    m_info = services->info;
    m_settings = services->settings;
    m_eventEmitter = services->eventEmitter;
}

bool cPlayerBrain::recallForRepairWhenBadlyDamaged(cUnit *unit)
{
    if (!unit->isValid() || !unit->isEligibleForRepair()) return false;
    if (unit->isHeadingForRepair()) return false;

    const s_AiSharedParameters &parameters = m_aiParameters.shared;
    const double health = unit->getHealthNormalized();
    // cheap checks first, looking for friendly units near this one is not
    if (health >= std::max(parameters.repairRecallAmbushedBelowHpFraction, parameters.repairRecallSiegingBelowHpFraction)) return false;
    if (!player->hasAtleastOneStructure(REPAIR)) return false;
    if (health >= parameters.repairRecallBelowHpFraction(hasFriendlyUnitFightingNear(unit))) return false;

    const eHeadTowardsStructureResult result = unit->findBestStructureCandidateAndHeadTowardsItOrWait(REPAIR, true, INTENT_REPAIR);
    return result == eHeadTowardsStructureResult::SUCCESS_RETURNING ||
           result == eHeadTowardsStructureResult::SUCCESS_AWAITING_CARRYALL;
}

bool cPlayerBrain::hasFriendlyUnitFightingNear(cUnit *unit) const
{
    const MapGeometry *geometry = m_objects->getMapGeometry();
    const int radius = m_aiParameters.shared.repairRecallSiegeRadius;
    const int unitX = geometry->getCellX(unit->getCell());
    const int unitY = geometry->getCellY(unit->getCell());
    for (int i = 0; i < m_objects->getUnitsSize(); i++) {
        cUnit *other = m_objects->getUnit(i);
        if (other == unit) continue;
        if (!other->isAlive()) continue;
        if (!other->belongsTo(player)) continue;
        if (other->isAirbornUnit()) continue;
        if (!other->isAttacking()) continue;
        if (std::abs(geometry->getCellX(other->getCell()) - unitX) > radius) continue;
        if (std::abs(geometry->getCellY(other->getCell()) - unitY) > radius) continue;
        return true;
    }
    return false;
}

}