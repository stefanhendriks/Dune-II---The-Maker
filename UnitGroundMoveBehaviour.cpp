#include "UnitGroundMoveBehaviour.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

UnitGroundMoveBehaviour::UnitGroundMoveBehaviour(Unit &unit) : UnitMoveBehaviour(unit) {

}

UnitGroundMoveBehaviour::~UnitGroundMoveBehaviour() {
}

void UnitGroundMoveBehaviour::updateMovePosition(const UnitContainer &units, sf::Time dt) {
  sf::Vector2f direction = unit.calculateDirection();

  float speed = unit.calculateSpeed(dt);
  sf::Vector2f unitDirection = thor::unitVector(direction);
  unit.sprite.move(speed * unitDirection);

  unit.triggerPreMove();

  if (unit.shouldMove) {
    unit.shouldMove = false;

    for (const auto& u : units) {
      if (unit.collidesWith(*u)) {
        unit.sprite.move(-speed*unitDirection);
        return;
      }
    }

    unit.shadowSprite.move(speed*unitDirection);
    unit.selectedSprite.move(speed*unitDirection);
    unit.triggerMove();
  } else {
    unit.sprite.move(-speed * unitDirection);
  }

}
