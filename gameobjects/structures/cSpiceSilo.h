#pragma once

class cSpiceSilo : public cAbstractStructure {

private:


public:
    cSpiceSilo();

    ~cSpiceSilo();

    // overloaded functions
    void think() override;

    void think_animation() override;

    void think_guard() override;

    int getType() const override;

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getSpiceSiloCapacity();
};

