#include "cKeyboard.h"
#include "cKeyboardEvent.h"
#include "utils/cIniFile.h"

#include <algorithm>

cKeyboard::cKeyboard() :
    m_keyboardObserver(nullptr),
    m_keyBindings(),
    m_keysPressed(),
    m_currentCombo()
{
}

void cKeyboard::loadKeyBindings(const cSection *section)
{
    m_keyBindings.loadDefaults();
    if (section) {
        m_keyBindings.loadFromSection(*section);
    }
}

void cKeyboard::handleEvent(const SDL_Event &event)
{
    SDL_Scancode scancode = event.key.keysym.scancode;
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        m_keysPressed.insert(scancode);
    }
    else if (event.type == SDL_KEYUP) {
        m_keysPressed.erase(scancode);
        m_keysReleased.insert(scancode);
    }

    // Read physical key state directly - more reliable on macOS
    // where the Option key may not generate KEYDOWN events due to OS-level interception
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    m_currentCombo.altPressed = keyState[SDL_SCANCODE_LALT] || keyState[SDL_SCANCODE_RALT];
    m_currentCombo.ctrlPressed = keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL];
    m_currentCombo.shiftPressed = keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT];
}

void cKeyboard::updateState()
{
    if (!m_keysPressed.empty()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, m_keysPressed, m_currentCombo, &m_keyBindings));
    }
    if (!m_keysReleased.empty()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, m_keysReleased, m_currentCombo, &m_keyBindings));
        m_keysReleased.clear();
    }
}
