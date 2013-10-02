#include "map.h"
#include "maploader.h"

#include <memory>    /* std::unique_ptr */
#include <cmath>     /* ceil */

Map::Map(sf::Texture &terrain, sf::Texture &shroud_edges) :
  terrain(terrain),
  shroud_edges(shroud_edges)
{
  max_width = MAX_WIDTH - 1;
  max_height = MAX_HEIGHT - 1;

  vertexArray.setPrimitiveType(sf::Quads);
  shroudArray.setPrimitiveType(sf::Quads);

  for (int x = 0; x < MAX_WIDTH; x++) {
    for (int y = 0; y < MAX_HEIGHT; y++) {
      int i = (y * MAX_WIDTH) + x;
      cells[i].init(x, y);
    }
  }
}

void Map::load(std::string file) {
  MapLoader::load(file, this);
  for (auto& cell : cells)
    cell.setIndex(determineCellTile(&cell));

  updateShroud();
}

void Map::updateShroud()
{
  for (auto& cell : cells)
    cell.setShroudIndex(determineShroudEdge(&cell));
}

void Map::prepare(const sf::Vector2f& topLeft)
{
  vertexArray.clear();
  shroudArray.clear();
  sf::Vector2f viewSize(800,600); //TODO: cheating - this should be passed by the window
  sf::Vector2i mapCell = toMapPoint(topLeft);

  // TODO: remove magic numbers
  const int nofTilesInRow = (viewSize.x/Cell::TILE_SIZE) + 3;
  const int nofRows = (viewSize.y/Cell::TILE_SIZE) + 3;

  for (int x=-1; x < nofTilesInRow; ++x) {
    for (int y=-1; y < nofRows; ++y) {
      int index = ((y + mapCell.y) * MAX_WIDTH) + x + mapCell.x;
      if ((index<0) || (index>=MAX_SIZE)) continue;
      for (int k=0; k<4; ++k){
        vertexArray.append(cells[index].getVertex(k));
        shroudArray.append(cells[index].getShroudVertex(k));
      }
    }
  }

}

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(vertexArray, &terrain);
}

void Map::drawShrouded(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(shroudArray, &shroud_edges);
}

int Map::determineCellTile(Cell* c) {
  bool cell_up = !c->shouldSmoothWithTerrainType(getCell(c->x, c->y-1));
  bool cell_down = !c->shouldSmoothWithTerrainType(getCell(c->x, c->y+1));
  bool cell_left = !c->shouldSmoothWithTerrainType(getCell(c->x-1, c->y));
  bool cell_right = !c->shouldSmoothWithTerrainType(getCell(c->x+1, c->y));

  return determineTerrainTile(cell_up, cell_down, cell_left, cell_right);
}

int Map::determineTerrainTile(bool cell_up, bool cell_down, bool cell_left, bool cell_right) {
  if ( cell_up &&  cell_down &&  cell_left &&  cell_right) return 0;
  if ( cell_up &&  cell_down && !cell_left &&  cell_right) return 1;
  if ( cell_up &&  cell_down &&  cell_left && !cell_right) return 2;
  if (!cell_up &&  cell_down &&  cell_left &&  cell_right) return 3;
  if ( cell_up && !cell_down &&  cell_left &&  cell_right) return 4;
  if (!cell_up &&  cell_down && !cell_left &&  cell_right) return 5;
  if ( cell_up && !cell_down &&  cell_left && !cell_right) return 6;
  if (!cell_up &&  cell_down &&  cell_left && !cell_right) return 7;
  if ( cell_up && !cell_down && !cell_left &&  cell_right) return 8;
  if (!cell_up && !cell_down && !cell_left && !cell_right) return 9;
  if (!cell_up && !cell_down &&  cell_left &&  cell_right) return 10;
  if (!cell_up && !cell_down &&  cell_left && !cell_right) return 11;
  if (!cell_up && !cell_down && !cell_left &&  cell_right) return 12;
  if ( cell_up && !cell_down && !cell_left && !cell_right) return 13;
  if (!cell_up &&  cell_down && !cell_left && !cell_right) return 14;
  if ( cell_up &&  cell_down && !cell_left && !cell_right) return 15;
  return -1;
}

int Map::determineShroudEdge(Cell* c) {
  if (c->shrouded) return 0;

  bool cell_up = getCell(c->x, c->y-1)->shrouded;
  bool cell_down = getCell(c->x, c->y+1)->shrouded;
  bool cell_left = getCell(c->x-1, c->y)->shrouded;
  bool cell_right = getCell(c->x+1, c->y)->shrouded;

  // Its harder then you think to make static consts for these 'magic values'.
  if (!cell_up && !cell_down && !cell_left && !cell_right)  return -1;
  if ( cell_up && !cell_down &&  cell_left && !cell_right)  return 1;
  if ( cell_up && !cell_down && !cell_left && !cell_right)  return 2;
  if ( cell_up && !cell_down && !cell_left &&  cell_right)  return 3;
  if (!cell_up && !cell_down && !cell_left &&  cell_right)  return 4;
  if (!cell_up &&  cell_down && !cell_left &&  cell_right)  return 5;
  if (!cell_up &&  cell_down && !cell_left && !cell_right)  return 6;
  if (!cell_up &&  cell_down &&  cell_left && !cell_right)  return 7;
  if (!cell_up && !cell_down &&  cell_left && !cell_right)  return 8;
  if ( cell_up &&  cell_down &&  cell_left && !cell_right)  return 9;
  if ( cell_up && !cell_down &&  cell_left &&  cell_right)  return 10;
  if ( cell_up &&  cell_down && !cell_left &&  cell_right)  return 11;
  if (!cell_up &&  cell_down &&  cell_left &&  cell_right)  return 12;

  return -1;
}


void Map::setBoundaries(int max_width, int max_height) {
  this->max_width = max_width;
  this->max_height = max_height;
}

bool Map::is_occupied(sf::Vector2i p, short layer) {
  int map_x = p.x / Cell::TILE_SIZE;
  int map_y = p.y / Cell::TILE_SIZE;
  Cell* c = getCell(map_x, map_y);
  return c->occupied[layer];
}
