#include "selectionbox.h"

SelectionBox::SelectionBox():
  pointSet(false)
{
  box.setOutlineThickness(3.f);
  box.setOutlineColor(sf::Color::White);
  box.setFillColor(sf::Color::Transparent);
}

void SelectionBox::setTopLeft(const sf::Vector2f &point)
{
  pointSet = true;
  box.setPosition(point);
}


void SelectionBox::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(box);
}

bool SelectionBox::intersects(const sf::FloatRect rect) const
{
  return box.getGlobalBounds().intersects(rect);
}


void SelectionBox::clear()
{
  pointSet = false;
  box.setSize(sf::Vector2f(0.f,0.f));
}


void SelectionBox::setBottomRight(const sf::Vector2f& point)
{
  if (!pointSet) return;
  box.setSize(point-box.getPosition());
}
