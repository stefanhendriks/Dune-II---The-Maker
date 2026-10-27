
#include <stdio.h>
#include <stdlib.h>
#include "seed.h"
#include "seed_protos.h"
#include "mapio_protos.h"

extern unsigned long seed;

char   matrix[MATRIX_SIZE + 1];
struct cell map[MAP_HEIGHT + 1][MAP_WIDTH];

int main(int argc, char **argv)
{
   FILE *f;

   if (argc < 3)
   {
      printf("USAGE: %s <seed> <name>\n", argv[0]);
      return(0);
   }
   seed = atoi(argv[1]);

   /* creates the shape of the map */
   createMatrix(matrix);
   addNoise1(matrix);
   addNoise2(matrix);

   /* copies the matrix on the map and spreads the numbers */
   copyMatrix(matrix, map);
   spreadMatrix(map);
   /* makes the map more smooth */
   balanceMap(map);   
   /* creates the terrain regions out of numbers */
   createRegions(map);   
   /* adds spice to the map (Coded by JPEXS) */
   addSpice(map);   
   /* prints map on the screen and writes the same text to the file */
   printMap(map, stdout);
   if (f = fopen(argv[2], "w"))
   {
      printMap(map, f);
      fclose(f);
   }
   else
      printf("Unable to write %s\n", argv[2]);
   return(0);
}
