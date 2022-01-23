#pragma once

#include "cPlayerBrain.h"

namespace brains {

    class cPlayerBrainEmpty : public brains::cPlayerBrain {

    public:
        cPlayerBrainEmpty(cPlayer *player);

        ~cPlayerBrainEmpty();

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    };

}
