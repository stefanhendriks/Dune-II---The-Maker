#include "cell.h"

Cell::Cell()
{
}

void Cell::setTerrain(sf::Texture &terrain, Terrain terrain_type, int tile)
{
    terrainType = terrain_type;
    this->tile = tile;

    const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;
    const int TILE_SIZE = 32; // squared

    int y = TILE_SIZE * static_cast<int>(terrain_type);
    int x = TILE_SIZE * (tile%17);
    sf::IntRect rect(x, y, TILE_SIZE, TILE_SIZE);

    sprite.setTexture(terrain);
    sprite.setTextureRect(rect);
    sprite.setPosition((this->x)*TILE_SIZE,(this->y)*TILE_SIZE);
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

void Cell::setTile(int index)
{
    static const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;

    tile = (static_cast<int>(terrainType) * TILES_IN_ROW_ON_TERRAIN_SURFACE) + index;
}



