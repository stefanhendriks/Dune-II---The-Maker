#include "UnitGroundMoveBehaviour.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

UnitGroundMoveBehaviour::UnitGroundMoveBehaviour(Unit *unit) : UnitMoveBehaviour(unit) {

}

UnitGroundMoveBehaviour::~UnitGroundMoveBehaviour() {
}

void UnitGroundMoveBehaviour::updateMovePosition(const std::vector<Unit> &units, sf::Time dt) {
  float speed = dt.asSeconds() * 250.f;
  sf::Vector2f direction = unit->target - unit->getCenter();
  sf::Vector2f unitDirection = thor::unitVector(direction);
  float distance = thor::length(direction);
  if (distance < speed) speed = distance;
  unit->sprite.move(speed*unitDirection);

  unit->messages->triggerEvent(PreMoveMessage(*unit));

  if (unit->shouldMove){
    unit->shouldMove = false;

    //collision detection with units still here
    for (const auto& u : units){
      if (unit->id == u.id) continue;
      if (u.type == Unit::Type::Carryall) continue; // HACK HACK

      if (unit->sprite.getGlobalBounds().intersects(u.sprite.getGlobalBounds())){
        unit->sprite.move(-speed*unitDirection);
        return;
      }
    }

    unit->shadowSprite.move(speed*unitDirection);
    unit->selectedSprite.move(speed*unitDirection);
//    unit->messages->triggerEvent(MoveMessage(*unit));

  }else{
    unit->sprite.move(-speed*unitDirection);
  }
}
