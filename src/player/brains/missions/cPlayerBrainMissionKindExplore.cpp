#include "cPlayerBrainMission.h"
#include "cPlayerBrainMissionKindExplore.h"
#include "gameobjects/units/cUnits.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "map/cMap.h"
#include <cassert>
#include <format>

#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

namespace brains {

cPlayerBrainMissionKindExplore::cPlayerBrainMissionKindExplore(cPlayer *player, cPlayerBrainMission *mission) :  cPlayerBrainMissionKind(player, mission)
{
    assert(player != nullptr);
    assert(mission != nullptr);
    targetCell = -1;
}

cPlayerBrainMissionKindExplore::~cPlayerBrainMissionKindExplore()
{

}

bool cPlayerBrainMissionKindExplore::think_SelectTarget()
{
    targetCell = game.m_gameObjectsContext->getMapGeometry()->getRandomCellWithinMapWithSafeDistanceFromBorder(2);
    return true;
}

void cPlayerBrainMissionKindExplore::think_Execute()
{
    const std::vector<int> &units = mission->getUnits();
    for (auto &myUnit : units) {
        cUnit *aUnit = game.m_gameObjectsContext->getUnit(myUnit);
        if (aUnit->isValid()) {
            if (aUnit->isIdle()) {
                if (game.m_gameObjectsContext->getMapGeometry()->distance(aUnit->getCell(), targetCell) < 4) {
                    targetCell = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET); // select new target
                }
                else {
                    aUnit->move_to(targetCell);
                }
            }
            else {
                if (game.m_gameObjectsContext->getMapGeometry()->distance(aUnit->getCell(), targetCell) < 2) {
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