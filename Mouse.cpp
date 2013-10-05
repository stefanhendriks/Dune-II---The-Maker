#include "Mouse.hpp"

Mouse::Mouse():
  m_type(Type::Default)
{
  sf::Image defaultImage;
  defaultImage.loadFromFile("graphics/MS_Pointer.bmp");
  defaultImage.createMaskFromColor(sf::Color(255, 0, 255));
  defaultTexture.loadFromImage(defaultImage);

  sf::Image moveImage;
  moveImage.loadFromFile("graphics/MS_Move.bmp");
  moveImage.createMaskFromColor(sf::Color(255, 0, 255));
  moveTexture.loadFromImage(moveImage);

  sprite.setTexture(defaultTexture);
}


void Mouse::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(sprite);
}

void Mouse::setPosition(const sf::Vector2f &point)
{
  sprite.setPosition(point);
}

void Mouse::setType(Type type)
{
  m_type = type;
  switch (m_type) {
  case Type::Default:
    sprite.setTexture(defaultTexture);
    break;
  case Type::Move:
    sprite.setTexture(moveTexture);
    break;
  case Type::Attack:
    sprite.setTexture(defaultTexture);
    break;
  }
}

sf::Vector2i Mouse::getHotspot(sf::Event event) const
{
  //if Default hotspot is top-left, else the center
  switch (m_type) {
  default:
  case Type::Default:
    return sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
    break;
  case Type::Move:{
    sf::FloatRect spriteRect = sprite.getGlobalBounds();
    return sf::Vector2i(event.mouseButton.x + spriteRect.width/2, event.mouseButton.y + spriteRect.height/2);
    break;
  }
  }
}

Mouse::Type Mouse::getType() const
{
  return m_type;
}
