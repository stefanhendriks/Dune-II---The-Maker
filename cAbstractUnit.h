#ifndef CABSTRUNIT
#define CABSTRUNIT

class cAbstractUnit { 

public:
	cAbstractUnit();

	virtual void think() = 0;
	virtual void die() = 0;

private:
	float fExperience;	// experience gained by unit
	bool bSelected;     // selected or not?
    int iCell;          // cell / location of unit
	int iPlayer;		// to what player (id) does this unit belong?
	int iGroup;			// to what group (made with CTRL-#) does this unit belong?

};



#endif