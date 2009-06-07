#ifndef UNIT_FACTORY
#define UNIT_FACTORY
class cUnitFactory {

private:
	static cUnitFactory *instance;

protected:
	cUnitFactory();

public:

	static cUnitFactory *getInstance();
	
	void deleteUnit(cAbstractUnit *unit);
	cAbstractUnit *createUnit(int type);

};

#endif