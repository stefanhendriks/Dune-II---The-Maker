#include "UnitGroundMoveBehaviour.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

UnitGroundMoveBehaviour::UnitGroundMoveBehaviour(Unit &unit) : UnitMoveBehaviour(unit) {

}

UnitGroundMoveBehaviour::~UnitGroundMoveBehaviour() {
}

void UnitGroundMoveBehaviour::updateMovePosition(const UnitContainer &units, sf::Time dt) {
  sf::Vector2f direction = unit.calculateDirection();

  float distance = thor::length(direction);
  float speed = dt.asSeconds() * 250.f;
  if (distance < speed) speed = distance;

  sf::Vector2f unitDirection = thor::unitVector(direction);
  unit.sprite.move(speed * unitDirection);

  unit.triggerPreMove();

  if (unit.shouldMove) {
    unit.shouldMove = false;

    //collision detection with units still here
    for (const auto& u : units){
      if (unit.id == u->id) continue;
      if (u->type == Unit::Type::Carryall) continue; // HACK HACK

      if (unit.sprite.getGlobalBounds().intersects(u->sprite.getGlobalBounds())){
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
