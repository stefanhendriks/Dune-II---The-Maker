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

    Unit create(Unit::Type type, const Player& player, sf::Vector2f position) const;

private:
    std::map<Unit::Type, sf::Texture> shadowTextures;
    sf::Texture selectedTexture;
};

#endif // UNITREPOSITORY_HPP
