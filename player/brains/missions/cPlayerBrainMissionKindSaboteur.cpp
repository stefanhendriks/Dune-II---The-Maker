#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindSaboteur.h"

namespace brains {

    cPlayerBrainMissionKindSaboteur::cPlayerBrainMissionKindSaboteur(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        targetStructureID = -1;
        specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_CREATED; // saboteur created
        specificBuildTypeToGoToSelectTargetState = sSpecialInfo[SPECIAL_SABOTEUR].providesType;
        specificBuildIdToGoToSelectTargetState = sSpecialInfo[SPECIAL_SABOTEUR].providesTypeId;
    }

    cPlayerBrainMissionKindSaboteur::~cPlayerBrainMissionKindSaboteur() {

    }

    bool cPlayerBrainMissionKindSaboteur::think_SelectTarget() {
        // only select a structure

        // TODO: based on priority?
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *theStructure = structure[i];
            if (!theStructure) continue;
            if (!theStructure->isValid()) continue;
            if (theStructure->getPlayer() == player) continue; // skip self
            if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies
            if (!map.isStructureVisible(theStructure, player)) continue; // skip non-visible targets

            // enemy structure
            targetStructureID = theStructure->getStructureId();
            if (rnd(100) < 25) {
                break; // this way we kind of have randomly another target...
            }
        }
        return targetStructureID > -1;
    }

    void cPlayerBrainMissionKindSaboteur::think_Execute() {
        if (targetStructureID < 0) {
            // this should not happen!
            assert(false && "Cannot run think_Execute() code without targetStructureID!");
        }

        if (targetStructureID > -1) {
            cAbstractStructure *pStructure = structure[targetStructureID];
            assert(pStructure != nullptr);
            if (pStructure->isValid()) {
                const std::vector<int> &units = mission->getUnits();
                for (auto &myUnit: units) {
                    cUnit &aUnit = unit[myUnit];
                    if (aUnit.isValid() && aUnit.isIdle()) {
                        log("cPlayerBrainMissionKindSaboteur::thinkState_Execute(): Ordering unit to attack!");
                        aUnit.attackStructure(targetStructureID);
                    }
                }
            } else {
                targetStructureID = -1;
            }
        }
    }

    void cPlayerBrainMissionKindSaboteur::onNotifyGameEvent(const s_GameEvent &event) {
        cPlayerBrainMissionKind::onNotifyGameEvent(event);

        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindSaboteur::onNotifyGameEvent() -> %s", event.toString(event.eventType));
        log(msg);

        if (event.eventType == GAME_EVENT_DESTROYED) {
          onEventDestroyed(event);
        }
    }

    void cPlayerBrainMissionKindSaboteur::onEventDestroyed(const s_GameEvent &event) {
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

    cPlayerBrainMissionKind *cPlayerBrainMissionKindSaboteur::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindSaboteur *copy = new cPlayerBrainMissionKindSaboteur(player, mission);
        copy->targetStructureID = targetStructureID;
        copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
        copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
        copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
        copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
        return copy;
    }

    void cPlayerBrainMissionKindSaboteur::onNotify_SpecificStateSwitch(const s_GameEvent &) {
        // NOOP
    }

}