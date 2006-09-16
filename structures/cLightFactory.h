// Structure class

class cLightFactory : public cStructure
{
private:
    
public:
    cLightFactory();
    ~cLightFactory();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

