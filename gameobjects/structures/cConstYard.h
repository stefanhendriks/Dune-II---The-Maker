#pragma once

#include "cAbstractStructure.h"

class cConstYard : public cAbstractStructure {
public:
    cConstYard();

    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;
};

