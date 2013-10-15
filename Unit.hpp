#ifndef UNIT_H
#define UNIT_H

#include <memory>
#include "Entity.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Thor/Vectors/VectorAlgebra2D.hpp>

const int FACING_RIGHT = 0;
const int FACING_UP_RIGHT = 1;
const int FACING_UP = 2;
const int FACING_LEFT_UP = 3;
const int FACING_LEFT = 4;
const int FACING_DOWN_LEFT = 5;
const int FACING_DOWN = 6;
const int FACING_RIGHT_DOWN = 7;
const int FACINGS = 8;          // used to calculate width of each 'tile' for a unit given a tilset

struct TexturePack{
    const sf::Texture* unit;
    const sf::Texture* shadow;
    const sf::Texture* selected;
};

static_assert(std::is_pod<TexturePack>::value, "Texture pack is a pod");

class Unit;

struct MoveMessage : public Message{
    MoveMessage(const std::string& id, const Unit& unit):
      Message(id), unit(unit)
    {}

    const Unit& unit;
};

class Unit : public sf::Drawable
{

  public:

    enum class Type {
      Trike,
      Quad,
      Devastator,
      Carryall,
      Frigate,
      Soldier
    };

    Unit(TexturePack pack, MessageSystem& messages, const sf::Vector2f& pos, int theId);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void updateState(const std::vector<Unit>& units);

    void orderMove(const sf::Vector2f &target);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenter() const;

    void select();
    void unselect();
    bool isSelected() const;

    int getViewRange() const;

  private:
    sf::Vector2f target;            // target of interest (move/attack, etc)

    sf::Sprite sprite;
    sf::Sprite shadowSprite;
    sf::Sprite selectedSprite;      //possibly static

    int viewRange;
    int bodyFacing;
    int desiredBodyFacing;

    bool selected;

    MessageSystem& messages;

    void setFacing(int facing);
    void turnBody();
    void updateMovePosition(const std::vector<Unit> &units);

    int  desiredFacing() const;
    bool hasTarget() const;
    bool shouldTurnBody() const;

    int id; //unique id for the unit
};

#endif
