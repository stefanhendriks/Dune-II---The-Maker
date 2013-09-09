#include "unit_move_behavior.h"

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
  return map->getCell(map_point)->terrain_type == TERRAIN_TYPE_MOUNTAIN;
}

