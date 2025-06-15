#pragma once

#include "cPlayerBrainMissionKind.h"

namespace brains {

    class cPlayerBrainMission;

    class cPlayerBrainMissionKindFremen : public cPlayerBrainMissionKind {

    public:

        cPlayerBrainMissionKindFremen(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKindFremen() override;

        cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) override;

        bool think_SelectTarget() override;

        void think_Execute() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindFremen"; }

        void onNotify_SpecificStateSwitch(const s_GameEvent &event) override;
    private:
    };

}
