/**
 * @file cFocusManager.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <SDL3/SDL.h>

class cTimeManager;

class cFocusManager {
public:
    cFocusManager(cTimeManager* timeManager);
    ~cFocusManager() = default;

    void setEnabled(bool value);
    [[nodiscard]] bool isEnabled() const;
    void onWindowFocusLost();
    void onWindowFocusGained();
    [[nodiscard]] bool isGameWindowActive() const;
private:
    bool m_enabled = false;
    bool gameWindowActive = true;
    cTimeManager* m_timeManager = nullptr;
};