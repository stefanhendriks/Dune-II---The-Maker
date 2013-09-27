#include "cell.h"

Cell::Cell()
{
}

void Cell::init(const sf::Texture &terrain, int row, int col)
{
    y = col;
    x = row;

    sprite.setTexture(terrain);
    sprite.setPosition(row*TILE_SIZE,col*TILE_SIZE);
}

void Cell::setType(Terrain terrain_type)
{
    terrainType = terrain_type;
}

void Cell::setIndex(int tileIndex)
{
    int tileRow = TILE_SIZE * static_cast<int>(terrainType);
    int tileCol = TILE_SIZE * tileIndex;
    sf::IntRect rect(tileCol, tileRow, TILE_SIZE, TILE_SIZE);

    sprite.setTextureRect(rect);
}

bool Cell::shouldSmoothWithTerrainType(Cell *other)
{
    if (terrainType == Terrain::Rock) {
        return ((other->terrainType != Terrain::Mountain) &&
                (other->terrainType != Terrain::Rock) &&
                (other->terrainType != Terrain::Slab));
    }
    if (terrainType == Terrain::Mountain) {
        return (other->terrainType != Terrain::Mountain);
    }
    if (terrainType == Terrain::Slab) {
        return ((other->terrainType != Terrain::Mountain) &&
                (other->terrainType != Terrain::Rock));
    }
    if (terrainType == Terrain::Spice) {
        return ((other->terrainType != Terrain::Spice) &&
                (other->terrainType != Terrain::Spicehill));
    }
    if (terrainType == Terrain::Spicehill) {
        return (other->terrainType != Terrain::Spicehill);
    }
    return (other->terrainType != terrainType);
}


