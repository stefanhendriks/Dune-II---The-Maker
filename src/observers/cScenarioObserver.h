/**
 * @file cScenarioObserver.h
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

#pragma once

#include "sGameEvent.h"

class cScenarioObserver {
public:
    virtual ~cScenarioObserver() = default;

    virtual void onNotifyGameEvent(const s_GameEvent &event) = 0;
};
