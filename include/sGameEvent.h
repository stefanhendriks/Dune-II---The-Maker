#pragma once

#include "enums.h"
#include "sidebar/cBuildingListItem.h"
#include "utils/common.h"

#include <fmt/core.h>

#include <cassert>
#include <string>

class cPlayer;
class cBuildingList;

struct s_GameEvent {
    eGameEventType eventType = eGameEventType::GAME_EVENT_NONE;

    /**
     * kind of entity this applies to.
     * In case of eventType == DISCOVERED, this is the entityType being discovered
     */
    eBuildType entityType = eBuildType::SPECIAL;

    /**
     * which entity? (ID),
     * in case of eventType == DISCOVERED, this is the entityID being discovered
     */
    int entityID = -1;

    /**
     * If eventType == CREATED/DESTROYED/DEVIATED/DAMAGED refers to player of entity.
     * If eventType == DISCOVERED then this player refers to the player who has discovered the entity.
     */
    cPlayer *player = nullptr;
    int entitySpecificType = -1; // type of <entityType>, ie, if entityType is STRUCTURE. This value can be CONSTYARD
    int atCell = -1;        // if applicable (== > -1) where on the map did this event happen?
    bool isReinforce = false;       // only applicable for UNIT and CREATED events. So we can distinguish between 'normal' CREATED units and reinforced units.
    cBuildingListItem *buildingListItem = nullptr; // if buildingListItem is ready (special, or not)
    cBuildingList *buildingList = nullptr; // in case buildingList is available (or not)
    int originId = -1; // in case damaged event, this is the UNIT id that damaged it

    // TODO: figure out a way to have bags of data depending on type of event without the need of expanding this generic GAME_EVENT struct

    static const char* toString(const eGameEventType &eventType) {
        switch (eventType) {
            case eGameEventType::GAME_EVENT_NONE: return "GAME_EVENT_NONE";
            case eGameEventType::GAME_EVENT_CREATED: return "GAME_EVENT_CREATED";
            case eGameEventType::GAME_EVENT_DESTROYED: return "GAME_EVENT_DESTROYED";
            case eGameEventType::GAME_EVENT_DAMAGED: return "GAME_EVENT_DAMAGED";
            case eGameEventType::GAME_EVENT_DECAY: return "GAME_EVENT_DECAY";
            case eGameEventType::GAME_EVENT_DEVIATED: return "GAME_EVENT_DEVIATED";
            case eGameEventType::GAME_EVENT_DISCOVERED: return "GAME_EVENT_DISCOVERED";
            case eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET: return "GAME_EVENT_SPECIAL_SELECT_TARGET";
            case eGameEventType::GAME_EVENT_SPECIAL_LAUNCH: return "GAME_EVENT_SPECIAL_LAUNCH";
            case eGameEventType::GAME_EVENT_CANNOT_BUILD: return "GAME_EVENT_CANNOT_BUILD";
            case eGameEventType::GAME_EVENT_CANNOT_CREATE_PATH: return "GAME_EVENT_CANNOT_CREATE_PATH";
            case eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED: return "GAME_EVENT_LIST_ITEM_FINISHED";
            case eGameEventType::GAME_EVENT_LIST_ITEM_ADDED: return "GAME_EVENT_LIST_ITEM_ADDED";
            case eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED: return "GAME_EVENT_LIST_ITEM_CANCELLED";
            case eGameEventType::GAME_EVENT_LIST_BECAME_AVAILABLE: return "GAME_EVENT_LIST_BECAME_AVAILABLE";
            case eGameEventType::GAME_EVENT_LIST_BECAME_UNAVAILABLE: return "GAME_EVENT_LIST_BECAME_UNAVAILABLE";
            case eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN: return "GAME_EVENT_ABOUT_TO_BEGIN";
            case eGameEventType::GAME_EVENT_LIST_ITEM_PLACE_IT: return "GAME_EVENT_LIST_ITEM_PLACE_IT";
            case eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED: return "GAME_EVENT_SPICE_BLOOM_SPAWNED";
            case eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW: return "GAME_EVENT_SPICE_BLOOM_BLEW";
            case eGameEventType::GAME_EVENT_PLAYER_DEFEATED: return "GAME_EVENT_PLAYER_DEFEATED";
            case eGameEventType::GAME_EVENT_SPECIAL_LAUNCHED: return "GAME_EVENT_SPECIAL_LAUNCHED";
            default:
                assert(false && "Unknown game event type for toString()");
                break;
        }
        return "";
    }

    static const std::string toString(const s_GameEvent &event) {
        return fmt::format("cGameEvent [type={}], [entityType={}], [entityId={}], [entitySpecificType={} ={}], [isReinforce={}], [atCell={}], [buildingListItem={}] [originId={}]",
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
};
