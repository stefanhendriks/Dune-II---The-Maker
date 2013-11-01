#include "UnitGroundMoveBehaviour.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

UnitGroundMoveBehaviour::UnitGroundMoveBehaviour(Unit &unit) : UnitMoveBehaviour(unit) {

}

UnitGroundMoveBehaviour::~UnitGroundMoveBehaviour() {
}

void UnitGroundMoveBehaviour::updateMovePosition(const UnitContainer &units, sf::Time dt) {
  sf::Vector2f movement = unit.calculateMovement(dt);
  unit.move(movement);

  unit.triggerPreMove();

  if (unit.shouldMove) {
    unit.shouldMove = false;

    if (unit.collidesWithAUnit(units)) unit.move(-movement);

    unit.triggerMove();
  } else {
    unit.move(-movement);
  }

}
