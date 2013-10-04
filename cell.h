#ifndef CELL_H
#define CELL_H
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <array>

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

class Cell {
public:

    static const int TILE_SIZE = 32; // squared

    Cell();

    void init(int row, int col);

    void setType(Terrain terrain_type);
    void setIndex(int tileIndex);
    void setShroudIndex(int tileIndex);

    bool shouldSmoothWithTerrainType(Cell& other) const;

    Terrain terrainType; // terrain type (sand, rock, etc)    
    bool shrouded;

    sf::Vertex getVertex(int index) const;
    sf::Vertex getShroudVertex(int index) const;

    sf::FloatRect getBounds() const;

    int x, y;

private:

    std::array<sf::Vertex, 4> vertices;
    std::array<sf::Vertex, 4> shroudVertices;
};
#endif // CELL_H
