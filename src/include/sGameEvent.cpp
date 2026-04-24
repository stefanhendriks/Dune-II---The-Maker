#include "include/sGameEvent.h"
// #include "sidebar/cBuildingListItem.h"
#include "utils/common.h"

const char *s_GameEvent::toString(const eGameEventType &eventType)
{
    switch (eventType) {
        case eGameEventType::GAME_EVENT_NONE:
            return "GAME_EVENT_NONE";
        case eGameEventType::GAME_EVENT_CREATED:
            return "GAME_EVENT_CREATED";
        case eGameEventType::GAME_EVENT_DESTROYED:
            return "GAME_EVENT_DESTROYED";
        case eGameEventType::GAME_EVENT_DAMAGED:
            return "GAME_EVENT_DAMAGED";
        case eGameEventType::GAME_EVENT_DECAY:
            return "GAME_EVENT_DECAY";
        case eGameEventType::GAME_EVENT_DEVIATED:
            return "GAME_EVENT_DEVIATED";
        case eGameEventType::GAME_EVENT_DISCOVERED:
            return "GAME_EVENT_DISCOVERED";
        case eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET:
            return "GAME_EVENT_SPECIAL_SELECT_TARGET";
        case eGameEventType::GAME_EVENT_SPECIAL_LAUNCH:
            return "GAME_EVENT_SPECIAL_LAUNCH";
        case eGameEventType::GAME_EVENT_CANNOT_BUILD:
            return "GAME_EVENT_CANNOT_BUILD";
        case eGameEventType::GAME_EVENT_CANNOT_CREATE_PATH:
            return "GAME_EVENT_CANNOT_CREATE_PATH";
        case eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED:
            return "GAME_EVENT_LIST_ITEM_FINISHED";
        case eGameEventType::GAME_EVENT_LIST_ITEM_ADDED:
            return "GAME_EVENT_LIST_ITEM_ADDED";
        case eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED:
            return "GAME_EVENT_LIST_ITEM_CANCELLED";
        case eGameEventType::GAME_EVENT_LIST_BECAME_AVAILABLE:
            return "GAME_EVENT_LIST_BECAME_AVAILABLE";
        case eGameEventType::GAME_EVENT_LIST_BECAME_UNAVAILABLE:
            return "GAME_EVENT_LIST_BECAME_UNAVAILABLE";
        case eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN:
            return "GAME_EVENT_ABOUT_TO_BEGIN";
        case eGameEventType::GAME_EVENT_LIST_ITEM_PLACE_IT:
            return "GAME_EVENT_LIST_ITEM_PLACE_IT";
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED:
            return "GAME_EVENT_SPICE_BLOOM_SPAWNED";
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW:
            return "GAME_EVENT_SPICE_BLOOM_BLEW";
        case eGameEventType::GAME_EVENT_PLAYER_DEFEATED:
            return "GAME_EVENT_PLAYER_DEFEATED";
        case eGameEventType::GAME_EVENT_SPECIAL_LAUNCHED:
            return "GAME_EVENT_SPECIAL_LAUNCHED";
        case eGameEventType::GAME_EVENT_DEPLOY_UNIT:
            return "GAME_EVENT_DEPLOY_UNIT";
        case eGameEventType::GAME_EVENT_CREATE_UNIT:
            return "GAME_EVENT_CREATE_UNIT";
        default:
            assert(false && "Unknown game event type for toString()");
            break;
    }
    return "";
}


const std::string s_GameEvent::toString(const s_GameEvent &event)
{
    if (const auto *deployEvent = std::get_if<DeployUnitEvent>(&event.data)) {
        return std::format(
            "DeployUnitEvent[cell={}, unitType={}, playerId={}, onStart={}, reinforcement={}, hp={}]",
            deployEvent->iCell,
            deployEvent->unitType,
            deployEvent->iPlayer,
            deployEvent->bOnStart ? "true" : "false",
            deployEvent->isReinforcement ? "true" : "false",
            deployEvent->hpPercentage
        );
    }

    if (const auto *launchEvent = std::get_if<LaunchDeathHandEvent>(&event.data)) {
        return std::format(
            "LaunchDeathHandEvent[targetCell={}, itemToLaunch={}, playerId={}]",
            launchEvent->targetCell,
            launchEvent->itemToLaunch ? "present" : "nullptr",
            launchEvent->playerID
        );
    }

    if (const auto *damagedEvent = std::get_if<DamagedEvent>(&event.data)) {
        return std::format(
            "DamagedEvent[entityType={}, entityId={}, player={}, entitySpecificType={}, atCell={}, originId={}, originType={}]",
            eBuildTypeString(damagedEvent->entityType),
            damagedEvent->entityID,
            damagedEvent->player ? "present" : "nullptr",
            damagedEvent->entitySpecificType,
            damagedEvent->atCell,
            damagedEvent->originId,
            eBuildTypeString(damagedEvent->originType)
        );
    }

    if (const auto *buildingEvent = std::get_if<BuildingEvent>(&event.data)) {
        return std::format(
            "BuildingEvent[entityType={}, player={}, entitySpecificType={}, buildingListItem={}, buildingList={}]",
            eBuildTypeString(buildingEvent->entityType),
            buildingEvent->player ? "present" : "nullptr",
            buildingEvent->entitySpecificType,
            buildingEvent->buildingListItem ? "present" : "nullptr",
            buildingEvent->buildingList ? "present" : "nullptr"
        );
    }

    if (const auto *commonEvent = std::get_if<CommonEvent>(&event.data)) {
        return std::format(
            "CommonEvent[entityType={}, entityId={}, player={}, entitySpecificType={}, atCell={}, isReinforce={}]",
            eBuildTypeString(commonEvent->entityType),
            commonEvent->entityID,
            commonEvent->player ? "present" : "nullptr",
            commonEvent->entitySpecificType,
            commonEvent->atCell,
            commonEvent->isReinforce ? "true" : "false"
        );
    }
    
    return std::format("cGameEvent [type={}]",toString(event.eventType));
}