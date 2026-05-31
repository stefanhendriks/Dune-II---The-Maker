#include "cKeyboardEvent.h"
#include "controls/cKeyBindings.h"

cKeyboardEvent::cKeyboardEvent(eKeyEventType eventType, const std::bitset<SDL_NUM_SCANCODES> &keys,
                               const s_KeysCombo &combo, const cKeyBindings *keyBindings, std::string textInput) :
    m_eventType(eventType),
    m_keys(keys),
    m_combo(combo),
    m_keyBindings(keyBindings),
    m_textInput(std::move(textInput))
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
    if (isAction(eKeyAction::GROUP_6)) return 6;
    return 0;
}

bool cKeyboardEvent::isPrintable() const
{
    const size_t keyCount = m_keys.count();
    if (keyCount == 0 || keyCount > 2) return false;

    for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc) {
        if (!m_keys.test(static_cast<size_t>(sc))) continue;

        if (sc != SDL_SCANCODE_LSHIFT && sc != SDL_SCANCODE_RSHIFT) {
            SDL_Keycode key = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(sc));
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
    return hasKey(SDL_SCANCODE_RETURN) || hasKey(SDL_SCANCODE_KP_ENTER);
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

bool cKeyboardEvent::hasTextInput() const
{
    return !m_textInput.empty();
}

const std::string &cKeyboardEvent::getTextInput() const
{
    return m_textInput;
}

// char cKeyboardEvent::getChar() const
// {
//     if (!isPrintable()) return '\0';
//     SDL_Scancode sc = *m_keys.begin();
//     SDL_Keycode key = SDL_GetKeyFromScancode(sc);
//     char c = static_cast<char>(key);
    
//     // If shift is pressed, handle uppercase and special characters
//     if (isShiftPressed()) {
//         // Uppercase letters
//         if (c >= 'a' && c <= 'z') {
//             return c - 32;  // Convert to uppercase
//         }
//         // Special characters with shift on US QWERTY keyboard
//         switch (c) {
//             case '1': return '!';
//             case '2': return '@';
//             case '3': return '#';
//             case '4': return '$';
//             case '5': return '%';
//             case '6': return '^';
//             case '7': return '&';
//             case '8': return '*';
//             case '9': return '(';
//             case '0': return ')';
//             case '-': return '_';
//             case '=': return '+';
//             case '[': return '{';
//             case ']': return '}';
//             case '\\': return '|';
//             case ';': return ':';
//             case '\'': return '"';
//             case ',': return '<';
//             case '.': return '>';
//             case '/': return '?';
//             case '`': return '~';
//         }
//     }
    
//     return c;
// }
