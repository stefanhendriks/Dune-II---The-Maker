#include "cKeyboard.h"
#include "cKeyBindings.h"
#include "cKeyboardEvent.h"
#include "utils/cIniFile.h"

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
    SDL_Scancode scancode = event.key.keysym.scancode;
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        m_keysPressed.insert(scancode);
    }
    else if (event.type == SDL_KEYUP) {
        m_keysPressed.erase(scancode);
        m_keysReleased.insert(scancode);
    }

    m_currentCombo.altPressed   = m_keysPressed.count(SDL_SCANCODE_LALT)   || m_keysPressed.count(SDL_SCANCODE_RALT);
    m_currentCombo.ctrlPressed  = m_keysPressed.count(SDL_SCANCODE_LCTRL)  || m_keysPressed.count(SDL_SCANCODE_RCTRL);
    m_currentCombo.shiftPressed = m_keysPressed.count(SDL_SCANCODE_LSHIFT) || m_keysPressed.count(SDL_SCANCODE_RSHIFT);
}

void cKeyboard::updateState()
{
    if (!m_keysPressed.empty()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, m_keysPressed, m_currentCombo, m_keyBindings.get()));
    }
    if (!m_keysReleased.empty()) {
        m_keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, m_keysReleased, m_currentCombo, m_keyBindings.get()));
        m_keysReleased.clear();
    }
}
