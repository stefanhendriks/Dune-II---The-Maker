// Structure class
#ifndef CHIGHTECH_H
#define CHIGHTECH_H

enum eAnimationDirection {
    ANIM_NONE, // not animating
    ANIM_OPEN, // open the dome
    ANIM_SPAWN_UNIT, // this is where we actually should spawn the unit... (which we don't)
    ANIM_CLOSE, // now close the dome
};

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

    void think_animation();

    void think_guard();

    int getType() const;

};

#endif