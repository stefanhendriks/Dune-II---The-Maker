// Structure class

class cRepairFacility : public cAbstractStructure {

private:

    int TIMER_repairunit;

public:
    cRepairFacility();

    ~cRepairFacility();

    // overloaded functions    
    void think();

    void think_animation();

    void think_guard();

    int getType() const;

    void think_repairUnit();
};

