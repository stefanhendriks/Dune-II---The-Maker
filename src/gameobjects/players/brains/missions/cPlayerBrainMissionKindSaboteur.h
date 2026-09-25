/**
 * @file cPlayerBrainMissionKindSaboteur.h
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
#include "gameobjects/players/cPlayer.h"

namespace brains {

class cPlayerBrainMission;

class cPlayerBrainMissionKindSaboteur : public cPlayerBrainMissionKind {

public:

    cPlayerBrainMissionKindSaboteur(cPlayer *player, cPlayerBrainMission *mission);

    ~cPlayerBrainMissionKindSaboteur() override;

    cPlayerBrainMissionKind *clone(cPlayer *player, cPlayerBrainMission *mission) override;

    bool think_SelectTarget() override;

    void think_Execute() override;

    void onNotifyGameEvent(const s_GameEvent &event) override;

    const char *toString() override {
        return "cPlayerBrainMissionKindSaboteur";
    }

    void onNotify_SpecificStateSwitch(const BuildingEvent &event) override;

private:
    int targetStructureID;

    void onEventDestroyed(const CommonEvent &event);
};

}
