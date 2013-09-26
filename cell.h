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

    void setTerrain(sf::Texture& terrain, Terrain terrain_type, int tile);

    // considers "this" as center opposed to other cell. (ego-centric)
    bool shouldSmoothWithTerrainType(Cell* other);

    void setTile(int index);

    int x, y;

    int tile; // tile to draw (one-dimension array)

    Terrain terrainType; // terrain type (sand, rock, etc)
    //bool occupied[MAP_MAX_LAYERS];
    //bool shrouded;


    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(sprite);
    }

private:

    sf::Sprite sprite;

};
#endif // CELL_H
