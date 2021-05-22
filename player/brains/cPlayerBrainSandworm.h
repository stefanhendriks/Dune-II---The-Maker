#ifndef D2TM_CPLAYERBRAINSANDWORM_H
#define D2TM_CPLAYERBRAINSANDWORM_H

#include "cPlayerBrain.h"

namespace brains {

    class cPlayerBrainSandworm : public cPlayerBrain {

    public:
        cPlayerBrainSandworm(cPlayer * player);
        ~cPlayerBrainSandworm();

        void think();

        void onNotify(const s_GameEvent &event) override;

    private:
        int TIMER_think;
    };

}
#endif //D2TM_CPLAYERBRAINSANDWORM_H
