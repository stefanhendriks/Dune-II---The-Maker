#ifndef HOUSES_H
#define HOUSES_H

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

int getDefaultColor(House house);

#endif // HOUSES_H
