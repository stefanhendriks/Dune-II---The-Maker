// Structure class

class cSpiceSilo : public cStructure
{
private:
   

public:
    cSpiceSilo();
    ~cSpiceSilo();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

