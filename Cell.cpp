#include "Cell.hpp"
#include <cassert>

Cell::Cell():
  terrainType(Terrain::Sand),
  shrouded(true),
  x(0), y(0)
{
}

void Cell::init(int row, int col)
{
  y = col;
  x = row;

  vertices[0].position = sf::Vector2f(row*TILE_SIZE,col*TILE_SIZE);
  vertices[1].position = sf::Vector2f((row+1)*TILE_SIZE,col*TILE_SIZE);
  vertices[2].position = sf::Vector2f((row+1)*TILE_SIZE,(col+1)*TILE_SIZE);
  vertices[3].position = sf::Vector2f(row*TILE_SIZE,(col+1)*TILE_SIZE);

  shroudVertices[0].position = sf::Vector2f(row*TILE_SIZE,col*TILE_SIZE);
  shroudVertices[1].position = sf::Vector2f((row+1)*TILE_SIZE,col*TILE_SIZE);
  shroudVertices[2].position = sf::Vector2f((row+1)*TILE_SIZE,(col+1)*TILE_SIZE);
  shroudVertices[3].position = sf::Vector2f(row*TILE_SIZE,(col+1)*TILE_SIZE);
}

void Cell::setType(Terrain terrain_type)
{
    terrainType = terrain_type;
}

void Cell::setIndex(int tileIndex) {
    if (tileIndex < 0) return;
    int tileRow = TILE_SIZE * static_cast<int>(terrainType);
    int tileCol = TILE_SIZE * tileIndex;

    vertices[0].texCoords = sf::Vector2f(tileCol, tileRow);
    vertices[1].texCoords = sf::Vector2f(tileCol + TILE_SIZE, tileRow);
    vertices[2].texCoords = sf::Vector2f(tileCol + TILE_SIZE, tileRow + TILE_SIZE);
    vertices[3].texCoords = sf::Vector2f(tileCol, tileRow + TILE_SIZE);    
}

void Cell::setShroudIndex(int tileIndex) {
  if (tileIndex == -1){
    shroudVertices[0].color = sf::Color::Transparent;
    shroudVertices[1].color = sf::Color::Transparent;
    shroudVertices[2].color = sf::Color::Transparent;
    shroudVertices[3].color = sf::Color::Transparent;
    return;
  }  

    int tileCol = TILE_SIZE * tileIndex;
    sf::IntRect rect(tileCol, 0, TILE_SIZE, TILE_SIZE);

    shroudVertices[0].texCoords = sf::Vector2f(tileCol, 0);
    shroudVertices[1].texCoords = sf::Vector2f(tileCol + TILE_SIZE, 0);
    shroudVertices[2].texCoords = sf::Vector2f(tileCol + TILE_SIZE, TILE_SIZE);
    shroudVertices[3].texCoords = sf::Vector2f(tileCol, TILE_SIZE);
}

bool Cell::shouldSmoothWithTerrainType(Cell &other) const
{
    if (terrainType == Terrain::Rock) {
        return ((other.terrainType != Terrain::Mountain) &&
                (other.terrainType != Terrain::Rock) &&
                (other.terrainType != Terrain::Slab));
    }
    if (terrainType == Terrain::Mountain) {
        return (other.terrainType != Terrain::Mountain);
    }
    if (terrainType == Terrain::Slab) {
        return ((other.terrainType != Terrain::Mountain) &&
                (other.terrainType != Terrain::Rock));
    }
    if (terrainType == Terrain::Spice) {
        return ((other.terrainType != Terrain::Spice) &&
                (other.terrainType != Terrain::Spicehill));
    }
    if (terrainType == Terrain::Spicehill) {
        return (other.terrainType != Terrain::Spicehill);
    }
    return (other.terrainType != terrainType);
}

sf::Vertex Cell::getVertex(int index) const
{
  assert(index>=0 && index<=3);

  return vertices[index];
}

sf::Vertex Cell::getShroudVertex(int index) const
{
  assert(index>=0 && index<=3);

  return shroudVertices[index];
}

sf::FloatRect Cell::getBounds() const
{
  sf::FloatRect rect(vertices[0].position,
      vertices[2].position - vertices[0].position);

  return rect;
}
