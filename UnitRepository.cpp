#include "UnitRepository.hpp"
#include <cassert>

UnitRepository::UnitRepository(MessageSystem& messages):
  messages(messages),
  idCount(0)
{
  sf::Image selectedImage;
  selectedImage.loadFromFile("graphics/selected.bmp");
  selectedImage.createMaskFromColor(sf::Color(255, 0, 255));
  selectedTexture.loadFromImage(selectedImage);

  filenames[Unit::Type::Trike] = "Unit_Trike";
  filenames[Unit::Type::Quad] = "Unit_Quad";
  filenames[Unit::Type::Frigate] = "Unit_Frigate";
  filenames[Unit::Type::Carryall] = "Unit_Carryall";
  filenames[Unit::Type::Devastator] = "Unit_Devastator";
  filenames[Unit::Type::Soldier] = "Unit_Soldier";
}

Unit UnitRepository::create(Unit::Type type, const Player &player, sf::Vector2f position)
{
  TexturePack pack{&getTexture(type, player.getColor()), &getShadow(type), &selectedTexture};
  return Unit(pack, messages, position, idCount++);
}

const sf::Texture &UnitRepository::getTexture(Unit::Type type, const sf::Color &color)
{
  auto unitTexture = unitTextures.find(type);

  if (unitTexture == unitTextures.end()) {
    assert(filenames.find(type) != filenames.end());
    std::string toLoad = "graphics/" + filenames[type] + ".bmp";
    unitTextures.insert({type, std::move(load(toLoad, sf::Color(0,0,0), color))});
    return unitTextures[type];
  }

  return unitTexture->second;
}

const sf::Texture &UnitRepository::getShadow(Unit::Type type)
{
  auto found = shadowTextures.find(type);
  if (found == shadowTextures.end()){
    assert(filenames.find(type) != filenames.end());
    std::string toLoad = "graphics/" + filenames[type] + "_s.bmp";
    shadowTextures.insert({type, std::move(load(toLoad, sf::Color(255,0,255)))});
    return shadowTextures[type];
  }

  return found->second;
}

void UnitRepository::recolor(sf::Image &image, const sf::Color &color) const
{
  sf::Vector2u imageSize = image.getSize();

  for (unsigned int x = 0; x < imageSize.x; x++){
    for (unsigned int y = 0; y < imageSize.y; y++){
      sf::Color pixelColor = image.getPixel(x, y);

      if ((pixelColor == sf::Color(125,0,0)) ||
          (pixelColor == sf::Color(214,0,0)) ||
          (pixelColor == sf::Color(60,0,0))  ||
          (pixelColor == sf::Color(89,0,0))  ||
          (pixelColor == sf::Color(153,0,0)) ||
          (pixelColor == sf::Color(182,0,0))) {
        image.setPixel(x, y, color); // TODO: Shade color here properly
      }
    }
  }
}

sf::Texture UnitRepository::load(const std::string &filename, const sf::Color &bitmask, const sf::Color &color) const
{
  sf::Image image;
  image.loadFromFile(filename);
  image.createMaskFromColor(bitmask);
  if (color != sf::Color(0,0,0,0))
    recolor(image, color);

  sf::Texture texture;
  texture.loadFromImage(image);
  return texture;
}
