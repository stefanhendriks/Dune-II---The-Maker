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

    for (const auto& u : units) {
      if (unit.collidesWith(*u)) {
        unit.move(-movement);
        return;
      }
    }

    unit.triggerMove();
  } else {
    unit.move(-movement);
  }

}
