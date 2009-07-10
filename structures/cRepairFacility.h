// Structure class

class cRepairFacility : public cStructure
{
private:
    
    int TIMER_repairunit;

public:
    cRepairFacility();
    ~cRepairFacility();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

	int getType();

};

