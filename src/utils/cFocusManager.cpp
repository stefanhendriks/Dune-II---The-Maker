#include "utils/cFocusManager.h"
#include "utils/cTimeManager.h"
#include <iostream>

cFocusManager::cFocusManager(cTimeManager* timeManager)
{
    // Initialize focus manager with time manager if needed
    m_timeManager = timeManager;
}

cFocusManager::~cFocusManager()
{
    // Clean up resources if needed
}

void cFocusManager::setActivateFocus(bool value)
{
    actifFocus = value;
}

bool cFocusManager::getActivateFocus() const
{
    return actifFocus;
}

bool cFocusManager::getFocus() const
{
    return hasFocus;
}

void cFocusManager::onWindowsFocus(const SDL_WindowEvent &event)
{
    if (!actifFocus) {
        return; // If focus management is not active, do nothing
    }
    switch (event.event) {
        case SDL_WINDOWEVENT_FOCUS_LOST:
            hasFocus = false;
            m_timeManager->focusLost();
            std::cout << "Window focus lost" << std::endl;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            hasFocus = true;
            m_timeManager->focusGained();
            std::cout << "Window focus gained" << std::endl;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            hasFocus = false;
            m_timeManager->focusLost();
            std::cout << "Window minimized" << std::endl;
            break;
        case SDL_WINDOWEVENT_RESTORED:
            hasFocus = true;
            m_timeManager->focusGained();
            std::cout << "Window restored" << std::endl;
            break;
    }
}