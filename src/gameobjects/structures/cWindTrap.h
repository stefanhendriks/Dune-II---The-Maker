#pragma once

#include "cAbstractStructure.h"

class cWindTrap : public cAbstractStructure {
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

    std::string getStatusForMessageBar() const override;

    void onNotifyGameEvent(const s_GameEvent &) override {}
private:
    bool m_flagDir = true;      // Fading direction (TRUE -> up, FALSE -> down)
};

