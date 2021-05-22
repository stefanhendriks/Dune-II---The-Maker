#ifndef D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
#define D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H

#include "player/brains/cPlayerBrain.h"

namespace brains {

    class cPlayerBrainFremenSuperWeapon : public cPlayerBrain {

    public:
        cPlayerBrainFremenSuperWeapon(cPlayer *player);

        ~cPlayerBrainFremenSuperWeapon();

        void think();

        void onNotify(const s_GameEvent &event) override;

    };

}

#endif //D2TM_CPLAYERBRAINFREMENSUPERWEAPON_H
