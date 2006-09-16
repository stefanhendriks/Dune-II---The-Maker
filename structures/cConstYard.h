// Structure class

class cConstYard : public cStructure
{
public:
    cConstYard();
    ~cConstYard();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

