/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#ifndef STRUCTURE_FACTORY
#define STRUCTURE_FACTORY

#include <set>

class cStructureFactory {


private:
	static cStructureFactory *instance;

protected:
	cStructureFactory();
    ~cStructureFactory();

public:

	static cStructureFactory *getInstance();
	static void destroy();

	void deleteStructureInstance(cAbstractStructure *pStructure);

	cAbstractStructure *createStructure(int iCell, int iStructureType, int iPlayer, int iPercent);
	cAbstractStructure *createStructure(int iCell, int iStructureType, int iPlayer);

	int getFreeSlot();

	void deleteAllExistingStructures();

	void clearFogForStructureType(int iCell, cAbstractStructure *str);
	void clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer);

	void createSlabForStructureType(int iCell, int iStructureType);

	void updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(int iCell, int iStructureType, int iPlayer);

	int getSlabStatus(int iCell, int iStructureType);

    void slabStructure(int iCll, int iStructureType, int iPlayer);

private:
    cAbstractStructure *createStructureInstance(int type);

};
#endif
