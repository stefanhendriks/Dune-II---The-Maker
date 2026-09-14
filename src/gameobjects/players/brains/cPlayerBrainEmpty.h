/**
 * @file cPlayerBrainEmpty.h
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

#include "cPlayerBrain.h"

namespace brains {

class cPlayerBrainEmpty : public brains::cPlayerBrain {

public:
    explicit cPlayerBrainEmpty(cPlayer *player);

    void think() override;

    void thinkFast() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    void addBuildOrder(s_buildOrder order) override;

};

}
