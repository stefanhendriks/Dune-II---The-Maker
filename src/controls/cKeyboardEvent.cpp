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

bool cKeyboardEvent::isPrintable() const
{
    if (keys.empty() || keys.size() > 2) return false;
    for (auto sc : keys) {
        if (sc != SDL_SCANCODE_LSHIFT && sc != SDL_SCANCODE_RSHIFT) {
            SDL_Keycode key = SDL_GetKeyFromScancode(sc);
            if (key >= 32 && key <= 126) return true;
        }
    }
    return false;
}

bool cKeyboardEvent::isBackspace() const
{
    return hasKey(SDL_SCANCODE_BACKSPACE);
}

bool cKeyboardEvent::isEnter() const
{
    return hasKey(SDL_SCANCODE_RETURN);
}

bool cKeyboardEvent::isShiftPressed() const
{
    return hasKey(SDL_SCANCODE_LSHIFT) || hasKey(SDL_SCANCODE_RSHIFT);
}

char cKeyboardEvent::getChar() const
{
    if (!isPrintable()) return '\0';
    SDL_Scancode sc = *keys.begin();
    SDL_Keycode key = SDL_GetKeyFromScancode(sc);
    return static_cast<char>(key);
}