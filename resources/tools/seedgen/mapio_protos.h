#ifndef MAPIO_PROTOS_H
#define MAPIO_PROTOS_H

#include <stdio.h>
#include "seed.h"

void printMatrix (char *matrix);
void writeMatrix (char *matrix, char *name);
int readMatrix (char *matrix, char *name);
void printMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], FILE *out);
void writeMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], char *name);
int readMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], char *name);

#endif
