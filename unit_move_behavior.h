#ifndef UNIT_MOVE_BEHAVIOR
#define UNIT_MOVE_BEHAVIOR

#include "point.h"
#include "map.h"

class UnitMoveBehavior {

  public:
    UnitMoveBehavior(Map* map, short layer);
    virtual ~UnitMoveBehavior();
    virtual bool canMoveTo(Point p) = 0;

    void occupyCell(Point p);
    void unOccupyCell(Point p);

    bool is_layer(short layer) { return this->layer == layer; }
  private:
    short layer;

  protected:
    Map* map;

};

class GroundUnitMovementBehavior: public UnitMoveBehavior {

  public:
    GroundUnitMovementBehavior(Map* map);
    ~GroundUnitMovementBehavior();
    bool canMoveTo(Point p);

  private:
};

class AirUnitMovementBehavior: public UnitMoveBehavior {

  public:
    AirUnitMovementBehavior(Map* map);
    ~AirUnitMovementBehavior();
    bool canMoveTo(Point p);

  private:
};



#endif
