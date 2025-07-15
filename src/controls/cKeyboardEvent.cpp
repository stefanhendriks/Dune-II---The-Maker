#include "cKeyboardEvent.h"

cKeyboardEvent::cKeyboardEvent(eKeyEventType eventType, std::set<SDL_Scancode> &keys) :
    eventType(eventType),
    keys(keys)
{

}

int cKeyboardEvent::getGroupNumber() const
{
    if (hasKey(SDL_SCANCODE_1)) {
        return 1;
    }
    if (hasKey(SDL_SCANCODE_2)) {
        return 2;
    }
    if (hasKey(SDL_SCANCODE_3)) {
        return 3;
    }
    if (hasKey(SDL_SCANCODE_4)) {
        return 4;
    }
    if (hasKey(SDL_SCANCODE_5)) {
        return 5;
    }
    return 0;
}