#include <iostream>
#include "unit_move_behavior.h"

using namespace std;

UnitMoveBehavior::UnitMoveBehavior(Map* map, short layer) {
  this->layer = layer;
  this->map = map;
}

UnitMoveBehavior::~UnitMoveBehavior() {
  this->map = NULL;
  this->layer = 0;
}

void UnitMoveBehavior::occupyCell(Point p) {
  map->occupyCell(p, layer);
}

void UnitMoveBehavior::unOccupyCell(Point p) {
  map->unOccupyCell(p, layer);
}


//// Ground unit movement
GroundUnitMovementBehavior::GroundUnitMovementBehavior(Map* map) : UnitMoveBehavior(map, MAP_LAYER_GROUND) {
}

GroundUnitMovementBehavior::~GroundUnitMovementBehavior() {
}

bool GroundUnitMovementBehavior::canMoveTo(Point p) {
  if (map->is_occupied(p, MAP_LAYER_GROUND)) return false;

  Point map_point = map->toMapPoint(p);
  int terrain_type = map->getCell(map_point)->terrain_type;
  return terrain_type != TERRAIN_TYPE_MOUNTAIN;
}

//// Air unit movement
AirUnitMovementBehavior::AirUnitMovementBehavior(Map* map) : UnitMoveBehavior(map, MAP_LAYER_AIR) {
}

AirUnitMovementBehavior::~AirUnitMovementBehavior() {
}

bool AirUnitMovementBehavior::canMoveTo(Point p) {
  return true;
}

