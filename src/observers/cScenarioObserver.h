#pragma once

#include "sGameEvent.h"

class cScenarioObserver {
public:
    virtual ~cScenarioObserver() = default;

    virtual void onNotifyGameEvent(const s_GameEvent &event) = 0;
};
