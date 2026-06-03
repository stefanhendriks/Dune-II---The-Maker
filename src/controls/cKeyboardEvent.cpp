#include "cKeyboardEvent.h"
#include "controls/cKeyBindings.h"
#include "include/cAssert.h"

#include <format>

namespace {

const char *toStringKeyboardEventType(const eKeyEventType type)
{
    switch (type) {
        case eKeyEventType::NONE:
            return "NONE";
        case eKeyEventType::HOLD:
            return "HOLD";
        case eKeyEventType::PRESSED:
            return "PRESSED";
    }
    return "";
}

}

cKeyboardEvent::cKeyboardEvent(eKeyEventType eventType, const std::bitset<SDL_NUM_SCANCODES> &keys,
                               const s_KeysCombo &combo, const cKeyBindings *keyBindings, std::string textInput) :
    m_eventType(eventType),
    m_keys(keys),
    m_combo(combo),
    m_keyBindings(keyBindings),
    m_textInput(std::move(textInput))
{
}

const std::string cKeyboardEvent::toString() const
{
    std::string str = std::format("cKeyboardEvent [type={}], keys: ", toStringKeyboardEventType(m_eventType));
    str.append("[");
    for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc) {
        if (!m_keys.test(static_cast<size_t>(sc))) continue;
        str.append(SDL_GetScancodeName(static_cast<SDL_Scancode>(sc)));
    }
    str.append("]");
    if (!m_textInput.empty()) {
        str.append(std::format(", text: [{}]", m_textInput));
    }

    return str;
}

eKeyEventType cKeyboardEvent::getType() const
{
    return m_eventType;
}

bool cKeyboardEvent::isType(eKeyEventType type) const
{
    return m_eventType == type;
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

bool cKeyboardEvent::hasKey(SDL_Scancode scanCode) const
{
    if (scanCode < 0 || scanCode >= SDL_NUM_SCANCODES) return false;
    return m_keys.test(static_cast<size_t>(scanCode));
}

bool cKeyboardEvent::hasKeys(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const
{
    return hasKey(firstScanCode) && hasKey(secondScanCode);
}

bool cKeyboardEvent::hasEitherKey(SDL_Scancode firstScanCode, SDL_Scancode secondScanCode) const
{
    return hasKey(firstScanCode) || hasKey(secondScanCode);
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
