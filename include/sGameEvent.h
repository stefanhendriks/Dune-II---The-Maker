#ifndef D2TM_SGAMEEVENT_H
#define D2TM_SGAMEEVENT_H

#include "enums.h"
#include <assert.h>

class cPlayer;
class cBuildingListItem;

struct s_GameEvent {
    eGameEventType eventType = eGameEventType::GAME_EVENT_NONE;

    /**
     * kind of entity this applies to.
     * In case of eventType == DISCOVERED, this is the entityType being discovered
     */
    eBuildType entityType;

    /**
     * which entity? (ID),
     * in case of eventType == DISCOVERED, this is the entityID being discovered
     */
    int entityID;

    /**
     * If eventType == CREATED/DESTROYED/DEVIATED/DAMAGED refers to player of entity.
     * If eventType == DISCOVERED then this player refers to the player who has discovered the entity.
     */
    cPlayer *player;
    int entitySpecificType; // type of <entityType>, ie, if entityType is STRUCTURE. This value can be CONSTYARD
    int atCell = -1;        // if applicable (== > -1) where on the map did this event happen?
    bool isReinforce = false;       // only applicable for UNIT and CREATED events. So we can distinguish between 'normal' CREATED units and reinforced units.
    cBuildingListItem *buildingListItem = nullptr; // if buildingListItem is ready (special, or not)

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
            case eGameEventType::GAME_EVENT_SPECIAL_READY: return "GAME_EVENT_SPECIAL_READY";
            case eGameEventType::GAME_EVENT_SPECIAL_DEPLOYED: return "GAME_EVENT_SPECIAL_DEPLOYED";
            default:
                assert(false);
                break;
        }
        return "";
    }
};

#endif //D2TM_SGAMEEVENT_H
