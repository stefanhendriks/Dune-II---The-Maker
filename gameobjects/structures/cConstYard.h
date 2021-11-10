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
    void startAnimating() {};
    void draw() override { drawWithShadow(); }

    int getType() const;
};

