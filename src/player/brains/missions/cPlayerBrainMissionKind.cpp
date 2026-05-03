#include "player/cPlayer.h"
#include "cPlayerBrainMission.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "gameobjects/map/cMap.h"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"

#include <format>
#include <cassert>

namespace brains {

const char *ePlayerBrainMissionKindString(const ePlayerBrainMissionKind &kind)
{
    switch (kind) {
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_DEFEND:
            return "PLAYERBRAINMISSION_KIND_DEFEND";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK:
            return "PLAYERBRAINMISSION_KIND_ATTACK";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_IMPROVE_ECONOMY:
            return "PLAYERBRAINMISSION_IMPROVE_ECONOMY";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND:
            return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR:
            return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN:
            return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE:
            return "PLAYERBRAINMISSION_KIND_EXPLORE";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_AIRSTRIKE:
            return "PLAYERBRAINMISSION_KIND_AIRSTRIKE";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_FIND_SPICE:
            return "PLAYERBRAINMISSION_KIND_FIND_SPICE";
        case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_HARASS:
            return "PLAYERBRAINMISSION_KIND_HARASS";
        default:
            assert(false && "Unknown kind?");
            break;
    }
    return "";
}

cPlayerBrainMissionKind::cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission *mission) : player(player), mission(mission)
{
    assert(player != nullptr);
    assert(mission != nullptr);
    player->log("cPlayerBrainMissionKind() constructor");
    specificPlayerForEventToGoToSelectTargetState = player;

    // if not NONE...
    specificEventTypeToGoToSelectTargetState = eGameEventType::GAME_EVENT_NONE;

    specificBuildIdToGoToSelectTargetState = -1; // if > -1, the specificBuildTypeToGoToSelectTargetState should also be given
    specificBuildTypeToGoToSelectTargetState = eBuildType::STRUCTURE; // bogus without buildId
}

cPlayerBrainMissionKind::~cPlayerBrainMissionKind()
{
    this->mission = nullptr; // we do not own it, so don't delete it
    this->player = nullptr; // we do not own it, so don't delete it
}

/**
 * Logs for a mission, do not call from the MissionKind constructors, as this will result into a SIGSEV (since the
 * mission->log() uses the missionKind as well, it will result into a SIGSEV when being cloned etc)
 * @param txt
 */
void cPlayerBrainMissionKind::log(const char *txt)
{
    mission->log(std::format("cPlayerBrainMissionKind | {}", txt).c_str());
}

void cPlayerBrainMissionKind::onNotifyGameEvent(const s_GameEvent &event)
{
    log(std::format("cPlayerBrainMissionKind::onNotifyGameEvent() -> {}", event.toString(event.eventType)).c_str());

    if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
        if (commonEvent->player == specificPlayerForEventToGoToSelectTargetState) {
            if (specificEventTypeToGoToSelectTargetState != eGameEventType::GAME_EVENT_NONE) {
                bool executeSpecificStateSwitch = false;
                if (event.eventType == specificEventTypeToGoToSelectTargetState) {
                    if (specificBuildIdToGoToSelectTargetState > -1) {
                        executeSpecificStateSwitch = (
                                commonEvent->entitySpecificType == specificBuildIdToGoToSelectTargetState &&
                                commonEvent->entityType == specificBuildTypeToGoToSelectTargetState
                            );
                    }
                    else {
                        executeSpecificStateSwitch = true;
                    }
                }
                if (executeSpecificStateSwitch) {
                    onExecuteSpecificStateSwitch(event);
                }
            }
        }
    }
}

void cPlayerBrainMissionKind::onExecuteSpecificStateSwitch(const s_GameEvent &event)
{
    if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
        // unit got created, not reinforced - add it to the units list (ie saboteur to command)
        if (commonEvent->entityType == UNIT && !commonEvent->isReinforce) {
            cUnit *entityUnit = game.m_gameObjectsContext->getUnit(commonEvent->entityID);

            // this unit has not been assigned to a mission yet
            if (!entityUnit->isAssignedAnyMission()) {
                entityUnit->assignMission(mission->getUniqueId());
                mission->getUnits().push_back(entityUnit->iID);
            }
        }
    }


    // just before we set select-target state, we call this method. So our child-implementations
    // can change state accordingly
    if (const auto *buildEvent = std::get_if<BuildingEvent>(&event.data)) {
        onNotify_SpecificStateSwitch(*buildEvent);
    }
    mission->changeState(PLAYERBRAINMISSION_STATE_SELECT_TARGET);
}

}