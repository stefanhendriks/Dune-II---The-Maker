#include "unit_move_behavior.h"

UnitMoveBehavior::UnitMoveBehavior(Map* map, short layer) {
  this->layer = layer;
  this->map = map;
}

void UnitMoveBehavior::occupyCell(sf::Vector2i p) {
  map->occupyCell(p, layer);
}

void UnitMoveBehavior::unOccupyCell(sf::Vector2i p) {
  map->unOccupyCell(p, layer);
}


//// Ground unit Move
GroundUnitMoveBehavior::GroundUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, Map::LAYER_GROUND) {
}

bool GroundUnitMoveBehavior::canMoveTo(sf::Vector2i p) {
  if (map->is_occupied(p, Map::LAYER_GROUND)) return false;

  sf::Vector2i map_point = map->toMapPoint(p);
  Terrain terrain_type = map->getCell(map_point)->terrainType;
  return terrain_type != Terrain::Mountain;
}

//// Air unit Move
AirUnitMoveBehavior::AirUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, Map::LAYER_AIR) {
}

bool AirUnitMoveBehavior::canMoveTo(sf::Vector2i p) {
  return true;
}

//// unit Move on foot
FootUnitMoveBehavior::FootUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, Map::LAYER_GROUND) {
}

bool FootUnitMoveBehavior::canMoveTo(sf::Vector2i p) {
  return true;
}



