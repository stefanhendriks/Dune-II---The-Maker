// Structure class

class cLightFactory : public cAbstractStructure
{
private:
	void think_animation_unitDeploy();
    
public:
    cLightFactory();
    ~cLightFactory();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

	int getType();

};

