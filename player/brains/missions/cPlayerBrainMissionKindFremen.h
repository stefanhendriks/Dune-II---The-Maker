#ifndef D2TM_CPLAYERBRAINMISSIONKIND_FREMEN_H
#define D2TM_CPLAYERBRAINMISSIONKIND_FREMEN_H

#include "player/playerh.h"
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

        void onNotify(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindFremen"; }

    private:
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_FREMEN_H
