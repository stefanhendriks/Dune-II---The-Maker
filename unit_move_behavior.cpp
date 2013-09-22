#include <iostream>
#include "unit_move_behavior.h"

using namespace std;

UnitMoveBehavior::UnitMoveBehavior(Map* map, short layer) {
  this->layer = layer;
  this->map = map;
}

UnitMoveBehavior::~UnitMoveBehavior() {
  // do not delete, class does not own them
  this->map = NULL;
}

void UnitMoveBehavior::occupyCell(Point p) {
  map->occupyCell(p, layer);
}

void UnitMoveBehavior::unOccupyCell(Point p) {
  map->unOccupyCell(p, layer);
}


//// Ground unit Move
GroundUnitMoveBehavior::GroundUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, MAP_LAYER_GROUND) {
}

GroundUnitMoveBehavior::~GroundUnitMoveBehavior() {
}

bool GroundUnitMoveBehavior::canMoveTo(Point p) {
  if (map->is_occupied(p, MAP_LAYER_GROUND)) return false;

  Point map_point = map->toMapPoint(p);
  int terrain_type = map->getCell(map_point)->terrain_type;
  return terrain_type != TERRAIN_TYPE_MOUNTAIN;
}

//// Air unit Move
AirUnitMoveBehavior::AirUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, MAP_LAYER_AIR) {
}

AirUnitMoveBehavior::~AirUnitMoveBehavior() {
}

bool AirUnitMoveBehavior::canMoveTo(Point p) {
  return true;
}

//// unit Move on foot
FootUnitMoveBehavior::FootUnitMoveBehavior(Map* map) : UnitMoveBehavior(map, MAP_LAYER_GROUND) {
}

FootUnitMoveBehavior::~FootUnitMoveBehavior() {
}

bool FootUnitMoveBehavior::canMoveTo(Point p) {
  return true;
}


