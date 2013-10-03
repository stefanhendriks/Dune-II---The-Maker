#include <iostream>
#include "unit.h"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

Unit::Unit(const sf::Texture &texture, const sf::Texture &shadow_texture, const sf::Texture& selectedBitmap, float x, float y,int body_facing, Map& theMap, int theId):
  sprite(texture),
  shadow_sprite(shadow_texture),
  selectedSprite(selectedBitmap),
  selected(false),
  view_range(10),
  map(theMap),
  id(theId)
{
  setFacing(body_facing);
  this->desired_body_facing = body_facing;

  sprite.setPosition(x,y);
  shadow_sprite.setPosition(x,y);
  this->target = getCenter();

  selectedSprite.setPosition(x,y);
  map.removeShroud(getCenter(), view_range);
}

void Unit::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  target.draw(shadow_sprite, sf::BlendAlpha);
  target.draw(sprite);
  if (selected)
    target.draw(selectedSprite);

  if (has_target())
    target.draw(thor::Arrow(getCenter(), this->target - getCenter(), sf::Color(51,255,51,125)), sf::BlendAdd);
}

void Unit::select() {
  selected = true;
}

void Unit::unselect() {
  selected = false;
}

bool Unit::is_selected() {
  return selected;
}

bool Unit::has_target() const {
  return getCenter() != target;
}

bool Unit::should_turn_body() {
  return desired_body_facing != body_facing;
}


void Unit::order_move(sf::Vector2f target) {

  this->target = target;
  desired_body_facing = desired_facing();

}

void Unit::setFacing(int facing) {
  body_facing = facing;
  int size = sprite.getTexture()->getSize().x / FACINGS;
  sprite.setTextureRect({size * body_facing, 0, size, size});
  shadow_sprite.setTextureRect({size * body_facing, 0, size, size});
  shadow_sprite.setColor(sf::Color(255, 255, 255, 128));
}

int Unit::desired_facing() {
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

  return body_facing;
}

void Unit::turn_body() {
  int body_facing = this->body_facing;
  int desired = desired_facing();

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

void Unit::updateMovePosition(const std::vector<Unit>& units)  {
  if (has_target()) {
    float speed = 5.f;
    sf::Vector2f direction = target - getCenter();
    sf::Vector2f unitDirection = thor::unitVector(direction);
    float distance = thor::length(direction);
    if (distance < speed) speed = distance;
    sprite.move(speed*unitDirection);
    //do collision detection now

    sf::Vector2i mapPoint = map.toMapPoint(getCenter());
    if (map.getCell(mapPoint.x, mapPoint.y)->terrainType == Terrain::Mountain){
      sprite.move(-speed*unitDirection); //unmove
      return;
    }

    for (const auto& unit : units){
      if (id == unit.id) continue;

      if (sprite.getGlobalBounds().intersects(unit.sprite.getGlobalBounds())){
        sprite.move(-speed*unitDirection); //unmove
        return;
      }
    }



    shadow_sprite.move(speed*unitDirection);
    selectedSprite.move(speed*unitDirection);
    map.removeShroud(getCenter(), view_range);
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

void Unit::updateState(const std::vector<Unit> &units) {

  updateMovePosition(units);

  if (should_turn_body()) {
    turn_body();
    return;
  }

}
