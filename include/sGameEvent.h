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
    bool isReinforce = false;       // only applicable for UNIT and CREATED events. So we can distinguish between 'normal' CREATED units and reinforced units.

    static const char* toString(const eGameEventType &eventType) {
        switch (eventType) {
            case eGameEventType::GAME_EVENT_NONE: return "GAME_EVENT_NONE";
            case eGameEventType::GAME_EVENT_CREATED: return "GAME_EVENT_CREATED";
            case eGameEventType::GAME_EVENT_DESTROYED: return "GAME_EVENT_DESTROYED";
            case eGameEventType::GAME_EVENT_DAMAGED: return "GAME_EVENT_DAMAGED";
            case eGameEventType::GAME_EVENT_DECAY: return "GAME_EVENT_DECAY";
            default:
                assert(false);
                break;
        }
        return "";
    }
};

#endif //D2TM_SGAMEEVENT_H
