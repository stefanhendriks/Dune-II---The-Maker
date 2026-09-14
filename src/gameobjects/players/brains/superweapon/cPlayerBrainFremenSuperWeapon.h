/**
 * @file cPlayerBrainFremenSuperWeapon.h
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

#include "gameobjects/players/brains/cPlayerBrain.h"

namespace brains {

class cPlayerBrainFremenSuperWeapon : public cPlayerBrain {

public:
    explicit cPlayerBrainFremenSuperWeapon(cPlayer *player);

    void think() override;

    void thinkFast() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    void addBuildOrder(s_buildOrder order) override;

};

}
