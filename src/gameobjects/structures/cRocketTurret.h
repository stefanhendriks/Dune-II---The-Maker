#pragma once

#include "cGunTurret.h"

class cRocketTurret : public cGunTurret {
  public:
    cRocketTurret();

    int getType() const override;

    std::string getStatusForMessageBar() const override { return getDefaultStatusMessageBar(); }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

