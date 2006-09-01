// Structure class

class cPalace : public cStructure
{
private:
    
public:
    cPalace();
    ~cPalace();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};