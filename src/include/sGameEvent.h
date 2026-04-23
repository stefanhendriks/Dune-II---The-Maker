#pragma once

#include "enums.h"
#include "eGameEventType.h"


#include <format>

#include <cassert>
#include <string>
#include <variant>

class cPlayer;
class cBuildingList;
class cBuildingListItem;

// GAME_EVENT_DEPLOY_UNIT
struct DeployUnitEvent {
    int iCell = -1;
    int unitType = -1;
    int iPlayer = -1;
    bool bOnStart = false;
    bool isReinforcement = false;
    float hpPercentage = 1.0F;
};

// GAME_EVENT_SPECIAL_LAUNCH
struct LaunchDeathHandEvent {
    int targetCell = -1;
    cBuildingListItem* itemToLaunch = nullptr;
    cPlayer* player = nullptr;
    int playerID = -1; // for looging purposes, as player pointer might not always be available
};


struct s_GameEvent {
    eGameEventType eventType = eGameEventType::GAME_EVENT_NONE;
    using GameEventData = std::variant<std::monostate, DeployUnitEvent, LaunchDeathHandEvent>;
    GameEventData data = std::monostate{};
    
    /**
     * kind of entity this applies to.
     * In case of eventType == DISCOVERED, this is the entityType being discovered
     * In case of eventType == GAME_EVENT_DAMAGED, this is the entityType being damaged
     */
    eBuildType entityType = eBuildType::UNKNOWN;

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

    int originId = -1; // in case GAME_EVENT_DAMAGED, this is the id that inflicted damage
    eBuildType originType = eBuildType::UNKNOWN; // in case GAME_EVENT_DAMAGED, this is the kind of entity that inflicted damage

    static const char *toString(const eGameEventType &eventType);

    static const std::string toString(const s_GameEvent &event);
};
