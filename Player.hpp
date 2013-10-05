#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>
#include "Unit.hpp"
#include <map>

enum class House;

class Player
{
public:
    Player(House theHouse, int theId);

    sf::Color getColor() const;

    bool operator==(const Player& other) const;
    bool operator!=(const Player& other) const;

    void recolor(sf::Image& image) const;

    const sf::Texture& getTexture(Unit::Type type) const;


private:
    int id;
    House house;
    sf::Color color;
    mutable std::map<Unit::Type, sf::Texture> textures;
};

#endif // PLAYER_H
