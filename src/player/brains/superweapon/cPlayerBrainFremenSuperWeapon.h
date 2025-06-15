#pragma once

#include "player/brains/cPlayerBrain.h"

namespace brains {

    class cPlayerBrainFremenSuperWeapon : public cPlayerBrain {

    public:
        explicit cPlayerBrainFremenSuperWeapon(cPlayer *player);

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    };

}
