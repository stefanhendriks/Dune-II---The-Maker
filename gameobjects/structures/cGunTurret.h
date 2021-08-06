// Structure class

class cGunTurret : public cAbstractStructure
{
private:
   int iHeadFacing;        // (for turrets only) what is this structure facing at?
   int iShouldHeadFacing;  // where should we look face at?
   int iTargetID;           // target id (assumes is always a unit)

   int TIMER_fire;
   int TIMER_turn;
   int TIMER_guard;         // timed 'area scanning'

public:
    cGunTurret();
    ~cGunTurret();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();
    void think_attack();

    int getType() const;
	int getHeadFacing() { return iHeadFacing; }

    void think_turning();
};

