// Structure class

class cRocketTurret : public cStructure
{
private:    
   int iHeadFacing;        // (for turrets only) what is this structure facing at?
   int iShouldHeadFacing;  // where should we look face at?
   int iTargetID;           // target id

   int TIMER_fire;
   int TIMER_turn;
   int TIMER_guard;         // timed 'area scanning'

public:
    cRocketTurret();
    ~cRocketTurret();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

	int getType();

};

