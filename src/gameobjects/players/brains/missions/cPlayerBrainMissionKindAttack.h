/**
 * @file cPlayerBrainMissionKindAttack.h
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

#include "cPlayerBrainMissionKind.h"

namespace brains {

class cPlayerBrainMission;

class cPlayerBrainMissionKindAttack : public cPlayerBrainMissionKind {

public:

    cPlayerBrainMissionKindAttack(cPlayer *player, cPlayerBrainMission *mission);

    ~cPlayerBrainMissionKindAttack() override;

    cPlayerBrainMissionKind *clone(cPlayer *player, cPlayerBrainMission *mission) override;

    bool think_SelectTarget() override;

    void think_Execute() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    void onNotify_SpecificStateSwitch(const BuildingEvent &event) override;

    const char *toString() override {
        return "cPlayerBrainMissionKindAttack";
    }

private:
    int targetStructureID;
    int targetUnitID;

    void onEventDeviated(const CommonEvent &event);

    void onEventDestroyed(const CommonEvent &event);

    int findEnemyUnit() const;

    int findEnemyStructure() const;
};

}
