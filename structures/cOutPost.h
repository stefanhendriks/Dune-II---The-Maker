// Structure class

class cOutPost : public cStructure
{
private:
  // outpost does not use any variables in p

public:
    cOutPost();
    ~cOutPost();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    void draw(int iStage);

};

