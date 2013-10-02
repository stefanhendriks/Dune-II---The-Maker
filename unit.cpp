#include <iostream>
#include "unit.h"

#include <Thor/Math.hpp>
#include <Thor/Shapes.hpp>

Unit::Unit(const sf::Texture &texture, const sf::Texture &shadow_texture, const sf::Texture& selectedBitmap, float x, float y,int body_facing, Map& theMap):
  sprite(texture),
  shadow_sprite(shadow_texture),
  selectedSprite(selectedBitmap),
  selected(false),
  shroud_range(10),
  map(theMap)
//owner(&thePlayer)
{
  this->selected = false;
  this->is_infantry = false;
  //this->shadowset = shadowset;
  setFacing(body_facing);
  this->desired_body_facing = body_facing;
  this->view_range = view_range;
  //    this->next_move_position = this->position;
  //    this->prev_position = this->position;
  //    this->map=map;
  //    this->move_behavior.reset(move_behavior);
  sprite.setPosition(x,y);
  shadow_sprite.setPosition(x,y);
  this->target = getCenter();

  //init(tileset, shadowset, map, world_x, world_y, view_range, move_behavior, sub_cell, tile_size, unit_size);
  selectedSprite.setPosition(x,y);
  map.removeShroud(getCenter(), shroud_range);
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

//bool Unit::is_within(const Rectangle& rectangle) {
//  return rectangle.is_point_within(Point(position.x, position.y));
//}

//bool Unit::is_on_air_layer() {
//  return isOnLayer(MAP_LAYER_AIR);
//}

//bool Unit::is_on_ground_layer() {
//  return isOnLayer(MAP_LAYER_GROUND);
//}

//bool Unit::is_point_within(const Point& point) {
//  Point start_of_click_area = Point(position.x, position.y) - Point(unit_size.x, unit_size.y);
//  Rectangle current_area = Rectangle(start_of_click_area, (Point(position.x, position.y) + unit_size));
//  return current_area.is_point_within(point);
//}

//void Unit::moveUp() {
//  updateMovePosition(Point(0, -TILE_SIZE));
//}

//void Unit::moveDown() {
//  updateMovePosition(Point(0, TILE_SIZE));
//}

//void Unit::moveLeft() {
//  updateMovePosition(Point(-TILE_SIZE, 0));
//}

//void Unit::moveRight() {
//  updateMovePosition(Point(TILE_SIZE, 0));
//}

//void Unit::updateMovePosition(sf::Vector2i p) {
//  // this makes it troublesome to move to 'sub cells' (other parts than center of cell, given by p from mouse)
//  this->next_move_position = this->next_move_position + p;
//  this->desired_body_facing = desired_facing();
//}

bool Unit::is_moving() {
  return position != next_move_position;
}

bool Unit::has_target() const {
  return getCenter() != target;
}

bool Unit::should_turn_body() {
  return desired_body_facing != body_facing;
}

void Unit::stopMoving() {
  this->next_move_position = position;
}

int Unit::getDrawX() {
  return (position.x + offset_x);
}

int Unit::getDrawY() {
  return (position.y + offset_y);
}

//bool Unit::isOnLayer(short layer) {
//  return this->move_behavior->is_layer(layer);
//}

void Unit::order_move(sf::Vector2f target) {
  // snap coordinates to center of cell
  //  int y = ((target.y / TILE_SIZE) * TILE_SIZE) + (TILE_SIZE / 2);
  //  int x = ((target.x / TILE_SIZE) * TILE_SIZE) + (TILE_SIZE / 2);

  //  this->target = sf::Vector2i(x, y);
  this->target = target;

  // then apply the same offset if given
  //  this->target = this->target + this->sub_position;
}

//const Player &Unit::getOwner() const
//{
//    return *owner;
//}

void Unit::setFacing(int facing) {
  body_facing = facing;
  int size = sprite.getTexture()->getSize().x / FACINGS;
  sprite.setTextureRect({size * body_facing, 0, size, size});
  shadow_sprite.setTextureRect({size * body_facing, 0, size, size});
  shadow_sprite.setColor(sf::Color(255, 255, 255, 128));
}

int Unit::desired_facing() {
  if (position == next_move_position) return body_facing;

  int nx = next_move_position.x;
  int ny = next_move_position.y;
  int x = position.x;
  int y = position.y;

  if (nx > x && ny == y) return FACING_RIGHT;
  if (nx < x && ny == y) return FACING_LEFT;
  if (nx == x && ny > y) return FACING_DOWN;
  if (nx == x && ny < y) return FACING_UP;

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

void Unit::updateMovePosition()  {
  if (has_target()) {
    float speed = 5.f;
    sf::Vector2f direction = target - getCenter();
    sf::Vector2f unitDirection = thor::unitVector(direction);
    float distance = thor::length(direction);
    if (distance < speed) speed = distance;
    sprite.move(speed*unitDirection);
    shadow_sprite.move(speed*unitDirection);
    selectedSprite.move(speed*unitDirection);
    map.removeShroud(getCenter(), shroud_range);
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

void Unit::updateState() {

  updateMovePosition();

  if (should_turn_body()) {
    turn_body();
    return;
  }

}
// think about movement
//  if (!is_moving()) {
//    if (prev_position != position) {
//      move_behavior->unOccupyCell(Point(prev_position.x, prev_position.y));
//      map->removeShroud(Point(position.x, position.y), this->view_range);
//    }

//    if (has_target()) {
//      // determine what the next adjecent tile should be
//      if (target.x < position.x) moveLeft();
//      if (target.x > position.x) moveRight();
//      if (target.y < position.y) moveUp();
//      if (target.y > position.y) moveDown();

//      // check if we can move to this
//      if (!move_behavior->canMoveTo(Point(next_move_position.x, next_move_position.y))) {
//        stopMoving();
//      } else {
//        // we can move to this tile, claim it
//        move_behavior->occupyCell(Point(next_move_position.x, next_move_position.y));
//        prev_position = position;

//        // TODO!??! - determine sub-cell / position for infantry if applicable?

//        // calculate the direction once
//        this->next_move_direction = Vector2D(position, next_move_position).normalize();
//      }

//    } else {
//      prev_position = position;
//    }

//  }

//  if (has_target()) {
//    Vector2D vector = Vector2D(position, next_move_position);

//    float distance = vector.length();
//    float speed = 2.0F;
//    if (distance < speed) speed = distance;

//    position.x = position.x + (next_move_direction.x * speed);
//    position.y = position.y + (next_move_direction.y * speed);
//  }

//}

////////////////////////////////////////////
//// Unit Repository
////
////////////////////////////////////////////
//UnitRepository::UnitRepository(Map* map) {
//  this->map = map;

//  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
//    unit_animation[i] = NULL;
//    unit_shadow[i] = NULL;
//  }

//  unit_animation[UNIT_QUAD] = Surface::load8bit("graphics/Unit_Quad.bmp");
//  unit_shadow[UNIT_QUAD] = Surface::load("graphics/Unit_Quad_s.bmp", 255, 0, 255);

//  unit_animation[UNIT_DEVASTATOR] = Surface::load8bit("graphics/Unit_Devastator.bmp");
//  unit_shadow[UNIT_DEVASTATOR] = Surface::load("graphics/Unit_Devastator_s.bmp", 255, 0, 255);

//  unit_animation[UNIT_FRIGATE] = Surface::load8bit("graphics/Unit_Frigate.bmp");
//  unit_shadow[UNIT_FRIGATE] = Surface::load("graphics/Unit_Frigate_s.bmp", 255, 0, 255);

//  unit_animation[UNIT_CARRYALL] = Surface::load8bit("graphics/Unit_Carryall.bmp");
//  unit_shadow[UNIT_CARRYALL] = Surface::load("graphics/Unit_Carryall_s.bmp", 255, 0, 255);

//  unit_animation[UNIT_TRIKE] = Surface::load8bit("graphics/Unit_Trike.bmp");
//  unit_shadow[UNIT_TRIKE] = Surface::load("graphics/Unit_Trike_s.bmp", 255, 0, 255);

//  unit_animation[UNIT_SOLDIER] = Surface::load8bit("graphics/Unit_Soldier.bmp");
//  unit_shadow[UNIT_SOLDIER] = NULL;

//  air_unit_move_behavior    = new AirUnitMoveBehavior(map);
//  ground_unit_move_behavior = new GroundUnitMoveBehavior(map);
//  foot_unit_move_behavior   = new FootUnitMoveBehavior(map);
//}

//UnitRepository::~UnitRepository() {
//  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
//    SDL_FreeSurface(unit_animation[i]);
//    SDL_FreeSurface(unit_shadow[i]);
//  }
//}

//Unit* UnitRepository::create(int unitType, int x, int y, int view_range, int sub_cell) {
//  SDL_Surface* copy = Surface::copy(unit_animation[unitType]);
//  int paletteIndex = thePlayer.getColor();
//  SDL_SetColors(copy, &copy->format->palette->colors[paletteIndex], 144, 8); //magic numbers

//  UnitMoveBehavior *move_behavior = NULL;
//  if (unitType == UNIT_FRIGATE) {
//    move_behavior = air_unit_move_behavior;
//  } else if (unitType == UNIT_SOLDIER) {
//    move_behavior = foot_unit_move_behavior;
//  } else {
//    move_behavior = ground_unit_move_behavior;
//  }
//  SDL_Surface* shadow_copy = Surface::copy(unit_shadow[unitType]);

//  int tile_height = 0, tile_width = 0;
//  tile_width = copy->w / FACINGS;

//  // check if our assumption (width==height) is true for this tileset, and if not warn the user.
//  if (copy->h % tile_width > 0) {
//    cerr << "WARNING: This copy does not meet the requirement : tile height must equal width." << endl << endl;
//    cerr << "Based on 8 sides of a unit, the calculated width of a tile on this copy is " << tile_width << ". The general assumption is that the height of a tile must equal width. " << endl;
//    cerr << "However the height of this copy (" << copy->h << ") was not one or more multiplications of " << tile_width << ". Its dimensions are " << copy->h << "x" << copy->w << ".";
//    cerr << endl;

//    // if it is less height than wide, we
//    if (copy->h < tile_width) {
//      cerr << "Because the height of the copy (" << copy->h << ") is *lower* than the width of a calculated tile(=" << tile_width << ") (and therefor has no animations). We assume the height of the copy of " << copy->h << "." << endl;
//      tile_height = copy->h;
//    } else {
//      cerr << "Since the height of the copy (" << copy->h << ") is *greater* than the width of a calculated tile(=" << tile_width <<") (which means there could be animations) we assume the height equals the calculated width of " << tile_width << "." << endl;
//      tile_height = tile_width;
//    }
//    cerr << endl;
//  } else {
//    tile_height = tile_width;
//  }

//  int unit_width = tile_width;
//  int unit_height = tile_height;

//  if (unitType == UNIT_SOLDIER) {
//    unit_width = unit_height = 8;
//  }

//  return new Unit(copy, shadow_copy, map, move_behavior, (x * TILE_SIZE) + (TILE_SIZE / 2), (y * TILE_SIZE) + (TILE_SIZE / 2), view_range, sub_cell, Point(tile_width, tile_height), Point(unit_width, unit_height), thePlayer);
//}
