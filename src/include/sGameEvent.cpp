#include "include/sGameEvent.h"

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
        case eGameEventType::GAME_EVENT_GAMESTATE:
            return "GAME_EVENT_GAMESTATE";
        default:
            assert(false && "Unknown game event type for toString()");
            break;
    }
    return "";
}


const std::string s_GameEvent::toString(const s_GameEvent &event)
{
    return std::format("cGameEvent [type={}], [entityType={}], [entityId={}], [entitySpecificType={} ={}], [isReinforce={}], [atCell={}], [buildingListItem={}] [originId={}]",
                           toString(event.eventType),
                           eBuildTypeString(event.entityType),
                           event.entityID,
                           event.entitySpecificType,
                           toStringBuildTypeSpecificType(event.entityType, event.entitySpecificType),
                           event.isReinforce ? "true" : "false",
                           event.atCell,
                           event.buildingListItem ? "present" : "nullptr",
                           event.originId
                          );
}