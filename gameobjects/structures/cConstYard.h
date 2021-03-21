// Structure class

class cConstYard : public cAbstractStructure
{
public:
    cConstYard();
    ~cConstYard();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();

    int getType() const;
};

