#include "cKeyboardEvent.h"

cKeyboardEvent::cKeyboardEvent(eKeyEventType eventType, std::set<int> & keys) :
        eventType(eventType),
        keys(keys) {

}

int cKeyboardEvent::getGroupNumber() const {
    if (hasKey(KEY_1)) {
        return 1;
    }
    if (hasKey(KEY_2)) {
        return 2;
    }
    if (hasKey(KEY_3)) {
        return 3;
    }
    if (hasKey(KEY_4)) {
        return 4;
    }
    if (hasKey(KEY_5)) {
        return 5;
    }
    return 0;
}