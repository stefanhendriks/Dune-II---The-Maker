/**
 * @file cFocusManager.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "utils/cFocusManager.h"
#include "game/cTimeManager.h"
#include <iostream>
#include "include/cAssert.h"

cFocusManager::cFocusManager(cTimeManager* timeManager)
{
    d2tm_assert(timeManager != nullptr);
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

void cFocusManager::onWindowFocusLost()
{
    if (!m_enabled) {
        return;
    }
    gameWindowActive = false;
    m_timeManager->onWindowFocusLost();
    std::cout << "D2TM: Window focus lost" << std::endl;
}

void cFocusManager::onWindowFocusGained()
{
    if (!m_enabled) {
        return;
    }
    gameWindowActive = true;
    m_timeManager->onWindowFocusGained();
    std::cout << "D2TM: Window focus gained" << std::endl;
}

bool cFocusManager::isGameWindowActive() const
{
    return gameWindowActive;
}
