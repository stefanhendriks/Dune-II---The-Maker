#ifndef UNIT_MOVE_BEHAVIOR
#define UNIT_MOVE_BEHAVIOR

#include <SFML/Graphics.hpp>
#include "map.h"

/*
 * Move behavior classes are simply classes with a single responsibility for a unit.
 * This makes it possible to assemble a unit with different behaviorable classes.
 *
 * Do note, these classes do not have state about the unit as they are instantiated
 * only once and used by all kinds of units.
 *
 */

//this sounds like policy classes - Koji

class UnitMoveBehavior {

  public:
    UnitMoveBehavior(Map* map, short layer);
    virtual ~UnitMoveBehavior(){}
    virtual bool canMoveTo(sf::Vector2i p) = 0;

    void occupyCell(sf::Vector2i p);
    void unOccupyCell(sf::Vector2i p);

    bool is_layer(short layer) { return this->layer == layer; }
  private:
    short layer;

  protected:
    Map* map;

};

class GroundUnitMoveBehavior : public UnitMoveBehavior {

  public:
    GroundUnitMoveBehavior(Map* map);

    bool canMoveTo(sf::Vector2i p);

  private:
};

class AirUnitMoveBehavior : public UnitMoveBehavior {

  public:
    AirUnitMoveBehavior(Map* map);

    bool canMoveTo(sf::Vector2i p);

  private:
};

class FootUnitMoveBehavior: public UnitMoveBehavior {

  public:
    FootUnitMoveBehavior(Map* map);

    bool canMoveTo(sf::Vector2i p);

};


#endif
