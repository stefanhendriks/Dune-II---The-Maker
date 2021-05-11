#ifndef D2TM_SGAMEEVENT_H
#define D2TM_SGAMEEVENT_H

#include "enums.h"
#include <assert.h>

struct s_GameEvent {
    eGameEventType eventType = eGameEventType::GAME_EVENT_NONE;

    eBuildType entityType;  // kind of entity this applies to
    int entityID;           // which entity? (ID)
    int entityOwnerID;      // who owns this entity? (player ID)
    int entitySpecificType; // type of <entityType>, ie, if entityType is STRUCTURE. This value can be CONSTYARD

    static const char* toString(const eGameEventType &eventType) {
        switch (eventType) {
            case eGameEventType::GAME_EVENT_NONE: return "GAME_EVENT_NONE";
            case eGameEventType::GAME_EVENT_STRUCTURE_CREATED: return "GAME_EVENT_STRUCTURE_CREATED";
            case eGameEventType::GAME_EVENT_STRUCTURE_DESTROYED: return "GAME_EVENT_STRUCTURE_DESTROYED";
            default:
                assert(false);
                break;
        }
        return "";
    }
};

#endif //D2TM_SGAMEEVENT_H
