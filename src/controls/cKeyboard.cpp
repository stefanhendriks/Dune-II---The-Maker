#include "cKeyboard.h"
#include "cKeyBindings.h"
#include "cKeyboardEvent.h"
#include "utils/cIniFile.h"

namespace {

bool isValidScancode(SDL_Scancode scancode)
{
    return scancode >= 0 && scancode < SDL_SCANCODE_COUNT;
}

size_t toScancodeIndex(SDL_Scancode scancode)
{
    return static_cast<size_t>(scancode);
}

bool isPressed(const std::bitset<SDL_SCANCODE_COUNT> &keys, SDL_Scancode scancode)
{
    return isValidScancode(scancode) && keys.test(toScancodeIndex(scancode));
}

}

cKeyboard::cKeyboard() :
    m_keyboardObserver(nullptr),
    m_keyBindings(nullptr),
    m_keysPressed(),
    m_currentCombo()
{
}

cKeyboard::~cKeyboard() = default;

void cKeyboard::loadKeyBindings(const cSection *section)
{
    m_keyBindings = std::make_unique<cKeyBindings>();
    m_keyBindings->loadDefaults();
    if (section) {
        m_keyBindings->loadFromSection(*section);
    }
}

void cKeyboard::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_TEXT_INPUT) {
        if (event.text.text[0] != '\0') {
            m_textInputs.emplace_back(event.text.text);
        }
        return;
    }

    SDL_Scancode scancode = event.key.scancode;
    if (!isValidScancode(scancode)) return;

    const size_t index = toScancodeIndex(scancode);
    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
        m_keysPressed.set(index);
        m_keysReleased.reset(index);
    }
    else if (event.type == SDL_EVENT_KEY_UP) {
        m_keysPressed.reset(index);
        m_keysReleased.set(index);
    }

    m_currentCombo.altPressed   = isPressed(m_keysPressed, SDL_SCANCODE_LALT)   || isPressed(m_keysPressed, SDL_SCANCODE_RALT);
    m_currentCombo.ctrlPressed  = isPressed(m_keysPressed, SDL_SCANCODE_LCTRL)  || isPressed(m_keysPressed, SDL_SCANCODE_RCTRL);
    m_currentCombo.shiftPressed = isPressed(m_keysPressed, SDL_SCANCODE_LSHIFT) || isPressed(m_keysPressed, SDL_SCANCODE_RSHIFT);
}

void cKeyboard::updateState()
{
    static const std::bitset<SDL_SCANCODE_COUNT> noKeys;

    for (const auto &textInput : m_textInputs) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, noKeys, m_currentCombo, m_keyBindings.get(), textInput));
    }
    m_textInputs.clear();

    if (m_keysPressed.any()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, m_keysPressed, m_currentCombo, m_keyBindings.get()));
    }
    if (m_keysReleased.any()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, m_keysReleased, m_currentCombo, m_keyBindings.get()));
        m_keysReleased.reset();
    }
}
