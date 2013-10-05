#ifndef UNITREPOSITORY_HPP
#define UNITREPOSITORY_HPP
#include "Unit.hpp"
#include "Player.hpp"
#include <map>
#include <SFML/Graphics/Texture.hpp>

class UnitRepository
{
public:
    UnitRepository();

    Unit create(Unit::Type type, const Player& player, sf::Vector2f position, Map &map) const;

private:
    std::map<Unit::Type, sf::Texture> shadowTextures;
    sf::Texture selectedTexture;

    int idCount;
};

#endif // UNITREPOSITORY_HPP
