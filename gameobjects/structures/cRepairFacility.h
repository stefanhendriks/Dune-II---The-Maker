// Structure class

class cRepairFacility : public cAbstractStructure {

private:
    void think_animation_unitDeploy();

    int TIMER_repairunit;

    eAnimationDirection animDir;
public:
    cRepairFacility();

    ~cRepairFacility();

    // overloaded functions    
    void think();

    void think_animation();

    void think_guard();

    int getType() const;

    void think_repairUnit();

    void startAnimating();

};

