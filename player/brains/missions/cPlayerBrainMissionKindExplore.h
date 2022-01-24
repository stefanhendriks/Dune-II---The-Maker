#pragma once

#include "player/playerh.h"
#include "cPlayerBrainMissionKind.h"

namespace brains {

    class cPlayerBrainMission;

    class cPlayerBrainMissionKindExplore : public cPlayerBrainMissionKind {

    public:

        cPlayerBrainMissionKindExplore(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKindExplore() override;

        cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) override;

        bool think_SelectTarget() override;

        void think_Execute() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindExplore"; }

        void onNotify_SpecificStateSwitch(const s_GameEvent &event) override;

    private:
        int targetCell;
    };

}
