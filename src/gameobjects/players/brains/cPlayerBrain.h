/**
 * @file cPlayerBrain.h
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

#include "cPlayerBrainData.h"
#include "observers/cScenarioObserver.h"

class cPlayer;
class cAbstractStructure;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cGameInterface;
struct sGameServices;

namespace {

constexpr int kScanRadius = 20; // hard-coded for now, make player property later?

}

namespace brains {

class cPlayerBrain : public cScenarioObserver {

public:
    // AI brain tick is every 100ms, so rest-time of 10 means resting 1 second.
    const static int RestTime = 10;

    explicit cPlayerBrain(cPlayer *player);

    virtual ~cPlayerBrain() = default;

    virtual void serviceInit(sGameServices* services);

    /**
     * called, every 100 ms
     */
    virtual void think() = 0;

    /**
     * called, every 5 ms
     */
    virtual void thinkFast() = 0;

    virtual void addBuildOrder(s_buildOrder order) = 0;

protected:
    cPlayer *player;
    cGameObjectContext *m_objects = nullptr;
    cInfoContext *m_info = nullptr;
    cGameSettings *m_settings = nullptr;
    cGameInterface *m_interface = nullptr;

private:


};
}
