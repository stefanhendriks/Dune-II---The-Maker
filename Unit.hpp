#ifndef UNIT_H
#define UNIT_H

#include <memory>
#include "Messages.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Thor/Vectors/VectorAlgebra2D.hpp>

// used to calculate width of each 'tile' for a unit given a tilset
const int FACING_RIGHT      = 0;
const int FACING_UP_RIGHT   = 1;
const int FACING_UP         = 2;
const int FACING_LEFT_UP    = 3;
const int FACING_LEFT       = 4;
const int FACING_DOWN_LEFT  = 5;
const int FACING_DOWN       = 6;
const int FACING_RIGHT_DOWN = 7;
const int FACINGS           = 8;

struct TexturePack{
    const sf::Texture* unit;
    const sf::Texture* shadow;
    const sf::Texture* selected;
};

static_assert(std::is_pod<TexturePack>::value, "Texture pack is a pod");

class UnitMoveBehaviour;
class Unit;

struct MoveMessage : public Message {
    MoveMessage(const Unit& unit):
      Message(Messages::unitMove), unit(unit)
    {}

    const Unit& unit;
};

struct PreMoveMessage : public Message {
    PreMoveMessage(const Unit& unit):
      Message(Messages::premove), unit(unit)
    {}

    const Unit& unit;
};

class Unit : public sf::Drawable
{
  friend class UnitGroundMoveBehaviour;

  public:

    enum class Type {
      Trike,
      Quad,
      Devastator,
      Carryall,
      Frigate,
      Soldier
    };

    Unit(TexturePack pack, MessageSystem& messages, const sf::Vector2f& pos, int theId, Type type);

    void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    void updateState(const std::vector<Unit>& units, sf::Time dt);

    void orderMove(const sf::Vector2f &target);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenter() const;

    void select();
    void unselect();
    bool isSelected() const;

    mutable bool shouldMove;

    int getViewRange() const;

    Type type;

    int zIndex() const {
      if (type == Type::Carryall) return 1;
      return 0;
    }

  private:
    sf::Vector2f target;            // target of interest (move/attack, etc)

    sf::Sprite sprite;
    sf::Sprite shadowSprite;
    sf::Sprite selectedSprite;      //possibly static

    int viewRange;
    int bodyFacing;
    int desiredBodyFacing;

    bool selected;

    MessageSystem* messages;

    void setFacing(int facing);
    void turnBody();
    void updateMovePosition(const std::vector<Unit> &units, sf::Time dt);

    int  desiredFacing() const;
    bool hasTarget() const;
    bool shouldTurnBody() const;

    void triggerPreMove();
    void triggerMove();

    int id; //unique id for the unit

    UnitMoveBehaviour* moveBehaviour;
};

#endif
