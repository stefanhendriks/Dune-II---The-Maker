#pragma once

#include "cAbstractStructure.h"

class cRefinery : public cAbstractStructure {
  private:
    void think_harvester_deploy();

  public:
    cRefinery();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void think_unit_occupation();

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;

    int getSpiceSiloCapacity();

    std::string getStatusForMessageBar() const override { return getDefaultStatusMessageBar(); }
};

