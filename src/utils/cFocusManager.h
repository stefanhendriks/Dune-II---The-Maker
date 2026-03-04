#pragma once

#include <SDL2/SDL.h>

class cTimeManager;

class cFocusManager {
public:
    cFocusManager(cTimeManager* timeManager);
    ~cFocusManager() = default;

    void setEnabled(bool value);
    [[nodiscard]] bool isEnabled() const;
    void onWindowsFocus(const SDL_WindowEvent& event);
    [[nodiscard]] bool isGameWindowActive() const;
private:
    bool m_enabled = false;
    bool gameWindowActive = true;
    cTimeManager* m_timeManager = nullptr;
};