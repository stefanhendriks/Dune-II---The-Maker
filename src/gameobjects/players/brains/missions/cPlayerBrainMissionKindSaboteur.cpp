#include "cPlayerBrainMission.h"
#include "cPlayerBrainMissionKindSaboteur.h"
// #include "gameobjects/particles/cParticles.h"
#include "gameobjects/structures/cStructures.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/units/cUnits.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/RNG.hpp"
#include <format>
#include "include/cAssert.h"

namespace brains {

cPlayerBrainMissionKindSaboteur::cPlayerBrainMissionKindSaboteur(cPlayer *player, cPlayerBrainMission *mission) :  cPlayerBrainMissionKind(player, mission)
{
    d2tm_assert(player != nullptr);
    d2tm_assert(mission != nullptr);
    targetStructureID = -1;
    specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_CREATED; // saboteur created
    specificBuildTypeToGoToSelectTargetState = player->getInfos()->getSpecialInfo(SPECIAL_SABOTEUR).providesType;
    specificBuildIdToGoToSelectTargetState = player->getInfos()->getSpecialInfo(SPECIAL_SABOTEUR).providesTypeId;
}

cPlayerBrainMissionKindSaboteur::~cPlayerBrainMissionKindSaboteur()
{

}

bool cPlayerBrainMissionKindSaboteur::think_SelectTarget()
{
    // only select a structure

    // TODO: based on priority?
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *theStructure = player->getObjects()->getStructures()[i];
        if (!theStructure) continue;
        if (!theStructure->isValid()) continue;
        if (theStructure->getPlayer() == player) continue; // skip self
        if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies
        if (!player->getObjects()->getMap()->isStructureVisible(theStructure, player)) continue; // skip non-visible targets

        // enemy structure
        targetStructureID = theStructure->getStructureId();
        if (RNG::rnd(100) < 25) {
            break; // this way we kind of have randomly another target...
        }
    }
    return targetStructureID > -1;
}

void cPlayerBrainMissionKindSaboteur::think_Execute()
{
    if (targetStructureID < 0) {
        // this should not happen!
        d2tm_assert(false && "Cannot run think_Execute() code without targetStructureID!");
    }

    if (targetStructureID > -1) {
        cAbstractStructure *pStructure = player->getObjects()->getStructures()[targetStructureID];
        d2tm_assert(pStructure != nullptr);
        if (pStructure->isValid()) {
            const std::vector<int> &units = mission->getUnits();
            for (auto &myUnit: units) {
                cUnit *aUnit = player->getObjects()->getUnit(myUnit);
                if (aUnit->isValid() && aUnit->isIdle()) {
                    log("cPlayerBrainMissionKindSaboteur::thinkState_Execute(): Ordering unit to attack!");
                    aUnit->attackStructure(targetStructureID);
                }
            }
        }
        else {
            targetStructureID = -1;
        }
    }
}

void cPlayerBrainMissionKindSaboteur::onNotifyGameEvent(const s_GameEvent &event)
{
    cPlayerBrainMissionKind::onNotifyGameEvent(event);

    log(std::format("cPlayerBrainMissionKindSaboteur::onNotifyGameEvent() -> {}", event.toString(event.eventType)).c_str());

    if (event.eventType == GAME_EVENT_DESTROYED) {
        if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
            onEventDestroyed(*commonEvent);
        }
    }
}

void cPlayerBrainMissionKindSaboteur::onEventDestroyed(const CommonEvent &event)
{
    if (event.entityType == STRUCTURE) {
        if (event.player != player) {
            // our target got destroyed
            if (targetStructureID == event.entityID) {
                targetStructureID = -1;
                mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
            }
        }
    }
}

cPlayerBrainMissionKind *cPlayerBrainMissionKindSaboteur::clone(cPlayer *player, cPlayerBrainMission *mission)
{
    cPlayerBrainMissionKindSaboteur *copy = new cPlayerBrainMissionKindSaboteur(player, mission);
    copy->targetStructureID = targetStructureID;
    copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
    copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
    copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
    copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
    return copy;
}

void cPlayerBrainMissionKindSaboteur::onNotify_SpecificStateSwitch(const BuildingEvent &)
{
    // NOOP
}

}