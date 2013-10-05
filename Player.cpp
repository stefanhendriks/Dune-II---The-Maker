#include "Player.hpp"
#include "Houses.hpp"
#include <cassert>

Player::Player(House theHouse, int theId):
    id(theId), house(theHouse), color(Houses::getDefaultColor(house))
{
  // SMELL: We also do this in the unit repository
  filenames[Unit::Type::Trike] = "Unit_Trike.bmp";
  filenames[Unit::Type::Quad] = "Unit_Quad.bmp";
  filenames[Unit::Type::Frigate] = "Unit_Frigate.bmp";
  filenames[Unit::Type::Carryall] = "Unit_Carryall.bmp";
  filenames[Unit::Type::Devastator] = "Unit_Devastator.bmp";
  filenames[Unit::Type::Soldier] = "Unit_Soldier.bmp";
}

sf::Color Player::getColor() const
{
    return color;
}

bool Player::operator ==(const Player &other) const
{
    return (id==other.id);
}

bool Player::operator !=(const Player &other) const
{
    return !(*this==other);
}

void Player::recolor(sf::Image &image) const
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

const sf::Texture &Player::getTexture(Unit::Type type) const
{
  auto unitTexture = loadedUnitTextures.find(type);

  // SMELL: we also do this in the unitRepository, but the only difference
  // is coloring. Need to move to 1 spot
  if (unitTexture == loadedUnitTextures.end()) {
    sf::Image image;
    assert(filenames.find(type) != filenames.end());
    image.loadFromFile("graphics/" + filenames.at(type));
    image.createMaskFromColor(sf::Color(0,0,0));
    recolor(image);

    sf::Texture texture;
    texture.loadFromImage(image);
    loadedUnitTextures.insert({type, std::move(texture)});
    return loadedUnitTextures[type];
  }

  return unitTexture->second;
}
