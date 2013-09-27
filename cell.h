#ifndef CELL_H
#define CELL_H
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

enum class Terrain
{
    Sand = 0,
    Hill = 1,
    Rock = 2,
    Spice = 3,
    Mountain = 4,
    Spicehill = 5,
    Slab = 6
};

class Cell : public sf::Drawable {
public:

    Cell();

    void init(const sf::Texture& terrain, int row, int col);

    void setType(Terrain terrain_type);
    void setIndex(int tileIndex);

    bool shouldSmoothWithTerrainType(Cell* other);

    Terrain terrainType; // terrain type (sand, rock, etc)
    //bool occupied[MAP_MAX_LAYERS];
    //bool shrouded;


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(sprite);
    }

    int x, y;

private:

    static const int TILE_SIZE = 32; // squared

    sf::Sprite sprite;

};
#endif // CELL_H
