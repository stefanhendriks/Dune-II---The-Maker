#ifndef UNIT_MOVE_BEHAVIOR
#define UNIT_MOVE_BEHAVIOR

#include "point.h"
#include "map.h"

class UnitMoveBehavior {

  public:
    UnitMoveBehavior();
    ~UnitMoveBehavior();
    virtual bool canMoveTo(Point p) = 0;

};

class GroundUnitMovementBehavior: public UnitMoveBehavior {

  public:
    GroundUnitMovementBehavior(Map* map);
    ~GroundUnitMovementBehavior();
    bool canMoveTo(Point p);

  private:
    Map* map;
};

class AirUnitMovementBehavior: public UnitMoveBehavior {

  public:
    AirUnitMovementBehavior();
    ~AirUnitMovementBehavior();
    bool canMoveTo(Point p);

  private:
    Map* map;
};



#endif
