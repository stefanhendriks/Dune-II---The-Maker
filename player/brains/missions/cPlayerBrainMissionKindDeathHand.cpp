#include "include/d2tmh.h"

#include "cPlayerBrainMissionKindDeathHand.h"

namespace brains {

    cPlayerBrainMissionKindDeathHand::cPlayerBrainMissionKindDeathHand(cPlayer *player, cPlayerBrainMission * mission) :  cPlayerBrainMissionKind(player, mission) {
        target = -1;
        itemToLaunch = nullptr;
        specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET; // ready for launch!
        specificBuildTypeToGoToSelectTargetState = eBuildType::SPECIAL;
        specificBuildIdToGoToSelectTargetState = SPECIAL_DEATHHAND;
        specificPlayerForEventToGoToSelectTargetState = player;
    }

    cPlayerBrainMissionKindDeathHand::~cPlayerBrainMissionKindDeathHand() {

    }

    bool cPlayerBrainMissionKindDeathHand::think_SelectTarget() {
        // and execute whatever?? (can merge with select target state?)
        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *theStructure = structure[i];
            if (!theStructure) continue;
            if (!theStructure->isValid()) continue;
            if (theStructure->getPlayer() == player) continue; // skip self
            if (theStructure->getPlayer()->isSameTeamAs(player)) continue; // skip allies
            if (!map.isStructureVisible(theStructure, player)) continue; // skip non-visible targets

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
                .eventType = eGameEventType::GAME_EVENT_SPECIAL_LAUNCH,
                .entityType = itemToLaunch->getBuildType(),
                .entityID = -1,
                .player = player,
                .entitySpecificType = itemToLaunch->getBuildId(),
                .atCell = target,
                .isReinforce = false,
                .buildingListItem = itemToLaunch
        };
        game.onNotify(event);

        // clear out item
        itemToLaunch = nullptr;

        // clear out target, figure next target later
        target = -1;

        // wait for the next missile to be ready!
        mission->changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
    }

    void cPlayerBrainMissionKindDeathHand::onNotify(const s_GameEvent &event) {
        cPlayerBrainMissionKind::onNotify(event);

        char msg[255];
        sprintf(msg, "cPlayerBrainMissionKindDeathHand::onNotify() -> %s", event.toString(event.eventType));
        log(msg);


        switch(event.eventType) {
            case eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED:
                onBuildItemCancelled(event);
                break;
        }
    }

    cPlayerBrainMissionKind *cPlayerBrainMissionKindDeathHand::clone(cPlayer *player, cPlayerBrainMission * mission) {
        cPlayerBrainMissionKindDeathHand *copy = new cPlayerBrainMissionKindDeathHand(player, mission);
        copy->target = target;
        copy->itemToLaunch = itemToLaunch;
        copy->specificEventTypeToGoToSelectTargetState = specificEventTypeToGoToSelectTargetState;
        copy->specificPlayerForEventToGoToSelectTargetState = specificPlayerForEventToGoToSelectTargetState;
        copy->specificBuildTypeToGoToSelectTargetState = specificBuildTypeToGoToSelectTargetState;
        copy->specificBuildIdToGoToSelectTargetState = specificBuildIdToGoToSelectTargetState;
        return copy;
    }

    void cPlayerBrainMissionKindDeathHand::onBuildItemCancelled(const s_GameEvent &event) {
        if (event.player == player) {
            // looks like the thing we had stored is being cancelled, so forget about it
            // this is usually done by a human player interfering (ie, when debugging)
            if (event.buildingListItem == itemToLaunch) {
                itemToLaunch = nullptr;
                mission->changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
            }
        }
    }

    void cPlayerBrainMissionKindDeathHand::onNotify_SpecificStateSwitch(const s_GameEvent &event) {
        // here we know it is the specific thing we are interested in (hence the "SpecificStateSwitch" name)
        if (event.player == player) {
            // it has a buildingListItem, so it is for us to command (launch!)
            if (event.buildingListItem) {
                itemToLaunch = event.buildingListItem;
            } else {
                assert(false && "Expected to have a buildingListItem");
            }
        }
    }

}