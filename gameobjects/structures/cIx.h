#pragma once

class cIx : public cAbstractStructure {
private:


public:
    cIx();

    ~cIx();

    // overloaded functions    
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {};

    void draw() override { drawWithShadow(); }

    int getType() const override;

};

