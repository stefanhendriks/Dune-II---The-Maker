#include "cSeedMap.h"

#include "data/gfxdata.h"

#include "include/cAssert.h"

cSeedMap::cSeedMap() : map(kMapWidth*kMapHeight, TERRAIN_SAND)
{
}

void cSeedMap::setCellType(int x, int y, int type)
{
    my_assert(x > -1);
    my_assert(x < kMapWidth);
    my_assert(y > -1);
    my_assert(y < kMapHeight);
    my_assert(type > -1);
    my_assert(type < TERRAIN_WALL);
    map[x + kMapWidth * y] = type;
}

int cSeedMap::getCellType(int x, int y)
{
    my_assert(x > -1);
    my_assert(x < kMapWidth);
    my_assert(y > -1);
    my_assert(y < kMapHeight);
    return map[x + kMapWidth * y];
}

char cSeedMap::getCellTypeCharacter(int x, int y)
{
    int type = getCellType(x, y);

    switch (type) {
        case TERRAIN_SAND:
            return '.';
            break;
        case TERRAIN_HILL:
            return 'H';
            break;
        case TERRAIN_SPICE:
            return '-';
            break;
        case TERRAIN_SPICEHILL:
            return '+';
            break;
        case TERRAIN_ROCK:
            return '%';
            break;
        case TERRAIN_MOUNTAIN:
            return 'R';
            break;
    }

    return 'X'; // error
}
