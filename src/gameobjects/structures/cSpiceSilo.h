#pragma once

#include "cAbstractStructure.h"

class cSpiceSilo : public cAbstractStructure {
public:
    cSpiceSilo();

    // void thinkFast() override;

    // void thinkSlow() override {}

    void think_animation() override;

    // void think_guard() override;

    int getType() const override;

    // void startAnimating() override {}

    // void draw() override {
    //     drawWithShadow();
    // }

    int getSpiceSiloCapacity();

    std::string getStatusForMessageBar() const override {
        return getDefaultStatusMessageBar();
    }

    void onNotifyGameEvent(const s_GameEvent &) override {}
};

