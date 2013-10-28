#ifndef UNITGROUNDMOVEBEHAVIOUR_HPP
#define UNITGROUNDMOVEBEHAVIOUR_HPP

#include "UnitMoveBehaviour.hpp"

class UnitGroundMoveBehaviour : public UnitMoveBehaviour {

  public:
    UnitGroundMoveBehaviour(Unit *unit);
    ~UnitGroundMoveBehaviour();

  void updateMovePosition(const std::vector<Unit> &units, sf::Time dt);

};

#endif
