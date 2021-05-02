#ifndef D2TM_CPLAYERBRAINSANDWORM_H
#define D2TM_CPLAYERBRAINSANDWORM_H

#include "cPlayerBrain.h"

class cPlayerBrainSandworm : public cPlayerBrain {

public:
    cPlayerBrainSandworm(cPlayer * player);
    ~cPlayerBrainSandworm();

    void think();

private:
    int TIMER_think;
};


#endif //D2TM_CPLAYERBRAINSANDWORM_H
