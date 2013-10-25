#ifndef UNITREPOSITORY_HPP
#define UNITREPOSITORY_HPP
#include "Unit.hpp"
#include "Player.hpp"
#include <map>
#include <SFML/Graphics/Texture.hpp>

class UnitRepository
{
  public:
    UnitRepository(MessageSystem& messages);

    Unit create(Unit::Type type, const Player& player, int x, int y);

  private:
    std::map<Unit::Type, sf::Texture> unitTextures;
    std::map<Unit::Type, sf::Texture> shadowTextures;
    sf::Texture selectedTexture;

    MessageSystem& messages;
    int idCount;

    const sf::Texture& getTexture(Unit::Type type, const sf::Color& color);
    const sf::Texture& getShadow(Unit::Type type);

    void recolor(sf::Image& image, const sf::Color& color) const;
    sf::Texture load(const std::string& filename, const sf::Color& bitmask, const sf::Color& color = sf::Color(0,0,0,0)) const;

    std::map<Unit::Type, std::string> filenames;
};

#endif // UNITREPOSITORY_HPP
