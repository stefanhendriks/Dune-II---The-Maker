#pragma once

class cPalace : public cAbstractStructure {
private:

public:
    cPalace();

    ~cPalace();

    // overloaded functions    
    void think() override;

    void think_animation() override;

    void think_guard() override;

    void startAnimating() override {}

    void draw() override { drawWithShadow(); }

    int getType() const override;

};

