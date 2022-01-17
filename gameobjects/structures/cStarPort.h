#pragma once

class cStarPort : public cAbstractStructure {
private:
    void think_deployment();

    bool frigateDroppedPackage;

    // TIMERs
    int TIMER_deploy;

public:
    cStarPort();

    ~cStarPort();

    // overloaded functions
    void think() override;

    void think_animation() override;

    void think_deploy();            // starport uses this to deploy unit
    void think_guard() override;

    int getType() const override;

    void setFrigateDroppedPackage(bool value) { frigateDroppedPackage = value; }

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

};

