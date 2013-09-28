#ifndef MOUSE_H
#define MOUSE_H

#include <SFML/Graphics.hpp>

enum class MouseType
{
    Default,
    Move,
    Attack
};

class Mouse : public sf::Drawable
{
public:
    Mouse();

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    void setPosition(const sf::Vector2f& point);
    void setType(MouseType type);

private:
    sf::Texture defaultTexture;
    sf::Texture moveTexture;

    sf::Sprite sprite;

};

#endif
