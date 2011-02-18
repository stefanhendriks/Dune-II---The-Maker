
#include <stdio.h>
#include "seed.h"
#include "seed_protos.h"

#define SAND      0
#define DUNES     2
#define ROCK      4
#define MOUNTAINS 6
#define SPICE     8
#define MUCHSPICE 9

#define  A  0,0
#define  B  2,0
#define  C  4,0
#define  D  0,2
#define  E  2,2
#define  F  4,2
#define  G  0,4
#define  H  2,4
#define  I  4,4
#define	 PAIRCOUNT 21

unsigned long seed=0;

/* these are offsets used by matrix modifier */

short offsets[21] =
{ 0,-1,1,-16,16,-17,17,-15,15,-2,2,-32,32,-4,4,-64,64,-30,30,-34,34 };

/* these is array used to find corresponding points */
/*
      A·B·C    00··20··40
      ·····    ··········
      D·E·F    02··22··42
      ·····    ··········
      G·H·I    04··24··44
*/
short offsets2[PAIRCOUNT*4*2] =
{
   A,C, C,I, A,G, G,I, A,D, D,G,
   A,B, B,C, C,F, F,I, G,H, H,I,
   A,I, B,E, A,E, C,E, D,E, E,F,
   E,G, E,I, E,H,
   A,C, C,I, A,G, G,I, A,D, D,G,
   A,B, B,C, C,F, F,I, G,H, H,I,
   C,G, B,E, A,E, C,E, D,E, E,F,
   E,G, E,I, E,H
};


//Which types of terrain can contain spice
bool canAddSpiceTo[10]={
     true, //sand
     false,
     true,//dunes
     false,
     false,//rock
     false,
     false,//mountains
     false,
     true, //spice
     true  //muchspice
};


//Offsets for calculating spice positions
signed char spicemap[256] = {
0x0,0x3,0x6,0x9,0xc,0xf,0x12,0x15,0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e,
0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,0x18,0x15,0x12,0xf,0xc,0x9,0x6,0x3,
0x0,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd
};
signed char spicemap2[256] = {
0x7f,0x7e,0x7e,0x7e,0x7e,0x7e,0x7d,0x7d,0x7c,0x7b,0x7b,0x7a,0x79,0x78,0x77,0x76,
0x75,0x74,0x72,0x71,0x70,0x6e,0x6c,0x6b,0x69,0x67,0x66,0x64,0x62,0x60,0x5e,0x5b,
0x59,0x57,0x55,0x52,0x50,0x4e,0x4b,0x49,0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,
0x30,0x2d,0x2a,0x27,0x24,0x21,0x1e,0x1b,0x18,0x15,0x12,0xf,0xc,0x9,0x6,0x3,
0x0,0xfd,0xfa,0xf7,0xf4,0xf1,0xee,0xeb,0xe8,0xe5,0xe2,0xdf,0xdc,0xd9,0xd6,0xd3,
0xd0,0xcd,0xca,0xc7,0xc5,0xc2,0xbf,0xbd,0xba,0xb7,0xb5,0xb2,0xb0,0xae,0xab,0xa9,
0xa7,0xa5,0xa2,0xa0,0x9e,0x9c,0x9a,0x99,0x97,0x95,0x94,0x92,0x91,0x8f,0x8e,0x8c,
0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x85,0x84,0x83,0x83,0x82,0x82,0x82,0x82,0x82,
0x82,0x82,0x82,0x82,0x82,0x82,0x83,0x83,0x84,0x85,0x85,0x86,0x87,0x88,0x89,0x8a,
0x8b,0x8c,0x8e,0x8f,0x90,0x92,0x94,0x95,0x97,0x99,0x9a,0x9c,0x9e,0xa0,0xa2,0xa5,
0xa7,0xa9,0xab,0xae,0xb0,0xb2,0xb5,0xb7,0xba,0xbd,0xbf,0xc2,0xc5,0xc7,0xca,0xcd,
0xd0,0xd3,0xd6,0xd9,0xdc,0xdf,0xe2,0xe5,0xe8,0xeb,0xee,0xf1,0xf4,0xf7,0xfa,0xfd,
0x0,0x3,0x6,0x9,0xc,0xf,0x12,0x15,0x18,0x1b,0x1e,0x21,0x24,0x27,0x2a,0x2d,
0x30,0x33,0x36,0x39,0x3b,0x3e,0x41,0x43,0x46,0x49,0x4b,0x4e,0x50,0x52,0x55,0x57,
0x59,0x5b,0x5e,0x60,0x62,0x64,0x65,0x67,0x69,0x6b,0x6c,0x6e,0x6f,0x71,0x72,0x74,
0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7b,0x7c,0x7d,0x7d,0x7e,0x7e,0x7e,0x7e,0x7e
};

/*
 * Converts map using ICON.MAP
 * iconmap must point to this file
 */

void convertMap(struct cell map[64][64], short *iconmap)
{
   short *icontab, w, x, y;

   icontab = iconmap + iconmap[9]; /* get base of convert array */
   for (y = 0; y < 64; y++)
   {
      for (x = 0; x < 64; x++)
      {
         w = map[y][x].w;
         w = icontab[w];
         map[y][x].w = w;
         map[y][x].a = map[y][x].b = 0;
      }
   }
}

/*
 * Add spice at specified position
 */
void addSpiceAt(struct cell map[64][64],short x,short y)
{     
      short dx,dy,d,x2,y2;   
      if(map[y][x].w==SPICE){
         map[y][x].w=MUCHSPICE;
         addSpiceAt(map,x,y);   
         return;      
      }
      if(map[y][x].w==MUCHSPICE){
             for(dy=-1;dy<=1;dy++) 
             {
              y2=y+dy;
              if(y2>63) continue;
              if(y2<0) continue;
              for(dx=-1;dx<=1;dx++) 
               {                  
                  x2=x+dx;  
                  if(x2>63) continue;
                  if(x2<0) continue;             
                  if(canAddSpiceTo[map[y2][x2].w]){
                     if(map[y2][x2].w!=MUCHSPICE){
                      map[y2][x2].w=SPICE;
                     }
                  }else{
                     map[y][x].w=SPICE;
                  }
               }
             }
      }
      else{
         if(canAddSpiceTo[map[y][x].w]){
            map[y][x].w=SPICE;
         }  
      }
      
}

/*
 * Add spice to the map
 */

void addSpice(struct cell map[64][64])
{
     short i,x,y,a,b,c,d;
     short e,j,m,x2,y2,x3,y3;
     signed char k;
     
     for(i=random()&0x2F;i>0;i--)
     {  
        y=random()&0x3F;
        x=random()&0x3F;              
        if(!canAddSpiceTo[map[y][x].w]){
          i++;
          continue;
        }
        a=random()&0x1F;       
        for(b=0;b<a;b++){
            do{
                c=random()&0x3F;   
                if(c==0){
                  x3=x;
                  y3=y;
                }
                else{
                     m=c; 
                     
                     short d=((x<<6)+y);
                     
                     y2=((d & 0x3F)<<8)+0x80;
                     x2=((d << 2)&0xff00)+0x80;
                     
                     e=random()&0xff;
                     while(e>m)
                       e=e>>1;
                     m=e;
                     
                     j=random()&0xff; 
                     k=spicemap[j];     
                     x2=(((k*m)>>7)<<4)+x2;
                     
                     k=spicemap2[j];   
                     k=-k;                        
                     y2=(((k*m)>>7)<<4)+y2;   
                                         
                     if((x2>=0)&&(x2<=0x4000)&&(y2<=0x4000)&&(y2>=0)){
                        x3=(x2>>8)&0xff;
                        y3=(y2>>8)&0xff;
                     }else{
                        x3=x;
                        y3=y;
                     }       
                }                                                     
            }while((x3<0)||(y3<0)||(x3>63)||(y3>63));             
            addSpiceAt(map,x3,y3);                     
        }
     }
}

/*
 * Replaces edges of regions with special numbers
 * (so converter knows which icons to use)
 * The technique is similiar to --> balanceMap
 */

void scanRegions(struct cell map[64][64])
{
   short prevln[64], currln[64];
   short i, x, y, left, up, right, down, middle, id;

   for (i = 0; i < 64; i++)
      currln[i] = map[0][i].w;
   for (y = 0; y < 64; y++)
   {
      for (i = 0; i < 64; i++)
      {
         prevln[i] = currln[i];
         currln[i] = map[y][i].w;
      }
      for (x = 0; x < 64; x++)
      {
         middle = map[y][x].w;
         left   = currln[x-1];
         up     = prevln[x];
         right  = currln[x+1];
         down   = map[y+1][x].w;
         if (x == 0)  left  = middle;
         if (x == 63) right = middle;
         if (y == 0)  up    = middle;
         if (y == 63) down  = middle;
         id = 0;
         if (middle == ROCK)
         {
            if (up == middle || up == MOUNTAINS) id = 0x1;
            if (right == middle || right == MOUNTAINS) id |=0x2;
            if (down == middle || down == MOUNTAINS) id |= 0x4;
            if (left == middle || left == MOUNTAINS) id |= 0x8;
         }
         else if (middle == SPICE) /* this section can be removed */
         {
            if (up == middle || up == MUCHSPICE) id = 0x1;
            if (right == middle || right == MUCHSPICE) id |= 0x2;
            if (down == middle || down == MUCHSPICE) id |= 0x4;
            if (left == middle || left == MUCHSPICE) id |= 0x8;
         }
         else
         {
            if (up == middle) id = 0x1;
            if (right == middle) id |= 0x2;
            if (down == middle) id |= 0x4;
            if (left == middle) id |= 0x8;
         }
         switch (middle)
         {  //fixed offset by JPEXS
            case SAND:      id = 0; break;
            case ROCK:      id += 1; break;
            case DUNES:     id += 0x11; break;
            case MOUNTAINS: id += 0x21; break;
            case SPICE:     id += 0x31; break;
            case MUCHSPICE: id += 0x41; break;
         }
         map[y][x].w = id; /* put the calculated number in the map */
      }
   }
}

/*
 * creates terrain regions by replacing numbers within specified range
 */

void createRegions(struct cell map[64][64])
{
   /* variables named like terrains are range limits */
   short x, y, rock, dunes, mountains, num, reg;

   rock = random() & 0xF;
   if (rock < 8) rock = 8;
   else if (rock > 0xC) rock = 0xC;
   mountains = rock + 4;
   dunes = (random() & 0x3) - 1;

   for (y = 0; y < 64; y++)
   {
      for (x = 0; x < 64; x++)
      {
         num = map[y][x].w;
         reg = SAND;
         if (num > mountains)   reg = MOUNTAINS;
         else if (num >= rock)  reg = ROCK;
         else if (num <= dunes) reg = DUNES;
         map[y][x].w = reg;
      }
   }
}

/*
 * Replaces each cell with arithmetic mean of itself
 * and all eight neighbours.
 * As the new numbers are stored in the same array
 * the original line is stored in 'currln' and then copied
 * to 'prevln'.
 */

void balanceMap(struct cell map[64][64])
{
   short prevln[64], currln[64];
   short i, x, y;
   /* these variables store proper numbers (c=center, l=left etc.) */
   short c, u, ru, r, rd, d, ld, l, lu;

   for (i = 0; i < 64; i++)
      currln[i] = 0;
   for (y = 0; y < 64; y++)
   {
      for (i = 0; i < 64; i++)
      {
         prevln[i] = currln[i];
         currln[i] = map[y][i].w;
      }
      for (x = 0; x < 64; x++)
      {
         lu = prevln[x-1];      u = prevln[x];     ru = prevln[x+1];
         l  = currln[x-1];      c = currln[x];     r  = currln[x+1];
         rd = map[y+1][x+1].w;  d = map[y+1][x].w; ld = map[y+1][x-1].w;
         if (x==0)  lu = l = ld = c; /* left edge */
         if (y==0)  lu = u = ru = c; /* top edge*/
         if (x==63) ru = r = rd = c; /* right edge */
         if (y==63) ld = d = rd = c; /* bottom edge */
         map[y][x].w = (lu + u + ru + r + rd + d + ld + l + c)/9;
      }
   }
}

/*
 * "spreads" the matrix by replacing empty cells with arithmetic
 * mean of two neighbors. Offsets needed to locate neighbors
 * are stored in 'offsets2' array.
 * in this part the generator has a tend to use 65 row of the map.
 * also the right border cells are inproperly calculated.
 */

void spreadMatrix(struct cell map[64][64])
{
   short i, x, y, bx, by, ex, ey, medx, medy, diag=0, *offs;

   for (y = 0; y < 64; y += 4)
   {
      for (x = 0; x < 64; x += 4)
      {
         /* 'diag' determines which diagonal to use to fill center cell */
         if (diag) offs = offsets2; else offs = offsets2+(PAIRCOUNT*4);
         diag = 1 - diag;
         for (i = 0; i < 21; i++)
         {
            bx = x + *(offs);   by = y + *(offs+1);
            ex = x + *(offs+2); ey = y + *(offs+3);
            medx = (ex + bx)/2; medy = (ey + by)/2;
            bx &= 0x3f; ex &= 0x3f; /* so x position never exceeds 0x3F */
            if (((medy * 64) +medx) < 64*64)
               map[medy][medx].w = (map[by][bx].w + map[ey][ex].w + 1)/2;
            offs += 4;
         }
      }
   }
}

/*
 * copies matrix on map
 * the height of 65 should be used to be best compatible
 * (the generator has a tend to use that extra row in spreadMatrix :)
 */

void copyMatrix(char *matrix, struct cell map[65][64])
{
   short x, y, i=0;

   for (y = 0; y < 65; y++)
      for (x = 0; x < 64; x++)
         map[y][x].w = map[y][x].a = map[y][x].b = 0;

   for (y = 0; y < 64; y += 4)
      for (x = 0; x < 64; x += 4)
         map[y][x].w = matrix[i++];
}

/* the generation of the map begins from creation of the 16x16 matrix
 * (it actually creates 17th line but don't use it)
 * which can be interpreted as a "shape" of new map
 * Matrix is filled with random numbers (0-10)
 * and modified a bit with "addnoise" functions.
 * Further the matrix is copied on real map every four column and row
 */

void createMatrix(char *matrix)
{
   short i; 
   unsigned char num;

   for (i = 0; i < 16*17; i++)
   {
      num = random() & 0xF;
      if (num > 10) num = 10;
      matrix[i] = num;
   }
   matrix[i] = 0;
}

/*
 * this part modifies matrix a bit by creating "noisy" pools
 */

void addNoise1(char *matrix)
{
   short i, count, cell, ncell, d;

   count = random() & 0xF;
   while (count >= 0)
   {
      /* 'ncell' is the offset in matrix */
      ncell = random() & 0xFF;
      for (i = 0; i < 21; i++)
      {
         cell = ncell + offsets[i];
         if (cell < 0) cell = 0; else if (cell > 16*17) cell = 16*17;
         d = random() & 0xF;
         matrix[cell] = (matrix[cell] + d) & 0xF;
      }
      count--;
   }
}

void addNoise2(char *matrix)
{
   short i, count, cell, ncell;

   count = random() & 0x3;
   while (count >= 0)
   {
      ncell = random() & 0xFF;
      for (i = 0; i < 21; i++)
      {
         cell = ncell + offsets[i];
         if (cell < 0) cell = 0; else if (cell > 16*17) cell = 16*17;
         matrix[cell] = random() & 0x3;
      }
      count--;
   }
}
