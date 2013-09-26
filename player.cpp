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
