#ifndef MOUSE_H
#define MOUSE_H

#include <SFML/Graphics.hpp>

class Mouse : public sf::Drawable
{
public:

    enum class Type
    {
        Default,
        Move,
        Attack
    };


    Mouse();

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    void setPosition(const sf::Vector2f& point);
    void setType(Type type);

    sf::Vector2i getHotspot(sf::Event event) const;

    Type getType() const;

private:
    sf::Texture defaultTexture;
    sf::Texture moveTexture;

    sf::Sprite sprite;

    Type m_type;

};

#endif
