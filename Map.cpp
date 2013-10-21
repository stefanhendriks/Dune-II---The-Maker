#include "Map.hpp"
#include "MapLoader.hpp"

#include <memory>    /* std::unique_ptr */
#include <cmath>     /* ceil */
#include "Unit.hpp"
#include <boost/cast.hpp>

Map::Map(sf::Texture &terrain, sf::Texture &shroud_edges, MessageSystem &messages) :
  terrain(terrain),
  shroudEdges(shroud_edges),
  messages(messages)
{
  maxWidth = MAX_WIDTH - 1;
  maxHeight = MAX_HEIGHT - 1;  

  for (int x = 0; x < MAX_WIDTH; x++) {
    for (int y = 0; y < MAX_HEIGHT; y++) {
      int i = (y * MAX_WIDTH) + x;
      cells[i].init(x, y);
    }
  }

  this->messages.connect("premove", [this](const Message& message){
    const PreMoveMessage* received = boost::polymorphic_downcast<const PreMoveMessage*>(&message);
    sf::Vector2i mapPoint = toMapPoint(received->unit.getCenter());
    if (getCell(mapPoint.x, mapPoint.y).terrainType != Terrain::Mountain)
      received->unit.shouldMove = true;
  });

  this->messages.connect("unitMove", [this](const Message& message){
    const MoveMessage* received = boost::polymorphic_downcast<const MoveMessage*>(&message);
    removeShroud(received->unit.getCenter(), received->unit.getViewRange());
  });
}

void Map::load(std::string file) {
  MapLoader::load(file, this);
  for (auto& cell : cells)
    cell.setIndex(determineCellTile(cell));

  updateShroud();
}

void Map::updateShroud()
{
  for (auto& cell : cells)
    cell.setShroudIndex(determineShroudEdge(cell));
}

void Map::prepare(const sf::Vector2f& topLeft) const
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
      const auto& vertices = cells[index].getQuad();
      vertexArray.insert(vertexArray.end(), vertices.begin(), vertices.end());
      const auto& shroudVertices = cells[index].getShroudQuad();
      shroudArray.insert(shroudArray.end(), shroudVertices.begin(), shroudVertices.end());
    }
  }

}

void Map::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(vertexArray.data(), vertexArray.size(), sf::Quads, &terrain);
}

void Map::drawShrouded(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(shroudArray.data(), shroudArray.size(), sf::Quads, &shroudEdges);
}

sf::Vector2i Map::toMapPoint(const sf::Vector2f &world_point) const{
  sf::Vector2i result(world_point.x / Cell::TILE_SIZE, world_point.y / Cell::TILE_SIZE);
  return result;
}

int Map::determineCellTile(Cell& c) {
  bool cell_up = !c.shouldSmoothWithTerrainType(getCell(c.x, c.y-1));
  bool cell_down = !c.shouldSmoothWithTerrainType(getCell(c.x, c.y+1));
  bool cell_left = !c.shouldSmoothWithTerrainType(getCell(c.x-1, c.y));
  bool cell_right = !c.shouldSmoothWithTerrainType(getCell(c.x+1, c.y));

  return determineTerrainTile(cell_up, cell_down, cell_left, cell_right);
}

int Map::determineTerrainTile(bool cellUp, bool cellDown, bool cellLeft, bool cellRight) const {
  if ( cellUp &&  cellDown &&  cellLeft &&  cellRight) return 0;
  if ( cellUp &&  cellDown && !cellLeft &&  cellRight) return 1;
  if ( cellUp &&  cellDown &&  cellLeft && !cellRight) return 2;
  if (!cellUp &&  cellDown &&  cellLeft &&  cellRight) return 3;
  if ( cellUp && !cellDown &&  cellLeft &&  cellRight) return 4;
  if (!cellUp &&  cellDown && !cellLeft &&  cellRight) return 5;
  if ( cellUp && !cellDown &&  cellLeft && !cellRight) return 6;
  if (!cellUp &&  cellDown &&  cellLeft && !cellRight) return 7;
  if ( cellUp && !cellDown && !cellLeft &&  cellRight) return 8;
  if (!cellUp && !cellDown && !cellLeft && !cellRight) return 9;
  if (!cellUp && !cellDown &&  cellLeft &&  cellRight) return 10;
  if (!cellUp && !cellDown &&  cellLeft && !cellRight) return 11;
  if (!cellUp && !cellDown && !cellLeft &&  cellRight) return 12;
  if ( cellUp && !cellDown && !cellLeft && !cellRight) return 13;
  if (!cellUp &&  cellDown && !cellLeft && !cellRight) return 14;
  if ( cellUp &&  cellDown && !cellLeft && !cellRight) return 15;
  return -1;
}

int Map::determineShroudEdge(Cell& cell) {
  if (cell.shrouded) return 0;

  bool cellUp = getCell(cell.x, cell.y-1).shrouded;
  bool cellDown = getCell(cell.x, cell.y+1).shrouded;
  bool cellLeft = getCell(cell.x-1, cell.y).shrouded;
  bool cellRight = getCell(cell.x+1, cell.y).shrouded;

  // Its harder then you think to make static consts for these 'magic values'.
  if (!cellUp && !cellDown && !cellLeft && !cellRight)  return -1;
  if ( cellUp && !cellDown &&  cellLeft && !cellRight)  return 1;
  if ( cellUp && !cellDown && !cellLeft && !cellRight)  return 2;
  if ( cellUp && !cellDown && !cellLeft &&  cellRight)  return 3;
  if (!cellUp && !cellDown && !cellLeft &&  cellRight)  return 4;
  if (!cellUp &&  cellDown && !cellLeft &&  cellRight)  return 5;
  if (!cellUp &&  cellDown && !cellLeft && !cellRight)  return 6;
  if (!cellUp &&  cellDown &&  cellLeft && !cellRight)  return 7;
  if (!cellUp && !cellDown &&  cellLeft && !cellRight)  return 8;
  if ( cellUp &&  cellDown &&  cellLeft && !cellRight)  return 9;
  if ( cellUp && !cellDown &&  cellLeft &&  cellRight)  return 10;
  if ( cellUp &&  cellDown && !cellLeft &&  cellRight)  return 11;
  if (!cellUp &&  cellDown &&  cellLeft &&  cellRight)  return 12;

  return -1;
}

int Map::getMaxWidth() const
{
  return maxWidth;
}

int Map::getMaxHeight() const
{
  return maxHeight;
}


void Map::setBoundaries(int maxWidth, int maxHeight) {
  this->maxWidth = maxWidth;
  this->maxHeight = maxHeight;
}
