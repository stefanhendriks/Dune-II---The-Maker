// Structure class

class cRefinery : public cStructure
{
private:
    int iFade;          // Fading progress (windtraps)
    bool bFadeDir;      // Fading direction (TRUE -> up, FALSE -> down)
    
    // TIMER
    int TIMER_fade;

public:
    cRefinery();
    ~cRefinery();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

	int getType();

};

