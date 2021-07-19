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

struct s_PlaceResult {
    bool success = false; // if true, all is ok

    // else, these are one of the reasons why it was not a success
    bool badTerrain = false; // not valid to place

    bool outOfBounds = false; // (one of the cells) is out of bounds

    // these units block it
    std::set<int> unitIds = std::set<int>();

    // these structures block it
    std::set<int> structureIds = std::set<int>();
};

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
    s_PlaceResult canPlaceStructureAt(int iCell, int iStructureType, int iUnitIDToIgnore);
    s_PlaceResult canPlaceStructureAt(int iCell, int iStructureType);

    void slabStructure(int iCll, int iStructureType, int iPlayer);

private:
    cAbstractStructure *createStructureInstance(int type);

};
#endif
