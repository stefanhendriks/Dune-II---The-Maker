#pragma once

#include "cAbstractStructure.h"

class cStarPort : public cAbstractStructure {
  private:
    void think_deployment();

    bool frigateDroppedPackage;

    // TIMERs
    int TIMER_deploy;

    void think_deploy();            // starport uses this to deploy unit

  public:
    cStarPort();

    // overloaded functions
    void thinkFast() override;

    void thinkSlow() override;

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

    void setFrigateDroppedPackage(bool value) { frigateDroppedPackage = value; }

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    std::string getStatusForMessageBar() const override { return getDefaultStatusMessageBar(); }

};

