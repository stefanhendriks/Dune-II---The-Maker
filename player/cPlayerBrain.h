//
// Created by shendriks on 5/1/2021.
//

#ifndef D2TM_CPLAYERBRAIN_H
#define D2TM_CPLAYERBRAIN_H


#include <observers/cScenarioObserver.h>

class cPlayer;
class cAbstractStructure;

class cPlayerBrain : public cScenarioObserver {

public:
    cPlayerBrain(cPlayer * player);
    virtual ~cPlayerBrain() = 0;

    virtual void think() = 0;

protected:
    cPlayer *player;

private:


};


#endif //D2TM_CPLAYERBRAIN_H
