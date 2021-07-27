#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindSaboteur.h"

namespace brains {

    cPlayerBrainMissionKindSaboteur::cPlayerBrainMissionKindSaboteur(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        targetStructureID = -1;
        player->log("cPlayerBrainMissionKindSaboteur() constructor");
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
            if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
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
            const std::vector<int> &units = mission->getUnits();
            for (auto &myUnit : units) {
                cUnit &aUnit = unit[myUnit];
                if (aUnit.isValid() && aUnit.isIdle()) {
                    player->log("cPlayerBrainMission::thinkState_Execute(): Ordering unit to attack!");
                    UNIT_ORDER_ATTACK(myUnit, pStructure->getCell(), -1, targetStructureID, -1);
                }
            }
        }
    }

    void cPlayerBrainMissionKindSaboteur::onNotify(const s_GameEvent &event) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindSaboteur::onNotify() -> %s", event.toString(event.eventType));
        player->log(msg);

        switch(event.eventType) {
            case GAME_EVENT_DESTROYED:
                onEventDestroyed(event);
                break;
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
        return copy;
    }

}