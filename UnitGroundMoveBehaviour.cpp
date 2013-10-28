#include "UnitGroundMoveBehaviour.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

UnitGroundMoveBehaviour::UnitGroundMoveBehaviour(Unit &unit) : UnitMoveBehaviour(unit) {

}

UnitGroundMoveBehaviour::~UnitGroundMoveBehaviour() {
}

void UnitGroundMoveBehaviour::updateMovePosition(const UnitContainer &units, sf::Time dt) {
  unit.triggerPreMove();
}
