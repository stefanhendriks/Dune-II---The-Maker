#ifndef UNITMOVEBEHAVIOUR_HPP
#define UNITMOVEBEHAVIOUR_HPP

#include "Unit.hpp"

class UnitMoveBehaviour {

  public:
    UnitMoveBehaviour(Unit *unit);
    ~UnitMoveBehaviour();

    virtual void updateMovePosition(const std::vector<Unit>& units, sf::Time dt) = 0;

  protected:
    Unit *unit;

};
#endif
