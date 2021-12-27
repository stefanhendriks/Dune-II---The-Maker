#pragma once

class cRefinery : public cAbstractStructure {
private:
    int iFade;          // Fading progress (windtraps)
    bool bFadeDir;      // Fading direction (TRUE -> up, FALSE -> down)

    // TIMER
    int TIMER_fade;

    void think_harvester_deploy();

public:
    cRefinery();

    ~cRefinery();

    // overloaded functions
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void think_unit_occupation();

    void startAnimating() override {};

    void draw() override { drawWithShadow(); }

    int getType() const override;

    int getSpiceSiloCapacity();

};

