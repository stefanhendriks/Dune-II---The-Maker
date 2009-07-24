
#ifndef SEED_PROTOS_H
#define SEED_PROTOS_H

#include "seed.h"

unsigned char random (void);
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
#endif
