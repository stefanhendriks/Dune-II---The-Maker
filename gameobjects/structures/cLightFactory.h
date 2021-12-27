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
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override;

    void draw() override;

    int getType() const override;

};

