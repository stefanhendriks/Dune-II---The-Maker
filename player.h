#ifndef PLAYER_H
#define PLAYER_H

enum class House;

class Player
{
public:
    Player(House theHouse, int theId);

    int getColor();

    bool operator==(const Player& other) const;
    bool operator!=(const Player& other) const;


private:
    int id;
    House house;
};

#endif // PLAYER_H
