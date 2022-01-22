#pragma once

class cLightFactory : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    bool drawFlash;
    int flashes;

public:
    cLightFactory();

    ~cLightFactory();

    // overloaded functions    
    void thinkFast() override;

    void thinkSlow() override {}

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override;

    void draw() override;

    int getType() const override;

};

