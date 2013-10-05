#include "UnitRepository.hpp"
#include <cassert>

UnitRepository::UnitRepository():
    idCount(0)
{
    sf::Image selectedImage;
    selectedImage.loadFromFile("graphics/selected.bmp");
    selectedImage.createMaskFromColor(sf::Color(255, 0, 255));
    selectedTexture.loadFromImage(selectedImage);

    filenames[Unit::Type::Trike] = "Unit_Trike_s.bmp";
    filenames[Unit::Type::Quad] = "Unit_Quad_s.bmp";
    filenames[Unit::Type::Frigate] = "Unit_Frigate_s.bmp";
    filenames[Unit::Type::Carryall] = "Unit_Carryall_s.bmp";
    filenames[Unit::Type::Devastator] = "Unit_Devastator_s.bmp";
    filenames[Unit::Type::Soldier] = "Unit_Soldier_s.bmp";
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
      assert(filenames.find(type) != filenames.end());
      image.loadFromFile("graphics/" + filenames.at(type));
      image.createMaskFromColor(sf::Color(255,0,255));

      sf::Texture texture;
      texture.loadFromImage(image);
      shadowTextures.insert({type, std::move(texture)});
      return shadowTextures[type];
    }
    return found->second;
}
