#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H
#include <SFML/Graphics.hpp>

class SelectionBox : public sf::Drawable
{
  public:
    SelectionBox();
    void setTopLeft(const sf::Vector2f &point);
    void setBottomRight(const sf::Vector2f &point);
    void clear();

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    bool intersects(const sf::FloatRect rect) const;

  private:
    sf::RectangleShape box;
    bool pointSet;
};

#endif // SELECTIONBOX_H
