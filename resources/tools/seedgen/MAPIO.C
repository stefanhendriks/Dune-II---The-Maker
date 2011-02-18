#include <stdio.h>
#include "seed.h"
#include "mapio_protos.h"

void printMatrix(char *matrix)
{
   short i;

   for (i = 0; i < 16*17; i++)
   {
      printf("%X", matrix[i]);
      if (i%16==15) putchar('\n');
   }
}

void printMap(struct cell map[64][64])
{
   short x, y;

   for (y = 0; y < 64; y++)
   {
      for (x = 0; x < 64; x++)
      {
		 switch (map[y][x].w)
		 {
		 case 0:
			 putchar('.'); break;
		 case 2:
			 putchar(':'); break;
		 case 4:
			 putchar('@'); break;
		 case 6:
			 putchar('*'); break;		 
		 case 8:
			 putchar('-'); break;
		 case 9:
			 putchar('+'); break;
		 }
      }
      putchar('\n');
   }
}

void writeMatrix(char *matrix, char *name)
{
   FILE *f;
   short i;

   if (f = fopen(name, "wb"))
   {
      for (i = 0; i <= 16*17; i++)
         fputc(matrix[i], f);
      fclose(f);
   }
   else
      printf("I can't write matrix ('%s')!\n", name);
}

int readMatrix(char *matrix, char *name)
{
   FILE *f;
   short i;

   if (f = fopen(name, "rb"))
   {
      for (i = 0; i < 16*17; i++)
         matrix[i] = fgetc(f);
      fclose(f);
      return(1);
   }
   else
      printf("I can't read matrix ('%s')!\n", name);
   return(0);
}

void writeMap(struct cell map[64][64], char *name)
{
   FILE *f;

   if (f = fopen(name, "wb"))
   {
      fwrite(map, sizeof(struct cell), 64*64, f);
      fclose(f);
   }
   else
      printf("I can't write map ('%s')!\n", name);
}

int readMap(struct cell map[64][64], char *name)
{
   FILE *f;

   if (f = fopen(name, "rb"))
   {
      fread(map, sizeof(struct cell), 64*64, f);
      fclose(f);
      return(1);
   }
   else
      printf("I can't read map ('%s')!\n", name);
   return(0);
}
