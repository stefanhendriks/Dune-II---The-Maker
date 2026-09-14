/**
 * @file cPlayerBrainMissionKindExplore.cpp
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

#include "cPlayerBrainMission.h"
#include "cPlayerBrainMissionKindExplore.h"
#include "gameobjects/units/cUnits.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/players/cPlayer.h"
#include "include/cAssert.h"
#include <format>

#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

namespace brains {

cPlayerBrainMissionKindExplore::cPlayerBrainMissionKindExplore(cPlayer *player, cPlayerBrainMission *mission) :  cPlayerBrainMissionKind(player, mission)
{
    d2tm_assert(player != nullptr);
    d2tm_assert(mission != nullptr);
    targetCell = -1;
}

cPlayerBrainMissionKindExplore::~cPlayerBrainMissionKindExplore()
{

}

bool cPlayerBrainMissionKindExplore::think_SelectTarget()
{
    targetCell = player->getObjects()->getMapGeometry()->getRandomCellWithinMapWithSafeDistanceFromBorder(2);
    return true;
}

void cPlayerBrainMissionKindExplore::think_Execute()
{
    const std::vector<int> &units = mission->getUnits();
    for (auto &myUnit : units) {
        cUnit *aUnit = player->getObjects()->getUnit(myUnit);
        if (aUnit->isValid()) {
            if (aUnit->isIdle()) {
                if (player->getObjects()->getMapGeometry()->distance(aUnit->getCell(), targetCell) < 4) {
                    targetCell = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET); // select new target
                }
                else {
                    aUnit->move_to(targetCell);
                }
            }
            else {
                if (player->getObjects()->getMapGeometry()->distance(aUnit->getCell(), targetCell) < 2) {
                    // almost there. Select new target.
                    targetCell = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET); // select new target
                }
            }
        }
    }
}

void cPlayerBrainMissionKindExplore::onNotifyGameEvent(const s_GameEvent &event)
{
    cPlayerBrainMissionKind::onNotifyGameEvent(event);
    log(std::format("cPlayerBrainMissionKindExplore::onNotifyGameEvent() -> {}", event.toString(event.eventType)).c_str());
}

cPlayerBrainMissionKind *cPlayerBrainMissionKindExplore::clone(cPlayer *player, cPlayerBrainMission *mission)
{
    cPlayerBrainMissionKindExplore *copy = new cPlayerBrainMissionKindExplore(player, mission);
    copy->targetCell = targetCell;
    copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
    copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
    copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
    copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
    return copy;
}

void cPlayerBrainMissionKindExplore::onNotify_SpecificStateSwitch(const BuildingEvent &)
{
    // NOOP
}

}