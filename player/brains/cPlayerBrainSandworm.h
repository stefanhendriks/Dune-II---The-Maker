#pragma once

#include "cPlayerBrain.h"

namespace brains {

    class cPlayerBrainSandworm : public cPlayerBrain {

    public:
        explicit cPlayerBrainSandworm(cPlayer * player);
        ~cPlayerBrainSandworm();

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    private:
        int TIMER_think;
    };

}
