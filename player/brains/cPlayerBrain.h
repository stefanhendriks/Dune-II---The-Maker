#ifndef D2TM_CPLAYERBRAIN_H
#define D2TM_CPLAYERBRAIN_H

#include <observers/cScenarioObserver.h>
#include "cPlayerBrainData.h"

class cPlayer;
class cAbstractStructure;

namespace brains {

    class cPlayerBrain : public cScenarioObserver {

    public:
        const static int RestTime = 10;

        cPlayerBrain(cPlayer *player);

        virtual ~cPlayerBrain() = 0;

        virtual void think() = 0;

        virtual void addBuildOrder(S_buildOrder order) = 0;

    protected:
        cPlayer *player;

    private:


    };
}

#endif //D2TM_CPLAYERBRAIN_H
