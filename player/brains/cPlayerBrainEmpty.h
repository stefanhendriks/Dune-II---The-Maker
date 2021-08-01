#ifndef D2TM_CPLAYERBRAINEMPTY_H
#define D2TM_CPLAYERBRAINEMPTY_H

#include "cPlayerBrain.h"

namespace brains {

    class cPlayerBrainEmpty : public brains::cPlayerBrain {

    public:
        cPlayerBrainEmpty(cPlayer *player);

        ~cPlayerBrainEmpty();

        void think() override;

        void onNotify(const s_GameEvent &event) override;

        void addBuildOrder(S_buildOrder order) override;

    };

}

#endif //D2TM_CPLAYERBRAINEMPTY_H
