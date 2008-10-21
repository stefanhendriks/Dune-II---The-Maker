#ifndef UNIT_FACTORY
#define UNIT_FACTORY
class cUnitFactory {

private:
	cUnitFactory *instance;

public:
	cUnitFactory();

	cUnitFactory *getInstance();
	
	void deleteUnit(cAbstractUnit *unit);
	cAbstractUnit *createUnit(int type);

};

#endif