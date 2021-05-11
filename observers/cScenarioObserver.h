#ifndef D2TM_CSCENARIOBSERVER_H
#define D2TM_CSCENARIOBSERVER_H

#include "../include/sGameEvent.h"

class cScenarioObserver {
public:
    virtual ~cScenarioObserver();

    virtual void onNotify(const s_GameEvent &event) = 0;
};


#endif //D2TM_CSCENARIOBSERVER_H
