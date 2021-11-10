// Structure class

class cLightFactory : public cAbstractStructure {
private:
    void think_animation_unitDeploy();

    bool drawFlash;
    int flashes;

public:
    cLightFactory();

    ~cLightFactory();

    // overloaded functions    
    void think();

    void think_animation();

    void think_guard();

    void startAnimating();

    void draw() override;

    int getType() const;

};

