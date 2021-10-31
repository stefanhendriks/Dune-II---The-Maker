// Structure class

class cStarPort : public cAbstractStructure
{
private:
	void think_deployment();

	bool frigateDroppedPackage;

    // TIMERs
    int TIMER_deploy;

public:
    cStarPort();
    ~cStarPort();

    // overloaded functions
    void think();
    void think_animation();
    void think_deploy();            // starport uses this to deploy unit
    void think_guard();

    int getType() const;

	bool isFrigatePackageDropped() { return frigateDroppedPackage; }
	void setFrigateDroppedPackage (bool value) { frigateDroppedPackage = value; }

    void startAnimating() {};

};

