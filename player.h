#ifndef PLAYER_H
#define PLAYER_H
#include <SFML/Graphics.hpp>

enum class House;

class Player
{
public:
    Player(House theHouse, int theId);

    sf::Color getColor();

    bool operator==(const Player& other) const;
    bool operator!=(const Player& other) const;

    void recolor(sf::Image& image) const;

    const sf::Texture& getTexture() const;


private:
    int id;
    House house;
    sf::Color color;
    sf::Texture texture;
};

#endif // PLAYER_H
