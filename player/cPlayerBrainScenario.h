#ifndef D2TM_CPLAYERBRAINSCENARIO_H
#define D2TM_CPLAYERBRAINSCENARIO_H

#include "cPlayerBrain.h"

class cPlayerBrainScenario : public cPlayerBrain {

public:
    cPlayerBrainScenario(cPlayer * player);
    ~cPlayerBrainScenario();

    void think();

private:
    int TIMER_think;
};


#endif //D2TM_CPLAYERBRAINSCENARIO_H
