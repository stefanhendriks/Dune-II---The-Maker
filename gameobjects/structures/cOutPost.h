#pragma once

class cOutPost : public cAbstractStructure {
private:
    // outpost does not use any variables in p

public:
    cOutPost();

    ~cOutPost();

    // overloaded functions
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {};

    void draw() override { drawWithShadow(); }

    int getType() const override;

};

