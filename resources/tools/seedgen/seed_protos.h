
#ifndef SEED_PROTOS_H
#define SEED_PROTOS_H

#include "seed.h"

unsigned char random (void);
void convertMap (struct cell map[MAP_HEIGHT][MAP_WIDTH], short *iconmap);
void addSpice(struct cell map[MAP_HEIGHT][MAP_WIDTH]);
void scanRegions (struct cell map[MAP_HEIGHT][MAP_WIDTH]);
void createRegions (struct cell map[MAP_HEIGHT][MAP_WIDTH]);
void balanceMap (struct cell map[MAP_HEIGHT][MAP_WIDTH]);
void spreadMatrix (struct cell map[MAP_HEIGHT][MAP_WIDTH]);
void copyMatrix (char *matrix, struct cell map[MAP_HEIGHT + 1][MAP_WIDTH]);
void createMatrix (char *matrix);
void addNoise1 (char *matrix);
void addNoise2 (char *matrix);
#endif
