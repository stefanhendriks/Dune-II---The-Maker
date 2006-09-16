// Structure class

class cWor : public cStructure
{
private:
    

public:
    cWor();
    ~cWor();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

