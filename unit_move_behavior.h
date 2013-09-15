#ifndef UNIT_MOVE_BEHAVIOR
#define UNIT_MOVE_BEHAVIOR

#include "point.h"
#include "map.h"

/*
 * Move behavior classes are simply classes with a single responsibility for a unit.
 * This makes it possible to assemble a unit with different behaviorable classes.
 *
 * Do note, these classes do not have state about the unit as they are instantiated
 * only once and used by all kinds of units.
 *
 */

class UnitMoveBehavior {

  public:
    UnitMoveBehavior(Map* map, short layer);
    ~UnitMoveBehavior();
    virtual bool canMoveTo(Point p) = 0;

    void occupyCell(Point p);
    void unOccupyCell(Point p);

    bool is_layer(short layer) { return this->layer == layer; }
  private:
    short layer;

  protected:
    Map* map;

};

class GroundUnitMoveBehavior: public UnitMoveBehavior {

  public:
    GroundUnitMoveBehavior(Map* map);
    ~GroundUnitMoveBehavior();
    bool canMoveTo(Point p);

  private:
};

class AirUnitMoveBehavior: public UnitMoveBehavior {

  public:
    AirUnitMoveBehavior(Map* map);
    ~AirUnitMoveBehavior();
    bool canMoveTo(Point p);

  private:
};

class FootUnitMoveBehavior: public UnitMoveBehavior {

  public:
    FootUnitMoveBehavior(Map* map);
    ~FootUnitMoveBehavior();
    bool canMoveTo(Point p);

};


#endif
