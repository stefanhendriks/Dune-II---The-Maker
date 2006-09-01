// Structure class

class cHighTech : public cStructure
{
private:
    
public:
    cHighTech();
    ~cHighTech();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};