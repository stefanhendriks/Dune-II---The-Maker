#pragma once

#include "cGunTurret.h"

class cRocketTurret : public cGunTurret {
  public:
    cRocketTurret();

    int getType() const;
};

