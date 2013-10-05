#include "Player.hpp"
#include "Houses.hpp"
#include <cassert>

Player::Player(House theHouse, int theId):
  id(theId), house(theHouse), color(Houses::getDefaultColor(house))
{ 
}

sf::Color Player::getColor() const {
  return color;
}

bool Player::operator ==(const Player &other) const {
  return (id==other.id);
}

bool Player::operator !=(const Player &other) const {
  return !(*this==other);
}
