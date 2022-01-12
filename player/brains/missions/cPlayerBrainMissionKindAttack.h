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

        bool think_SelectTarget() override;

        void think_Execute() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void onNotify_SpecificStateSwitch(const s_GameEvent &event) override;

        const char *toString() override { return "cPlayerBrainMissionKindAttack"; }

    private:
        int targetStructureID;
        int targetUnitID;

        void onEventDeviated(const s_GameEvent &event);

        void onEventDestroyed(const s_GameEvent &event);

        int findEnemyUnit() const;

        int findEnemyStructure() const;
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_ATTACK_H
