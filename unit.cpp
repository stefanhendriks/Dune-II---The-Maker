#include <iostream>
#include "unit.h"

using namespace std;

Unit::Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, UnitMoveBehavior* move_behavior) {
  init(tileset, shadowset, map, 128 + rnd(256), 128 + rnd(256), 1 + rnd(5), move_behavior);
}

Unit::Unit(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, UnitMoveBehavior* move_behavior, int x, int y, int viewRange) {
  init(tileset, shadowset, map, x, y, viewRange, move_behavior);
}

Unit::~Unit() {
  SDL_FreeSurface(tileset);
  SDL_FreeSurface(shadowset);
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

bool Unit::is_within(const Rectangle& rectangle) {
  return rectangle.is_point_within(position);
}

bool Unit::is_on_air_layer() {
  return isOnLayer(MAP_LAYER_AIR);
}

bool Unit::is_on_ground_layer() {
  return isOnLayer(MAP_LAYER_GROUND);
}

bool Unit::is_point_within(const Point& point) {
  Rectangle current_area = Rectangle(position, (position + size));
  return current_area.is_point_within(point);
}

void Unit::moveUp() {
  updateMovePosition(Point(0, -TILE_SIZE));
}

void Unit::moveDown() {
  updateMovePosition(Point(0, TILE_SIZE));
}

void Unit::moveLeft() {
  updateMovePosition(Point(-TILE_SIZE, 0));
}

void Unit::moveRight() {
  updateMovePosition(Point(TILE_SIZE, 0));
}

void Unit::updateMovePosition(Point p) {
  this->next_move_position = this->next_move_position + p;
  this->desired_body_facing = desired_facing();
}

bool Unit::is_moving() {
  return position != next_move_position;
}

bool Unit::has_target() {
  return position != target;
}

bool Unit::should_turn_body() {
  return desired_body_facing != body_facing;
}

void Unit::stopMoving() {
  this->next_move_position = position;
}

int Unit::getDrawX() {
  return position.x + offset_x;
}

int Unit::getDrawY() {
  return position.y + offset_y;
}

bool Unit::isOnLayer(short layer) {
  return this->move_behavior->is_layer(layer);
}

void Unit::order_move(Point target) {
  // snap coordinates
  int y = (target.y / TILE_SIZE) * TILE_SIZE;
  int x = (target.x / TILE_SIZE) * TILE_SIZE;
  this->target = Point(x,y);
}

void Unit::draw(SDL_Surface* screen, MapCamera* map_camera) {
  int draw_x = map_camera->screenCoordinateX(getDrawX());
  int draw_y = map_camera->screenCoordinateY(getDrawY());

  int tile_width = size.x;
  int tile_height = size.y;

  int src_x = this->body_facing * tile_width;
  int src_y = tile_height * this->anim_frame;

  Surface::draw(shadowset, screen, src_x, src_y, tile_width, tile_height, draw_x, draw_y, this->shadow_alpha);
  Surface::draw(tileset, screen, src_x, src_y, tile_width, tile_height, draw_x, draw_y);

  if (selected) Surface::draw(selected_bitmap, screen, draw_x, draw_y);

  if (target != position) {
    int draw_move_to_x = map_camera->screenCoordinateX(target.x);
    int draw_move_to_y = map_camera->screenCoordinateY(target.y);
    lineRGBA(screen, draw_x + 16, draw_y + 16, draw_move_to_x, draw_move_to_y, 255, 0, 255, 255);
  }
}

void Unit::init(SDL_Surface* tileset, SDL_Surface* shadowset, Map* map, int world_x, int world_y, int viewRange, UnitMoveBehavior* move_behavior) {
  this->selected = false;
  this->is_infantry = false;
  this->selected_bitmap = Surface::load("graphics/selected.bmp", 255, 0, 255);
  this->tileset = tileset;
  this->shadowset = shadowset;
  this->body_facing = rnd(FACINGS);
  this->desired_body_facing = this->body_facing;
  this->view_range = viewRange;
  this->position = Point(world_x, world_y);
  this->target = this->position;
  this->next_move_position = this->position;
  this->prev_position = this->position;
  this->map = map;
  this->move_behavior = move_behavior;
  this->move_behavior->occupyCell(this->position);
  this->map->removeShroud(this->position, this->view_range);
  this->shadow_alpha = 128;
  this->anim_frame = 0;

  int tile_height = 0, tile_width = 0;
  tile_width = tileset->w / FACINGS;

  // check if our assumption (width==height) is true for this tileset, and if not warn the user.
  if (tileset->h % tile_width > 0) {
    cerr << "WARNING: This tileset does not meet the requirement : tile height must equal width." << endl << endl;
    cerr << "Based on 8 sides of a unit, the calculated width of a tile on this tileset is " << tile_width << ". The general assumption is that the height of a tile must equal width. " << endl;
    cerr << "However the height of this tileset (" << tileset->h << ") was not one or more multiplications of " << tile_width << ". Its dimensions are " << tileset->h << "x" << tileset->w << ".";
    cerr << endl;

    // if it is less height than wide, we
    if (tileset->h < tile_width) {
      cerr << "Because the height of the tileset (" << tileset->h << ") is *lower* than the width of a calculated tile(=" << tile_width << ") (and therefor has no animations). We assume the height of the tileset of " << tileset->h << "." << endl;
      tile_height = tileset->h;
    } else {
      cerr << "Since the height of the tileset (" << tileset->h << ") is *greater* than the width of a calculated tile(=" << tile_width <<") (which means there could be animations) we assume the height equals the calculated width of " << tile_width << "." << endl;
      tile_height = tile_width;
    }
    cerr << endl;
  } else {
    tile_height = tile_width;
  }
  this->size = Point(tile_width, tile_height);

  // every pixel short/too much of the perfect tile size will be spread evenly
  this->offset_x = (TILE_SIZE - tile_width) / 2;
  this->offset_y = (TILE_SIZE - tile_height) / 2;
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
  int desired = desired_facing();

  int turning_left = (body_facing + FACINGS) - desired;
  if (turning_left > (FACINGS - 1)) turning_left -= FACINGS;
  int turning_right = abs(turning_left - FACINGS);

  if (turning_right < turning_left) {
    body_facing++;
  } else if (turning_left < turning_right) {
    body_facing--;
  } else {
    flipCoin() ? body_facing-- : body_facing++;
  }

  // wrap around
  if (body_facing < 0) body_facing += FACINGS;
  if (body_facing > 7) body_facing -= FACINGS;
}

void Unit::updateState() {

  if (should_turn_body()) {
    turn_body();
    return;
  }

  // think about movement
  if (!is_moving()) {
    if (prev_position != position) {
      move_behavior->unOccupyCell(prev_position);
      map->removeShroud(position, this->view_range);
    }

    if (has_target()) {
      // determine what the next adjecent tile should be
      if (target.x < position.x) moveLeft();
      if (target.x > position.x) moveRight();
      if (target.y < position.y) moveUp();
      if (target.y > position.y) moveDown();

      // check if we can move to this
      if (!move_behavior->canMoveTo(next_move_position)) {
        stopMoving();
      } else {
        // we can move to this tile, claim it
        move_behavior->occupyCell(next_move_position);
        prev_position = position;
      }

    } else {
      prev_position = position;
    }

  }

  // execute movement
  if (position.x < next_move_position.x) position.x++;
  if (position.x > next_move_position.x) position.x--;
  if (position.y < next_move_position.y) position.y++;
  if (position.y > next_move_position.y) position.y--;
}

//////////////////////////////////////////
// Unit Repository
//
//////////////////////////////////////////

UnitRepository::UnitRepository(Map* map) {
  this->map = map;

  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
    unit_animation[i] = NULL;
    unit_shadow[i] = NULL;
  }

  unit_animation[UNIT_QUAD] = Surface::load8bit("graphics/Unit_Quad.bmp");
  unit_shadow[UNIT_QUAD] = Surface::load("graphics/Unit_Quad_s.bmp", 255, 0, 255);

  unit_animation[UNIT_DEVASTATOR] = Surface::load8bit("graphics/Unit_Devastator.bmp");
  unit_shadow[UNIT_DEVASTATOR] = Surface::load("graphics/Unit_Devastator_s.bmp", 255, 0, 255);

  unit_animation[UNIT_FRIGATE] = Surface::load8bit("graphics/Unit_Frigate.bmp");
  unit_shadow[UNIT_FRIGATE] = Surface::load("graphics/Unit_Frigate_s.bmp", 255, 0, 255);

  unit_animation[UNIT_CARRYALL] = Surface::load8bit("graphics/Unit_Carryall.bmp");
  unit_shadow[UNIT_CARRYALL] = Surface::load("graphics/Unit_Carryall_s.bmp", 255, 0, 255);

  unit_animation[UNIT_TRIKE] = Surface::load8bit("graphics/Unit_Trike.bmp");
  unit_shadow[UNIT_TRIKE] = Surface::load("graphics/Unit_Trike_s.bmp", 255, 0, 255);

  unit_animation[UNIT_SOLDIER] = Surface::load8bit("graphics/Unit_Soldier.bmp");
  unit_shadow[UNIT_SOLDIER] = NULL;


  air_unit_move_behavior    = new AirUnitMovementBehavior(map);
  ground_unit_move_behavior = new GroundUnitMovementBehavior(map);
}

UnitRepository::~UnitRepository() {
  for (int i = 0; i < MAX_UNIT_TYPES; i++) {
    SDL_FreeSurface(unit_animation[i]);
    SDL_FreeSurface(unit_shadow[i]);
  }

  delete air_unit_move_behavior;
  delete ground_unit_move_behavior;
}

Unit* UnitRepository::create(int unitType, int house, int x, int y, int viewRange) {
  SDL_Surface* copy = Surface::copy(unit_animation[unitType]);
  int paletteIndexUsedForColoring = 144;
  int paletteIndex = paletteIndexUsedForColoring + (16 * house);
  SDL_SetColors(copy, &copy->format->palette->colors[paletteIndex], paletteIndexUsedForColoring, 8);

  UnitMoveBehavior *move_behavior = NULL;
  if (unitType == UNIT_FRIGATE) {
    move_behavior = air_unit_move_behavior;
  } else {
    move_behavior = ground_unit_move_behavior;
  }
  SDL_Surface* shadow_copy = Surface::copy(unit_shadow[unitType]);
  return new Unit(copy, shadow_copy, map, move_behavior, x, y, viewRange);
}
