#include "UnitRepository.hpp"

UnitRepository::UnitRepository():
    idCount(0)
{
}

Unit UnitRepository::create(Unit::Type type, const Player &player, sf::Vector2f position) const
{
    //return Unit(player.getTexture(type), getShadow(type), selectedTexture, position, idCount);
}
