#include "UnitRepository.hpp"

UnitRepository::UnitRepository():
    idCount(0)
{
    sf::Image selectedImage;
    selectedImage.loadFromFile("graphics/selected.bmp");
    selectedImage.createMaskFromColor(sf::Color(255, 0, 255));
    selectedTexture.loadFromImage(selectedImage);
}

Unit UnitRepository::create(Unit::Type type, const Player &player, sf::Vector2f position, Map& map)
{
    return Unit(player.getTexture(type), getShadow(type), selectedTexture, position, map, idCount++);
}

const sf::Texture &UnitRepository::getShadow(Unit::Type type) const
{
    auto found = shadowTextures.find(type);
    if (found == shadowTextures.end()){
      sf::Image image;
      image.loadFromFile("graphics/Unit_Trike_s.bmp");
      image.createMaskFromColor(sf::Color(255,0,255));

      sf::Texture texture;
      texture.loadFromImage(image);
      shadowTextures.insert({type, std::move(texture)});
      return shadowTextures[type];
    }
    return found->second;
}
