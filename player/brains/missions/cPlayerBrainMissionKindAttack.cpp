#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindAttack.h"

namespace brains {

    cPlayerBrainMissionKindAttack::cPlayerBrainMissionKindAttack(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        targetStructureID = -1;
        targetUnitID = -1;
    }

    cPlayerBrainMissionKindAttack::~cPlayerBrainMissionKindAttack() {

    }

    bool cPlayerBrainMissionKindAttack::think_SelectTarget() {
        if (rnd(100) < 50) {
            targetStructureID = -1;
            targetUnitID = findEnemyUnit();
            if (targetUnitID < 0) {
                targetStructureID = findEnemyStructure();
            }
        } else {
            targetUnitID = -1;
            targetStructureID = findEnemyStructure();
            if (targetStructureID < 0) {
                targetUnitID = findEnemyStructure();
            }
        }
        return targetStructureID > -1 || targetUnitID > -1;
    }

    int cPlayerBrainMissionKindAttack::findEnemyStructure() const {
        int target = -1;
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *theStructure = structure[i];
            if (!theStructure) continue;
            if (!theStructure->isValid()) continue;
            if (theStructure->getPlayer() == player) continue; // skip self
            if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies
            if (!map.isStructureVisible(theStructure, player)) continue; // skip non-visible targets

            // enemy structure
            target =  theStructure->getStructureId();
            if (rnd(100) < 10) {
                break; // this way we kind of have randomly another target...
            }
        }
        return target;
    }

    int cPlayerBrainMissionKindAttack::findEnemyUnit() const {
        int target = -1;
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &pUnit = unit[i];
            if (!pUnit.isValid()) continue;
            if (pUnit.getPlayer() == player) continue; // skip self
            if (pUnit.getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
            if (!map.isVisible(pUnit.getCell(), player)) continue; // skip non visible targets
            // enemy unit
            target = i;
            if (rnd(100) < 5) {
                break; // this way we kind of have randomly another target...
            }
        }
        return target;
    }

    void cPlayerBrainMissionKindAttack::think_Execute() {
        if (targetStructureID < 0 && targetUnitID < 0) {
            // this should not happen!
            assert(false);
        }

        if (targetStructureID > -1) {
            cAbstractStructure *pStructure = structure[targetStructureID];
            if (!pStructure || !pStructure->isValid()) {
                mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
                return;
            }

            const std::vector<int> &units = mission->getUnits();
            for (auto &myUnit : units) {
                cUnit &aUnit = unit[myUnit];
                if (aUnit.isValid() && aUnit.isIdle()) {
                    log("cPlayerBrainMissionKindAttack::thinkState_Execute(): Ordering unit to attack!");
                    aUnit.attackStructure(targetStructureID);
                }
            }
        } else if (targetUnitID > -1) {
            cUnit &targetUnit = unit[targetUnitID];
            const std::vector<int> &units = mission->getUnits();
            for (auto &myUnit : units) {
                cUnit &aUnit = unit[myUnit];
                if (aUnit.isValid() && aUnit.isIdle()) {
                    log("cPlayerBrainMissionKindAttack::thinkState_Execute(): Ordering unit to attack!");
                    aUnit.attackUnit(targetUnitID);
                }
            }
        }
    }

    void cPlayerBrainMissionKindAttack::onNotify(const s_GameEvent &event) {
        cPlayerBrainMissionKind::onNotify(event);

        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindAttack::onNotify() -> %s", event.toString(event.eventType));
        log(msg);

        switch(event.eventType) {
            case GAME_EVENT_DESTROYED:
                onEventDestroyed(event);
                break;
            case GAME_EVENT_DEVIATED:
                onEventDeviated(event);
                break;
        }
    }

    void cPlayerBrainMissionKindAttack::onEventDeviated(const s_GameEvent &event) {
        if (event.entityType == UNIT) {
            cUnit &entityUnit = unit[event.entityID];
            if (entityUnit.getPlayer() == player) {
                // the unit is ours, if it was a target, then we can forget it.
                if (targetUnitID == event.entityID) {
                    // our target got deviated, so it is no longer a threat
                    targetUnitID = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
                }
            }
        }
    }

    void cPlayerBrainMissionKindAttack::onEventDestroyed(const s_GameEvent &event) {
        if (event.entityType == UNIT) {
            if (event.player != player) {
                // our target got destroyed?
                if (targetUnitID == event.entityID) {
                    targetUnitID = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
                }
            }
        } else if (event.entityType == STRUCTURE) {
            if (event.player != player) {
                // our target got destroyed
                if (targetStructureID == event.entityID) {
                    targetStructureID = -1;
                    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
                }
            }
        }
    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindAttack::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindAttack *copy = new cPlayerBrainMissionKindAttack(player, mission);
        copy->targetUnitID = targetUnitID;
        copy->targetStructureID = targetStructureID;
        copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
        copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
        copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
        copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
        return copy;
    }

    void cPlayerBrainMissionKindAttack::onNotify_SpecificStateSwitch(const s_GameEvent &event) {
        // NOOP
    }

}