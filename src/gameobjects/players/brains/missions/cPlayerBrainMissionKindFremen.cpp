#include "cPlayerBrainMission.h"
#include "cPlayerBrainMissionKindFremen.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/players/cPlayers.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/cAssert.h"

namespace brains {

cPlayerBrainMissionKindFremen::cPlayerBrainMissionKindFremen(cPlayer *player, cPlayerBrainMission *mission) :  cPlayerBrainMissionKind(player, mission)
{
    d2tm_assert(player != nullptr);
    d2tm_assert(mission != nullptr);
    specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_CREATED; // fremen created
    specificBuildTypeToGoToSelectTargetState = player->getInfos()->getSpecialInfo(SPECIAL_FREMEN).providesType;
    specificBuildIdToGoToSelectTargetState = player->getInfos()->getSpecialInfo(SPECIAL_FREMEN).providesTypeId;
    specificPlayerForEventToGoToSelectTargetState = player->getObjects()->getPlayer(AI_CPU5);
}

cPlayerBrainMissionKindFremen::~cPlayerBrainMissionKindFremen()
{

}

bool cPlayerBrainMissionKindFremen::think_SelectTarget()
{
    return true;
}

void cPlayerBrainMissionKindFremen::think_Execute()
{
    mission->changeState(PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES); // keep awaiting resources
}

void cPlayerBrainMissionKindFremen::onNotifyGameEvent(const s_GameEvent &event)
{
    cPlayerBrainMissionKind::onNotifyGameEvent(event);
}

cPlayerBrainMissionKind *cPlayerBrainMissionKindFremen::clone(cPlayer *player, cPlayerBrainMission *mission)
{
    cPlayerBrainMissionKindFremen *copy = new cPlayerBrainMissionKindFremen(player, mission);
    copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
    copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
    copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
    copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
    return copy;
}

void cPlayerBrainMissionKindFremen::onNotify_SpecificStateSwitch(const BuildingEvent &)
{
    // NOOP
}

}