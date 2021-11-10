// Structure class

class cPalace : public cAbstractStructure
{
private:
    
public:
    cPalace();
    ~cPalace();
  
    // overloaded functions    
    void think();
    void think_animation();
    void think_guard();
    void startAnimating() {};
    void draw() override { drawWithShadow(); }

    int getType() const;

};

