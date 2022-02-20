#pragma once

#include "cAbstractStructure.h"

class cWindTrap : public cAbstractStructure {
private:
    int iFade;          // Fading progress (windtraps)
    bool bFadeDir;      // Fading direction (TRUE -> up, FALSE -> down)

    // TIMER
    int TIMER_fade;

    // windtrap specific animation:
    void think_fade();

public:
    cWindTrap();

    ~cWindTrap();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;

    int getPowerOut() const;

    int getMaxPowerOut() const;

    int getFade() { return iFade; }

    std::string getStatusForMessageBar() const override;
};

