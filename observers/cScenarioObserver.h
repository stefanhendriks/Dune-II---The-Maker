#ifndef D2TM_CSCENARIOBSERVER_H
#define D2TM_CSCENARIOBSERVER_H

#include "../include/sGameEvent.h"

class cScenarioObserver {
public:
    virtual ~cScenarioObserver() = default;

    virtual void onNotifyGameEvent(const s_GameEvent &event) = 0;
};


#endif //D2TM_CSCENARIOBSERVER_H
