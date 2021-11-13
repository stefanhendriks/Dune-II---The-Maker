// Structure class

class cHeavyFactory : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    bool drawFlash;
    int flashes;

    int TIMER_animation;

public:
    cHeavyFactory();

    ~cHeavyFactory();

    // overloaded functions    
    void think();

    void think_animation();

    void think_guard();

    void startAnimating();

    void draw() override;

    int getType() const;

};

