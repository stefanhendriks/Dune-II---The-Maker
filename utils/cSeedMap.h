#ifndef SEEDMAP
#define SEEDMAP

/**
 * Representation of terrain, called a seed map. This is not per-see generated from seed info. But
 * for now it is used to get the generated seed map into the Map class by D2TM.
 *
 */
class cSeedMap {

private:
	int map[64][64];

public:
	cSeedMap();

	void setCellType(int x, int y, int type);
	int getCellType(int x, int y);
	char getCellTypeCharacter(int x, int y);

};

#endif
