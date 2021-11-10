// Structure class

class cSpiceSilo : public cAbstractStructure
{
private:


public:
    cSpiceSilo();
    ~cSpiceSilo();

    // overloaded functions
    void think();
    void think_animation();
    void think_guard();

    int getType() const;
    void startAnimating() {};
    void draw() override { drawWithShadow(); }

	int getSpiceSiloCapacity();
};

