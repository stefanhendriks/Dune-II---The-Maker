#include "cPlayerBrainMissionKindDeathHand.h"

#include "d2tmc.h"
#include "definitions.h"
#include "player/cPlayer.h"

#include <fmt/core.h>

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

        if (target < 0) {
            // find any unit to attack instead
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
        game.onNotifyGameEvent(event);

        // clear out item
        itemToLaunch = nullptr;

        // clear out target, figure next target later
        target = -1;

        // wait for the next missile to be ready!
        mission->changeState(ePlayerBrainMissionState::PLAYERBRAINMISSION_STATE_PREPARE_AWAIT_RESOURCES);
    }

    void cPlayerBrainMissionKindDeathHand::onNotifyGameEvent(const s_GameEvent &event) {
        cPlayerBrainMissionKind::onNotifyGameEvent(event);

        log(fmt::format("cPlayerBrainMissionKindDeathHand::onNotifyGameEvent() -> {}", event.toString(event.eventType)).c_str());

        if (event.eventType == eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED) {
            onBuildItemCancelled(event);
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