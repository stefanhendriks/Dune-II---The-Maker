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

void GroundUnitMovementBehavior::occupyCell(Point p) {
  map->occupyCell(p, MAP_LAYER_GROUND);
}

void GroundUnitMovementBehavior::unOccupyCell(Point p) {
  map->unOccupyCell(p, MAP_LAYER_GROUND);
}

bool GroundUnitMovementBehavior::canMoveTo(Point p) {
  if (map->is_occupied(p, MAP_LAYER_GROUND)) return false;

  Point map_point = map->toMapPoint(p);
  int terrain_type = map->getCell(map_point)->terrain_type;
  return terrain_type != TERRAIN_TYPE_MOUNTAIN;
}

//// Air unit movement
AirUnitMovementBehavior::AirUnitMovementBehavior(Map* map) {
  this->map = map;
}

AirUnitMovementBehavior::~AirUnitMovementBehavior() {
  this->map = NULL;
}

bool AirUnitMovementBehavior::canMoveTo(Point p) {
  return true;
}

void AirUnitMovementBehavior::occupyCell(Point p) {
  map->occupyCell(p, MAP_LAYER_AIR);
}

void AirUnitMovementBehavior::unOccupyCell(Point p) {
  map->unOccupyCell(p, MAP_LAYER_AIR);
}

