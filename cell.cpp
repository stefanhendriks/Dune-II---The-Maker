#include "cell.h"

Cell::Cell()
{
}

void Cell::init(const sf::Texture &terrain, const sf::Texture &shrouded_edges, int row, int col)
{
    y = col;
    x = row;

    sprite.setTexture(terrain);
    sprite.setPosition(row*TILE_SIZE,col*TILE_SIZE);

    sprite_shroud.setTexture(shrouded_edges);
    sprite_shroud.setPosition(row*TILE_SIZE,col*TILE_SIZE);

    shrouded = true;
}

void Cell::setType(Terrain terrain_type)
{
    terrainType = terrain_type;
}

void Cell::setIndex(int tileIndex) {
    if (tileIndex < 0) return;
    int tileRow = TILE_SIZE * static_cast<int>(terrainType);
    int tileCol = TILE_SIZE * tileIndex;

    sprite.setTextureRect({tileCol, tileRow, TILE_SIZE, TILE_SIZE});
}

void Cell::setShroudIndex(int tileIndex) {
    int tileCol = TILE_SIZE * tileIndex;
    sf::IntRect rect(tileCol, 0, TILE_SIZE, TILE_SIZE);

    sprite_shroud.setTextureRect(rect);
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

void Cell::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(sprite);
}

void Cell::drawShrouded(sf::RenderTarget &target, sf::RenderStates states) const {
  target.draw(sprite_shroud);
}


