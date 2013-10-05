#ifndef MAP_H
#define MAP_H

#include <iostream>

#include <cmath>
#include "Cell.hpp"
#include <array>

#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>

#define DEV_DRAWGRID true

class Map : public sf::Drawable {

  public:

    static const int MAX_SIZE = 65536; // 256X256 map
    static const int MAX_WIDTH = 256;
    static const int MAX_HEIGHT = 256;
    static const int MIN_WIDTH = 32;
    static const int MIN_HEIGHT = 32;

    static const int LAYER_GROUND = 0;
    static const int LAYER_AIR = 1;
    static const int MAX_LAYERS = 2;

    Map(sf::Texture& terrain, sf::Texture& shroudEdges);

    void setBoundaries(int maxWidth, int maxHeight);

    void load(std::string file);

    void updateShroud();

    void prepare(const sf::Vector2f &topLeft) const; //prepares the arrays for drawing

    Cell& getCell(sf::Vector2i map_point) {
      return getCell(map_point.x, map_point.y);
    }

    Cell& getCell(int x, int y) {
      x = std::min(std::max(x, 0), (MAX_WIDTH-1));
      y = std::min(std::max(y, 0), (MAX_HEIGHT-1));
      int cell = (y * MAX_WIDTH) + x;
      return cells[cell];
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void drawShrouded(sf::RenderTarget& target, sf::RenderStates states) const;

    void removeShroud(sf::Vector2f world_point, int range) {
        sf::Vector2i mapPoint = toMapPoint(world_point);
        int x = mapPoint.x;
        int y = mapPoint.y;

        for (int cell_x = std::max(x - range, 0); cell_x <= std::min(x + range, maxWidth -1); cell_x++) {
            for (int cell_y = std::max(y - range, 0); cell_y <= std::min(y + range, maxHeight -1); cell_y++) {
                if (std::pow(cell_x - x, 2) + std::pow(cell_y - y, 2) <= std::pow(range, 2) + 1) {
                    getCell(cell_x, cell_y).shrouded = false;
                }
            }
        }
        updateShroud();
    }

    sf::Vector2i toMapPoint(const sf::Vector2f& world_point) const;

    int determineShroudEdge(Cell &c);

    int getMaxWidth() const;
    int getMaxHeight() const;
private:
    std::array<Cell, MAX_SIZE> cells; //why not vector? -Koji
    int maxWidth;
    int maxHeight;
    sf::Texture& terrain;
    sf::Texture& shroudEdges;
    mutable sf::VertexArray vertexArray;
    mutable sf::VertexArray shroudArray;

    int determineCellTile(Cell &cell);
    int determineTerrainTile(bool cellUp, bool cellDown, bool cellLeft, bool cellRight) const;
};

#endif
