// Structure class

class cGunTurret : public cAbstractStructure
{
private:
   int iHeadFacing;        // (for turrets only) what is this structure facing at?
   int iShouldHeadFacing;  // where should we look face at?
   int iTargetID;           // target id

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

    void draw(int iStage);

	int getType();

};

