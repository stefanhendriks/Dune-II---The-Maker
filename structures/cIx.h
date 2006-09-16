// Structure class

class cIx : public cStructure
{
private:
    

public:
    cIx();
    ~cIx();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

