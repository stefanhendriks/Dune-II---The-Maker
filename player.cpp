#include "player.h"
#include "houses.h"

Player::Player(House theHouse, int theId):
    id(theId), house(theHouse)
{
}

int Player::getColor()
{
    return getDefaultColor(house);
}

bool Player::operator ==(const Player &other) const
{
    return (id==other.id);
}

bool Player::operator !=(const Player &other) const
{
    return !(*this==other);
}
