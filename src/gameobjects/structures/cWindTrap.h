#pragma once

#include "cAbstractStructure.h"

class cWindTrap : public cAbstractStructure {
private:
    int m_iFade;          // Fading progress (windtraps)
    bool m_bFadeDir;      // Fading direction (TRUE -> up, FALSE -> down)

    // TIMER
    int m_TIMER_fade;

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

    void draw() override {
        drawWithShadow();
    }

    int getType() const override;

    int getPowerOut() const;

    int getMaxPowerOut() const;

    int getFade() {
        return m_iFade;
    }

    std::string getStatusForMessageBar() const override;

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

