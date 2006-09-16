// Structure class

class cBarracks : public cStructure
{
private:
  

public:
    cBarracks();
    ~cBarracks();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

