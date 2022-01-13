#ifndef D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
#define D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H

#include "player/brains/cPlayerBrain.h"

namespace brains {

    class cPlayerBrainFremenSuperWeapon : public cPlayerBrain {

    public:
        cPlayerBrainFremenSuperWeapon(cPlayer *player);

        ~cPlayerBrainFremenSuperWeapon();

        void think() override;

        void thinkFast() override;

        void onNotifyGameEvent(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    };

}

#endif //D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
