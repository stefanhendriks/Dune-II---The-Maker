#ifndef D2TM_CPLAYERBRAINMISSIONKIND_DEATHHAND_H
#define D2TM_CPLAYERBRAINMISSIONKIND_DEATHHAND_H

#include "player/playerh.h"
#include "cPlayerBrainMissionKind.h"

class cBuildingListItem;

namespace brains {

    class cPlayerBrainMission;

    class cPlayerBrainMissionKindDeathHand : public cPlayerBrainMissionKind {

    public:

        cPlayerBrainMissionKindDeathHand(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKindDeathHand() override;

        cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) override;

        bool think_SelectTarget() override;

        void think_Execute() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindDeathHand"; }

        void onNotify_SpecificStateSwitch(const s_GameEvent &event) override;

    private:
        int target;
        cBuildingListItem * itemToLaunch;

        void onEventDeviated(const s_GameEvent &event);

        void onEventDestroyed(const s_GameEvent &event);

        void onMySpecialSelectTarget(const s_GameEvent &event);

        void onBuildItemCancelled(const s_GameEvent &event);
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_DEATHHAND_H
