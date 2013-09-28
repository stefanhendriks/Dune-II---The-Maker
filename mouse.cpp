#include "mouse.h"

Mouse::Mouse()
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

#include "mouse.h"
