#ifndef UNIT_MOVE_BEHAVIOR
#define UNIT_MOVE_BEHAVIOR

#include "point.h"
#include "map.h"

class UnitMoveBehavior {

  public:
    UnitMoveBehavior();
    ~UnitMoveBehavior();
    virtual bool canMoveTo(Point p) = 0;
    virtual void occupyCell(Point p) = 0;
    virtual void unOccupyCell(Point p) = 0;

};

class GroundUnitMovementBehavior: public UnitMoveBehavior {

  public:
    GroundUnitMovementBehavior(Map* map);
    ~GroundUnitMovementBehavior();
    bool canMoveTo(Point p);
    void occupyCell(Point p);
    void unOccupyCell(Point p);

  private:
    Map* map;
    Unit* unit;
};

class AirUnitMovementBehavior: public UnitMoveBehavior {

  public:
    AirUnitMovementBehavior(Map* map);
    ~AirUnitMovementBehavior();
    bool canMoveTo(Point p);
    void occupyCell(Point p);
    void unOccupyCell(Point p);

  private:
    Map* map;
};



#endif
