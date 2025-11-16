#pragma once

#include "utils/cPoint.h"
#include <memory>

class cPlayer;
class cBounceInt;

class cLowPower {

public:
    cLowPower(cPlayer *player, cPoint &absCoords);
    ~cLowPower() = default;

    void draw();

    void thinkFast();

    static cLowPower *createFlag(cPlayer *player, cPoint &position);
private:
    cPlayer *player;
    cPoint absCoords;
    std::unique_ptr<cBounceInt> intensity;
};
