// Structure class

class cHeavyFactory : public cStructure
{
private:
    
public:
    cHeavyFactory();
    ~cHeavyFactory();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

	int getType();

};

