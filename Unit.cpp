#include <iostream>
#include "Unit.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

Unit::Unit(TexturePack pack, MessageSystem &messages, const sf::Vector2f& pos, int theId, Type type):
  shouldMove(false),
  sprite(*pack.unit),
  shadowSprite(*pack.shadow),
  selectedSprite(*pack.selected),
  viewRange(10),
  selected(false),
  messages(messages),
  id(theId),
  type(type)
{
  setFacing(FACING_UP);
  desiredBodyFacing = bodyFacing;

  sprite.setPosition(pos);
  shadowSprite.setPosition(pos);
  shadowSprite.setColor(sf::Color(255, 255, 255, 128));
  target = getCenter();

  selectedSprite.setPosition(pos);

  //send a fake move message to remove shroud on creation
  messages.triggerEvent(MoveMessage(*this));
}

void Unit::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(shadowSprite, sf::BlendAlpha);
  target.draw(sprite);
  if (selected)
    target.draw(selectedSprite);

  if (hasTarget())
    target.draw(thor::Arrow(getCenter(), this->target - getCenter(), sf::Color(51,255,51,125)), sf::BlendAdd);
}

void Unit::select() {
  selected = true;
}

void Unit::unselect() {
  selected = false;
}

bool Unit::isSelected() const {
  return selected;
}

int Unit::getViewRange() const
{
  return viewRange;
}

bool Unit::hasTarget() const {
  return getCenter() != target;
}

bool Unit::shouldTurnBody() const {
  return desiredBodyFacing != bodyFacing;
}


void Unit::orderMove(const sf::Vector2f& target) {

  this->target = target;
  desiredBodyFacing = desiredFacing();

}

void Unit::setFacing(int facing) {
  bodyFacing = facing;
  int size = sprite.getTexture()->getSize().x / FACINGS;
  sprite.setTextureRect({size * bodyFacing, 0, size, size});
  shadowSprite.setTextureRect({size * bodyFacing, 0, size, size});
}

int Unit::desiredFacing() const {
  //if (position == next_move_position) return body_facing;

  int nx = target.x;
  int ny = target.y;
  int x = getCenter().x;
  int y = getCenter().y;

  if (nx > x && (ny-y)<0.1) return FACING_RIGHT;
  if (nx < x && (ny-y)<0.1) return FACING_LEFT;
  if ((nx-x)<0.1 && ny > y) return FACING_DOWN;
  if ((nx-x)<0.1 && ny < y) return FACING_UP;

  if (nx > x && ny > y) return FACING_RIGHT_DOWN;
  if (nx < x && ny > y) return FACING_DOWN_LEFT;
  if (nx > x && ny < y) return FACING_UP_RIGHT;
  if (nx < x && ny < y) return FACING_LEFT_UP;

  return bodyFacing;
}

void Unit::turnBody() {
  int body_facing = this->bodyFacing;
  int desired = desiredFacing();

  int turning_left = (body_facing + FACINGS) - desired;
  if (turning_left > (FACINGS - 1)) turning_left -= FACINGS;
  int turning_right = abs(turning_left - FACINGS);

  if (turning_right < turning_left) {
    body_facing++;
  } else if (turning_left < turning_right) {
    body_facing--;
  } else {
    thor::random(0, 10) < 5 ? body_facing-- : body_facing++;
  }

  // wrap around
  if (body_facing < 0) body_facing += FACINGS;
  if (body_facing > 7) body_facing -= FACINGS;

  setFacing(body_facing);
}

void Unit::updateMovePosition(const std::vector<Unit>& units, sf::Time dt)  {
  if (hasTarget()) {
    if (type == Type::Carryall) { // HACK HACK
      float speed = dt.asSeconds() * 250.f;
      sf::Vector2f direction = target - getCenter();
      sf::Vector2f unitDirection = thor::unitVector(direction);
      float distance = thor::length(direction);
      if (distance < speed) speed = distance;
      sprite.move(speed*unitDirection);
      shadowSprite.move(speed*unitDirection);
      selectedSprite.move(speed*unitDirection);
      messages.triggerEvent(MoveMessage(*this));

    } else {
      float speed = dt.asSeconds() * 250.f;
      sf::Vector2f direction = target - getCenter();
      sf::Vector2f unitDirection = thor::unitVector(direction);
      float distance = thor::length(direction);
      if (distance < speed) speed = distance;
      sprite.move(speed*unitDirection);

      messages.triggerEvent(PreMoveMessage(*this));

      if (shouldMove){
        shouldMove = false;

        //collision detection with units still here
        for (const auto& unit : units){
          if (id == unit.id) continue;
          if (unit.type == Type::Carryall) continue; // HACK HACK

          if (sprite.getGlobalBounds().intersects(unit.sprite.getGlobalBounds())){
            sprite.move(-speed*unitDirection);
            return;
          }
        }

        shadowSprite.move(speed*unitDirection);
        selectedSprite.move(speed*unitDirection);
        messages.triggerEvent(MoveMessage(*this));

      }else{
        sprite.move(-speed*unitDirection);
      }
    }
  }
}

sf::FloatRect Unit::getBounds() const {
  return sprite.getGlobalBounds();
}

sf::Vector2f Unit::getPosition() const {
  return sprite.getPosition();
}

sf::Vector2f Unit::getCenter() const {
  sf::FloatRect spriteRect(sprite.getGlobalBounds());
  return (sprite.getPosition() + sf::Vector2f(spriteRect.width/2, spriteRect.height/2));
}

void Unit::updateState(const std::vector<Unit> &units, sf::Time dt) {

  updateMovePosition(units, dt);

  if (shouldTurnBody())
    turnBody();

}
