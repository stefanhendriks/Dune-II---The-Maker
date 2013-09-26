#ifndef CELL_H
#define CELL_H
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class Cell : public sf::Drawable {
public:

    Cell() {}

    void setTerrain(sf::Texture& terrain, int terrain_type, int tile) {
        this->terrain_type = terrain_type;
        this->tile = tile;

        const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;
        const int TILE_SIZE = 32; // squared

        int y = TILE_SIZE * terrain_type;
        int x = TILE_SIZE * (tile%17);
        sf::IntRect rect(x, y, TILE_SIZE, TILE_SIZE);

        sprite.setTexture(terrain);
        sprite.setTextureRect(rect);
        sprite.setPosition((this->x)*TILE_SIZE,(this->y)*TILE_SIZE);
    }

    // considers "this" as center opposed to other cell. (ego-centric)
    bool shouldSmoothWithTerrainType(Cell* other) {
        const int TERRAIN_TYPE_SAND     =  0;
        const int TERRAIN_TYPE_HILL     =  1;
        const int TERRAIN_TYPE_ROCK     =  2;
        const int TERRAIN_TYPE_SPICE    =  3;
        const int TERRAIN_TYPE_MOUNTAIN =  4;
        const int TERRAIN_TYPE_SPICEHILL = 5;
        const int TERRAIN_TYPE_SLAB     =  6;

        if (this->terrain_type == TERRAIN_TYPE_ROCK) {
            return other->terrain_type != TERRAIN_TYPE_MOUNTAIN &&
                    other->terrain_type != TERRAIN_TYPE_ROCK &&
                    other->terrain_type != TERRAIN_TYPE_SLAB;
        }
        if (this->terrain_type == TERRAIN_TYPE_MOUNTAIN) {
            return other->terrain_type != TERRAIN_TYPE_MOUNTAIN;
        }
        if (this->terrain_type == TERRAIN_TYPE_SLAB) {
            return other->terrain_type != TERRAIN_TYPE_MOUNTAIN &&
                    other->terrain_type != TERRAIN_TYPE_ROCK;
        }
        if (this->terrain_type == TERRAIN_TYPE_SPICE) {
            return other->terrain_type != TERRAIN_TYPE_SPICE &&
                    other->terrain_type != TERRAIN_TYPE_SPICEHILL;
        }
        if (this->terrain_type == TERRAIN_TYPE_SPICEHILL) {
            return other->terrain_type != TERRAIN_TYPE_SPICEHILL;
        }
        return other->terrain_type != this->terrain_type;
    }

    void setTile(int index){
        static const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;

        tile = (terrain_type * TILES_IN_ROW_ON_TERRAIN_SURFACE) + index;
    }

    int x, y;

    int tile; // tile to draw (one-dimension array)

    int terrain_type; // terrain type (sand, rock, etc)
    //bool occupied[MAP_MAX_LAYERS];
    //bool shrouded;


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(sprite);
    }

private:

    sf::Sprite sprite;

};
#endif // CELL_H
