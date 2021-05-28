#ifndef D2TM_CPLAYERBRAINMISSIONKIND_ATTACK_H
#define D2TM_CPLAYERBRAINMISSIONKIND_ATTACK_H

#include "player/playerh.h"
#include "cPlayerBrainMissionKind.h"

namespace brains {

    class cPlayerBrainMission;

    class cPlayerBrainMissionKindAttack : public cPlayerBrainMissionKind {

    public:

        cPlayerBrainMissionKindAttack(cPlayer *player, cPlayerBrainMission * mission);

        ~cPlayerBrainMissionKindAttack() override;

        cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) override;

        void think_SelectTarget() override;

        void think_Execute() override;

        void onNotify(const s_GameEvent &event) override;

    private:
        int targetStructureID;
        int targetUnitID;

        void onEventDeviated(const s_GameEvent &event);

        void onEventDestroyed(const s_GameEvent &event);
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_ATTACK_H
