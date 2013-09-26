#ifndef CELL_H
#define CELL_H
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class Cell : public sf::Drawable {
public:

    Cell() {}

    void setTerrain(sf::Image& terrain, int terrain_type, int tile) {
        this->terrain_type = terrain_type;
        this->tile = tile;

        const int TILES_IN_ROW_ON_TERRAIN_SURFACE = 17;
        const int TILE_SIZE = 32; // squared

        int y = TILES_IN_ROW_ON_TERRAIN_SURFACE * terrain_type;
        int x = TILE_SIZE * tile;
        sf::IntRect rect = sf::IntRect(x, y, x + TILE_SIZE, y + TILE_SIZE);
        texture.loadFromImage(terrain, rect);
        sprite.setTexture(texture);
        sprite.setPosition((this->x)*TILE_SIZE,(this->y)*TILE_SIZE);
    }

    int tile; // tile to draw (one-dimension array)

    int terrain_type; // terrain type (sand, rock, etc)
    //bool occupied[MAP_MAX_LAYERS];
    //bool shrouded;
    int x, y;

    sf::Texture texture;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(sprite);
    }

    sf::Sprite sprite;

};
#endif // CELL_H
