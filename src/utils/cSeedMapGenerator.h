#ifndef SEEDMAP_GENERATOR
#define SEEDMAP_GENERATOR

struct cell
{
   short w, a, b;
};

#define SMG_SAND      0
#define SMG_DUNES     2
#define SMG_ROCK      4
#define SMG_MOUNTAINS 6
#define SMG_SPICE     8
#define SMG_MUCHSPICE 9

// needed for offset2
#define  SMG_A  0,0
#define  SMG_B  2,0
#define  SMG_C  4,0
#define  SMG_D  0,2
#define  SMG_E  2,2
#define  SMG_F  4,2
#define  SMG_G  0,4
#define  SMG_H  2,4
#define  SMG_I  4,4
#define	 SMG_PAIRCOUNT 21

#define SMG_OFFSET2_SIZE SMG_PAIRCOUNT*4*2

class cSeedMapGenerator {

private:

	unsigned long seed;

	short random();	// create random number

	/* these is array used to find corresponding points */
	/*
	      A·B·C    00··20··40
	      ·····    ··········
	      D·E·F    02··22··42
	      ·····    ··········
	      G·H·I    04··24··44
	*/
	static short offsets2[SMG_OFFSET2_SIZE];
	static short offsets[21];
	static bool canAddSpiceTo[10];
	static signed char spicemap[256];
	static signed char spicemap2[256];

	char   matrix[16*17+1];
	struct cell map[65][64];
	short  compact[64][64];

	// methods needed to generate seedmap
	void convertMap (struct cell map[64][64], short *iconmap);
	void addSpice(struct cell map[64][64]);
	void scanRegions (struct cell map[64][64]);
	void createRegions (struct cell map[64][64]);
	void balanceMap (struct cell map[64][64]);
	void spreadMatrix (struct cell map[64][64]);
	void copyMatrix (char *matrix, struct cell map[65][64]);
	void createMatrix (char *matrix);
	void addNoise1 (char *matrix);
	void addNoise2 (char *matrix);
	void addSpiceAt(struct cell map[64][64],short x,short y);

public:
	cSeedMapGenerator();
	cSeedMapGenerator(unsigned long value);

	cSeedMap *generateSeedMap();
};

#endif
