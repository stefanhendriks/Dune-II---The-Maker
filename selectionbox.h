#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H
#include <SFML/Graphics.hpp>

class SelectionBox : public sf::Drawable
{
public:
    SelectionBox();
    void setTopLeft(int x, int y);
    void setBottomRight(const sf::Vector2f &point);
    void clear();

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:
    sf::RectangleShape box;
};

#endif // SELECTIONBOX_H
