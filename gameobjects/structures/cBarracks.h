#pragma once

#include "cAbstractStructure.h"

class cBarracks : public cAbstractStructure {
  public:
    cBarracks();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;
};

