#include "houses.h"

sf::Color getDefaultColor(House house)
{
    switch (house) {
    default:
    case House::Harkonnen:
        return sf::Color(153,0,0);
    case House::Sardaukar:
        return sf::Color(51,0,102);
    }
}
