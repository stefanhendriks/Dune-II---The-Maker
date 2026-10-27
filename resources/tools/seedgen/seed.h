
#ifndef SEED_H
#define SEED_H

/* Dune 2 maps are 64x64, which is the default. Other sizes (multiples of 4) can be built by defining
   MAP_WIDTH and MAP_HEIGHT at compile time, the 64x64 output stays identical. */
#ifndef MAP_WIDTH
#define MAP_WIDTH 64
#endif
#ifndef MAP_HEIGHT
#define MAP_HEIGHT 64
#endif

/* the coarse matrix holds one value per 4x4 cells, plus one extra line */
#define MATRIX_WIDTH (MAP_WIDTH / 4)
#define MATRIX_HEIGHT (MAP_HEIGHT / 4)
#define MATRIX_SIZE (MATRIX_WIDTH * (MATRIX_HEIGHT + 1))

struct cell
{
   short w, a, b;
};

#endif
