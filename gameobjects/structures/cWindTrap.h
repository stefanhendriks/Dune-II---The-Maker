// Structure class

class cWindTrap : public cAbstractStructure
{
private:
    int iFade;          // Fading progress (windtraps)
    bool bFadeDir;      // Fading direction (TRUE -> up, FALSE -> down)

    // TIMER
    int TIMER_fade;

	// windtrap specific animation:
	void think_fade();

public:
    cWindTrap();
    ~cWindTrap();

    // overloaded functions
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);
	int getType();

	int getPowerOut();
	int getMaxPowerOut();
};

