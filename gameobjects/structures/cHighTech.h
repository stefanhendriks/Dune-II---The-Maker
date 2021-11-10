// Structure class
#ifndef CHIGHTECH_H
#define CHIGHTECH_H

class cHighTech : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    eAnimationDirection animDir;

public:
    cHighTech();

    ~cHighTech();

    // overloaded functions    
    void think();

    void startAnimating() override;

    void draw() override { drawWithShadow(); }

    void think_animation();

    void think_guard();

    int getType() const;


};

#endif