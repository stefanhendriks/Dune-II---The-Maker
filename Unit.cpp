#include <iostream>
#include "Unit.hpp"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

Unit::Unit(const sf::Texture &texture, const sf::Texture &shadow_texture, const sf::Texture& selectedBitmap, const sf::Vector2f& pos, Map& theMap, int theId):
  sprite(texture),
  shadowSprite(shadow_texture),
  selectedSprite(selectedBitmap),
  viewRange(10),
  selected(false),
  map(theMap),
  id(theId)
{
  setFacing(FACING_UP);
  desiredBodyFacing = bodyFacing;

  sprite.setPosition(pos);
  shadowSprite.setPosition(pos);
  shadowSprite.setColor(sf::Color(255, 255, 255, 128));
  target = getCenter();

  selectedSprite.setPosition(pos);
  map.removeShroud(getCenter(), viewRange);
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

void Unit::updateMovePosition(const std::vector<Unit>& units)  {
  if (hasTarget()) {
    float speed = 5.f;
    sf::Vector2f direction = target - getCenter();
    sf::Vector2f unitDirection = thor::unitVector(direction);
    float distance = thor::length(direction);
    if (distance < speed) speed = distance;
    sprite.move(speed*unitDirection);

    //do collision detection now
    sf::Vector2i mapPoint = map.toMapPoint(getCenter());

    // for now it may seem that units are not blocked by terrain correctly.
    // however, units *do* get blocked, but they are being drawn over mountains
    // because they can move on a pixel perfect level, yet are being checked
    // against cells.
    if (map.getCell(mapPoint.x, mapPoint.y).terrainType == Terrain::Mountain) {
      sprite.move(-speed*unitDirection);
      return;
    }

    for (const auto& unit : units){
      if (id == unit.id) continue;

      if (sprite.getGlobalBounds().intersects(unit.sprite.getGlobalBounds())){
        sprite.move(-speed*unitDirection);
        return;
      }
    }

    shadowSprite.move(speed*unitDirection);
    selectedSprite.move(speed*unitDirection);
    map.removeShroud(getCenter(), viewRange);
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

  if (shouldTurnBody()) {
    turnBody();
    return;
  }

}
