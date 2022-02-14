#include "d2tmh.h"
#include "cPlayerBrainMissionKind.h"

#include <fmt/core.h>

namespace brains {

    const char* ePlayerBrainMissionKindString(const ePlayerBrainMissionKind &kind) {
        switch (kind) {
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_DEFEND: return "PLAYERBRAINMISSION_KIND_DEFEND";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK: return "PLAYERBRAINMISSION_KIND_ATTACK";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_IMPROVE_ECONOMY: return "PLAYERBRAINMISSION_IMPROVE_ECONOMY";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE: return "PLAYERBRAINMISSION_KIND_EXPLORE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_AIRSTRIKE: return "PLAYERBRAINMISSION_KIND_AIRSTRIKE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_FIND_SPICE: return "PLAYERBRAINMISSION_KIND_FIND_SPICE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_HARASS: return "PLAYERBRAINMISSION_KIND_HARASS";
            default:
                assert(false && "Unknown kind?");
                break;
        }
        return "";
    }

    cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission) : player(player), mission(mission) {
        player->log("cPlayerBrainMissionKind() constructor");
        specificPlayerForEventToGoToSelectTargetState = player;

        // if not NONE...
        specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_NONE;

        specificBuildIdToGoToSelectTargetState = -1; // if > -1, the specificBuildTypeToGoToSelectTargetState should also be given
        specificBuildTypeToGoToSelectTargetState = eBuildType::STRUCTURE; // bogus without buildId
    }

    cPlayerBrainMissionKind::~cPlayerBrainMissionKind() {
        this->mission = nullptr; // we do not own it, so don't delete it
        this->player = nullptr; // we do not own it, so don't delete it
    }

    /**
     * Logs for a mission, do not call from the MissionKind constructors, as this will result into a SIGSEV (since the
     * mission->log() uses the missionKind as well, it will result into a SIGSEV when being cloned etc)
     * @param txt
     */
    void cPlayerBrainMissionKind::log(const char *txt) {
        mission->log(fmt::format("cPlayerBrainMissionKind | {}", txt).c_str());
    }

    void cPlayerBrainMissionKind::onNotifyGameEvent(const s_GameEvent &event) {
        log(fmt::format("cPlayerBrainMissionKind::onNotifyGameEvent() -> {}", event.toString(event.eventType)).c_str());

        if (event.player == specificPlayerForEventToGoToSelectTargetState) {
            if (specificEventTypeToGoToSelectTargetState != eGameEventType::GAME_EVENT_NONE) {
                bool executeSpecificStateSwitch = false;
                if (event.eventType == specificEventTypeToGoToSelectTargetState) {
                    if (specificBuildIdToGoToSelectTargetState > -1) {
                        executeSpecificStateSwitch = (
                                event.entitySpecificType == specificBuildIdToGoToSelectTargetState &&
                                event.entityType == specificBuildTypeToGoToSelectTargetState
                        );
                    } else {
                        executeSpecificStateSwitch = true;
                    }
                }
                if (executeSpecificStateSwitch) {
                    onExecuteSpecificStateSwitch(event);
                }
            }
        }
    }

    void cPlayerBrainMissionKind::onExecuteSpecificStateSwitch(const s_GameEvent &event) {
        // unit got created, not reinforced - add it to the units list (ie saboteur to command)
        if (event.entityType == UNIT && !event.isReinforce) {
            cUnit &entityUnit = unit[event.entityID];

            // this unit has not been assigned to a mission yet
            if (!entityUnit.isAssignedAnyMission()) {
                entityUnit.assignMission(mission->getUniqueId());
                mission->getUnits().push_back(entityUnit.iID);
            }
        }


        // just before we set select-target state, we call this method. So our child-implementations
        // can change state accordingly
        onNotify_SpecificStateSwitch(event);

        mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
    }

}