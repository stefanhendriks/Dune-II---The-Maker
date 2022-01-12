#ifndef D2TM_CPLAYERBRAINSANDWORM_H
#define D2TM_CPLAYERBRAINSANDWORM_H

#include "cPlayerBrain.h"

namespace brains {

    class cPlayerBrainSandworm : public cPlayerBrain {

    public:
        cPlayerBrainSandworm(cPlayer * player);
        ~cPlayerBrainSandworm();

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    private:
        int TIMER_think;
    };

}
#endif //D2TM_CPLAYERBRAINSANDWORM_H
