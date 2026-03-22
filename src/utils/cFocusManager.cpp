#include "utils/cFocusManager.h"
#include "game/cTimeManager.h"
#include <iostream>
#include <cassert>

cFocusManager::cFocusManager(cTimeManager* timeManager)
{
    assert(timeManager != nullptr);
    // Initialize focus manager with time manager if needed
    m_timeManager = timeManager;
}

void cFocusManager::setEnabled(bool value)
{
    m_enabled = value;
}

bool cFocusManager::isEnabled() const
{
    return m_enabled;
}

void cFocusManager::onWindowsFocus(const SDL_WindowEvent &event)
{
    if (!m_enabled) {
        return; // If focus management is not active, do nothing
    }
    switch (event.event) {
        case SDL_WINDOWEVENT_FOCUS_LOST:
            gameWindowActive = false;
            m_timeManager->onWindowFocusLost();
            std::cout << "D2TM: Window focus lost" << std::endl;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            gameWindowActive = false;
            m_timeManager->onWindowFocusLost();
            std::cout << "D2TM: Window minimized" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            gameWindowActive = true;
            m_timeManager->onWindowFocusGained();
            std::cout << "D2TM: Window focus gained" << std::endl;
            break;
        case SDL_WINDOWEVENT_RESTORED:
            gameWindowActive = true;
            m_timeManager->onWindowFocusGained();
            std::cout << "D2TM: Window restored" << std::endl;
            break;
        default:
            // ignore other events
            break;
    }
}

bool cFocusManager::isGameWindowActive() const
{
    return gameWindowActive;
}