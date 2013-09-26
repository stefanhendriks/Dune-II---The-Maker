#ifndef PLAYER_H
#define PLAYER_H

enum class House;

class Player
{
public:
    Player(House theHouse, int theId);

    int getColor();


private:
    int id;
    House house;
};

#endif // PLAYER_H
