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

void printMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], FILE *out)
{
   short x, y;

   for (y = 0; y < MAP_HEIGHT; y++)
   {
      for (x = 0; x < MAP_WIDTH; x++)
      {
		 switch (map[y][x].w)
		 {
		 case 0:
			 fputc('.', out); break;
		 case 2:
			 fputc(':', out); break;
		 case 4:
			 fputc('@', out); break;
		 case 6:
			 fputc('*', out); break;		 
		 case 8:
			 fputc('-', out); break;
		 case 9:
			 fputc('+', out); break;
		 }
      }
      fputc('\n', out);
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

void writeMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], char *name)
{
   FILE *f;

   if (f = fopen(name, "wb"))
   {
      fwrite(map, sizeof(struct cell), MAP_WIDTH*MAP_HEIGHT, f);
      fclose(f);
   }
   else
      printf("I can't write map ('%s')!\n", name);
}

int readMap(struct cell map[MAP_HEIGHT][MAP_WIDTH], char *name)
{
   FILE *f;

   if (f = fopen(name, "rb"))
   {
      fread(map, sizeof(struct cell), MAP_WIDTH*MAP_HEIGHT, f);
      fclose(f);
      return(1);
   }
   else
      printf("I can't read map ('%s')!\n", name);
   return(0);
}
