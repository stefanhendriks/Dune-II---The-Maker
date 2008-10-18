#ifndef STRUCTURE_FACTORY
#define STRUCTURE_FACTORY
class cStructureFactory {

private:
	cStructureFactory *instance;

public:
	cStructureFactory();

	cStructureFactory *getInstance();
	
	void deleteStructure(cStructure *structure);
	cStructure *createStructure(int type);

};

#endif