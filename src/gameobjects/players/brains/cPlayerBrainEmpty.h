/**
 * @file cPlayerBrainEmpty.h
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
