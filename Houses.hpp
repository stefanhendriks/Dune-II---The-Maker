#ifndef HOUSES_H
#define HOUSES_H
#include <SFML/Graphics/Color.hpp>

// note, these house numbers are based on the palette indices. As index 144 is harkonnen
// colors and the formula to actually copy the correct colors is based from 144 + house nr
// we just use this as a convenience.
enum class House
{
  Harkonnen = 0,
  Atreides = 1,
  Ordos = 2,
  Fremen = 3,
  Sardaukar = 4,
  Mercenary = 5
};

namespace Houses {

  sf::Color getDefaultColor(House house);

} //namespace Houses

#endif // HOUSES_H
