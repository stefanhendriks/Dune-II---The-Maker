#include <iostream>
#include "unit_move_behavior.h"

using namespace std;

UnitMoveBehavior::UnitMoveBehavior() {
}

UnitMoveBehavior::~UnitMoveBehavior() {
}

//// Ground unit movement
GroundUnitMovementBehavior::GroundUnitMovementBehavior(Map* map) {
  this->map = map;
}

GroundUnitMovementBehavior::~GroundUnitMovementBehavior() {
  this->map = NULL; // we are not the owner, so don't delete it.
}

bool GroundUnitMovementBehavior::canMoveTo(Point p) {
  if (map->is_occupied(p)) return false;

  Point map_point = map->toMapPoint(p);
  int terrain_type = map->getCell(map_point)->terrain_type;
  return terrain_type != TERRAIN_TYPE_MOUNTAIN;
}

//// Air unit movement
AirUnitMovementBehavior::AirUnitMovementBehavior() {
}

AirUnitMovementBehavior::~AirUnitMovementBehavior() {
}

bool AirUnitMovementBehavior::canMoveTo(Point p) {
  return true;
}

