
#include <stdio.h>
#include <stdlib.h>
#include "seed.h"
#include "seed_protos.h"
#include "mapio_protos.h"

extern unsigned long seed;

char   matrix[16*17+1];
struct cell map[65][64];
short  iconmap[743];
short  compact[64][64];

void writeCompact(char *name)
{
   FILE *f;
   short x, y;

   if (f = fopen(name, "wb"))
   {
      for (y = 0; y < 64; y++)
         for (x = 0; x < 64; x++)
            compact[y][x] = map[y][x].w;
      fwrite(compact, 64*64, sizeof(short), f);
      fclose(f);
   }
}

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
   /* prints map on the screen */
   printMap(map);
   /* finds edges of regions */
   scanRegions(map);
   /* converts the map to actual icon representation */
   if (f = fopen("ICON.MAP", "rb"))
   {
	  fread(iconmap, 743, 2, f);
      fclose(f);
      convertMap(map, iconmap);      
      writeCompact(argv[2]);
   }
   else
      puts("File ICON.MAP is missing: Unable to convert map!");
   return(0);
}
