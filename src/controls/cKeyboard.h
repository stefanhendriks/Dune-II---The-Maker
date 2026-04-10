#pragma once

#include "controls/eKeyboardEnum.h"
#include "observers/cInputObserver.h"
#include <SDL2/SDL.h>
#include <memory>
#include <set>

class cKeyBindings;
class cSection;

class cKeyboard {
public:
    cKeyboard();
    ~cKeyboard();
    void handleEvent(const SDL_Event &event);
    void updateState();

    void setKeyboardObserver(cInputObserver *keyboardObserver) {
        m_keyboardObserver = keyboardObserver;
    }

    /**
     * Load default key bindings, then optionally override from an INI section.
     * Pass nullptr to use defaults only.
     */
    void loadKeyBindings(const cSection *section = nullptr);

private:
    cInputObserver *m_keyboardObserver;
    std::unique_ptr<cKeyBindings> m_keyBindings;
    std::set<SDL_Scancode> m_keysPressed;
    std::set<SDL_Scancode> m_keysReleased;
    s_KeysCombo m_currentCombo;
};
