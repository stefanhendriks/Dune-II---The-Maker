#include "cKeyboardEvent.h"
#include "controls/cKeyBindings.h"

cKeyboardEvent::cKeyboardEvent(eKeyEventType eventType, const std::set<SDL_Scancode> &keys,
                               const s_KeysCombo &combo, const cKeyBindings *keyBindings) :
    m_eventType(eventType),
    m_keys(keys),
    m_combo(combo),
    m_keyBindings(keyBindings)
{
}

bool cKeyboardEvent::isAction(eKeyAction action) const
{
    if (!m_keyBindings) return false;
    return m_keyBindings->matches(m_keys, m_combo, action);
}

int cKeyboardEvent::getGroupNumber() const
{
    if (isAction(eKeyAction::GROUP_1)) return 1;
    if (isAction(eKeyAction::GROUP_2)) return 2;
    if (isAction(eKeyAction::GROUP_3)) return 3;
    if (isAction(eKeyAction::GROUP_4)) return 4;
    if (isAction(eKeyAction::GROUP_5)) return 5;
    return 0;
}

bool cKeyboardEvent::isPrintable() const
{
    if (m_keys.empty() || m_keys.size() > 2) return false;
    for (auto sc : m_keys) {
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
    return m_combo.shiftPressed;
}

bool cKeyboardEvent::isAltPressed() const
{
    return m_combo.altPressed;
}

bool cKeyboardEvent::isCtrlPressed() const
{
    return m_combo.ctrlPressed;
}

char cKeyboardEvent::getChar() const
{
    if (!isPrintable()) return '\0';
    SDL_Scancode sc = *m_keys.begin();
    SDL_Keycode key = SDL_GetKeyFromScancode(sc);
    return static_cast<char>(key);
}
