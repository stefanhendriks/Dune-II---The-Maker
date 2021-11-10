// Structure class

class cHeavyFactory : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

public:
    cHeavyFactory();

    ~cHeavyFactory();

    // overloaded functions    
    void think();

    void think_animation();

    void think_guard();

    void startAnimating() {};

    void draw() override { drawWithShadow(); }

    int getType() const;

};

