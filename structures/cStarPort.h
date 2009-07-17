// Structure class

class cStarPort : public cAbstractStructure
{
private:
	void think_deployment();

public:
    cStarPort();
    ~cStarPort();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_deploy();            // starport uses this to deploy unit
    void think_guard();

    void draw(int iStage);

	int getType();

    // TIMERs
    int TIMER_deploy;
};

