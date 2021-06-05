#include "include/d2tmh.h"

#include "cPlayerBrainMissionKind.h"
#include "cPlayerBrainMissionKindDeathHand.h"

namespace brains {

    cPlayerBrainMissionKindDeathHand::cPlayerBrainMissionKindDeathHand(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        target = -1;
        itemToDeploy = nullptr;
        logbook("cPlayerBrainMissionKindDeathHand() constructor");
    }

    cPlayerBrainMissionKindDeathHand::~cPlayerBrainMissionKindDeathHand() {

    }

    bool cPlayerBrainMissionKindDeathHand::think_SelectTarget() {
        // and execute whatever?? (can merge with select target state?)
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *theStructure = structure[i];
            if (!theStructure) continue;
            if (!theStructure->isValid()) continue;
            if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies and self
            // enemy structure
            target = theStructure->getCell();
            if (rnd(100) < 25) {
                break; // this way we kind of have randomly another target...
            }
        }
        return target > -1;
    }

    void cPlayerBrainMissionKindDeathHand::think_Execute() {
        // launch missile by sending event
        s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_SPECIAL_DEPLOYED,
                .entityType = eBuildType::SPECIAL,
                .entityID = -1,
                .player = player,
                .entitySpecificType = -1,
                .atCell = target,
                .isReinforce = false,
                .buildingListItem = itemToDeploy
        };
        game.onNotify(event);

        itemToDeploy = nullptr;
    }

    void cPlayerBrainMissionKindDeathHand::onNotify(const s_GameEvent &event) {
        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindDeathHand::onNotify(), for player [%d] -> %s", player->getId(), event.toString(event.eventType));
        logbook(msg);

        switch(event.eventType) {
            case eGameEventType::GAME_EVENT_SPECIAL_READY:
                onMySpecialIsReady(event);
                // should repair when under 75%?
                break;
        }
    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindDeathHand::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindDeathHand *copy = new cPlayerBrainMissionKindDeathHand(player, mission);
        copy->target = target;
        copy->itemToDeploy = itemToDeploy;
        return copy;
    }

    void cPlayerBrainMissionKindDeathHand::onMySpecialIsReady(const s_GameEvent &event) {
        if (event.player == player) {
            if (event.entityType == eBuildType::SPECIAL) {
                itemToDeploy = event.buildingListItem;
            }

            // missile ready, select target
            mission->changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_SELECT_TARGET);
        }
    }

}