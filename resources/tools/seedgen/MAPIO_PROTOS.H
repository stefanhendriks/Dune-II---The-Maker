#ifndef MAPIO_PROTOS_H
#define MAPIO_PROTOS_H

#include "seed.h"

void printMatrix (char *matrix);
void writeMatrix (char *matrix, char *name);
int readMatrix (char *matrix, char *name);
void printMap(struct cell map[64][64]);
void writeMap(struct cell map[64][64], char *name);
int readMap(struct cell map[64][64], char *name);

#endif
