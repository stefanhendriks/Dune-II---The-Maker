#ifndef MOUSE_H
#define MOUSE_H

#include <SFML/Graphics.hpp>

class Mouse : public sf::Drawable
{
public:
    Mouse();

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    void setPosition(const sf::Vector2f& point);

private:
    sf::Texture defaultTexture;
    sf::Texture moveTexture;

    sf::Sprite sprite;

};

#endif
