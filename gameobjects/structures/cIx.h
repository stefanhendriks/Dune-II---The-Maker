// Structure class

class cIx : public cAbstractStructure
{
private:
    

public:
    cIx();
    ~cIx();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    int getType() const;

};

