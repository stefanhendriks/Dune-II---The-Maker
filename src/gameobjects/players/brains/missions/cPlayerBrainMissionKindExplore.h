/**
 * @file cPlayerBrainMissionKindExplore.h
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

#include "cPlayerBrainMissionKind.h"

namespace brains {

class cPlayerBrainMission;

class cPlayerBrainMissionKindExplore : public cPlayerBrainMissionKind {

public:

    cPlayerBrainMissionKindExplore(cPlayer *player, cPlayerBrainMission *mission);

    ~cPlayerBrainMissionKindExplore() override;

    cPlayerBrainMissionKind *clone(cPlayer *player, cPlayerBrainMission *mission) override;

    bool think_SelectTarget() override;

    void think_Execute() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    const char *toString() override {
        return "cPlayerBrainMissionKindExplore";
    }

    void onNotify_SpecificStateSwitch(const BuildingEvent &event) override;

private:
    int targetCell;
};

}
