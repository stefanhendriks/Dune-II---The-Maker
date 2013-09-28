#include "selectionbox.h"

SelectionBox::SelectionBox()
{
    box.setOutlineThickness(3.f);
    box.setOutlineColor(sf::Color::White);
    box.setFillColor(sf::Color::Transparent);
}

void SelectionBox::setTopLeft(int x, int y)
{
    box.setPosition(x, y);
}


void SelectionBox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(box);
}

bool SelectionBox::intersects(const sf::FloatRect rect) const
{
    return box.getLocalBounds().intersects(rect);
}


void SelectionBox::clear()
{
    box.setSize(sf::Vector2f(0.f,0.f));
}


void SelectionBox::setBottomRight(const sf::Vector2f& point)
{
    box.setSize(point-box.getPosition());
}
