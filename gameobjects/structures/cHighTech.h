#pragma once

class cHighTech : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    eAnimationDirection animDir;

public:
    cHighTech();

    ~cHighTech();

    // overloaded functions    
    void thinkFast() override;

    void thinkSlow() override {}

    void startAnimating() override;

    void draw() override { drawWithShadow(); }

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

};
