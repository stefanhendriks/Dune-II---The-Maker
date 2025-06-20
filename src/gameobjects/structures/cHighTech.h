#pragma once

#include "cAbstractStructure.h"
#include "enums.h"

class cHighTech : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    eAnimationDirection animDir;

public:
    cHighTech();

    // overloaded functions
    void thinkFast() override;

    void thinkSlow() override {}

    void startAnimating() override;

    void draw() override {
        drawWithShadow();
    }

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};
