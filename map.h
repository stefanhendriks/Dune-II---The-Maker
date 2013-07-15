#ifndef MAP_H
#define MAP_H

const int MAP_MAX_SIZE = 65536; // 256X256 map
const int MAP_MAX_WIDTH = 256;
const int MAP_MAX_HEIGHT = 256;

class Cell {
  public:
    int tile; // tile to draw (one-dimension array)

    static int getCellIndex(int x, int y) { return (y * MAP_MAX_WIDTH) + x; }

};

class Map {

   public:
     Map();

	   void setBoundaries(int max_width, int max_height);

     Cell getCell(int index) { return cells[index]; }

     int getMaxWidth() { return max_width; }
     int getMaxHeight() { return max_height; }

   private:
    Cell cells[MAP_MAX_SIZE];
	  int max_width;
	  int max_height;

};

class MapCamera {

	public:
		MapCamera(int x, int y, SDL_Surface* screen, Map* map);

		int getX() { return x; }
		int getY() { return y; }

		int getWidth() { return max_cells_width_on_screen; }
		int getHeight() { return max_cells_height_on_screen; }

    void moveUp() { if (y > 0) y--; }
    void moveDown() { if (y < max_y()) y++; }
    void moveLeft() { if (x > 0) x--; }
    void moveRight() { if (y < max_x()) x++; }


    void draw(Map* map, SDL_Surface* tileset, SDL_Surface* screen);

	private:
		int x, y;
		int max_cells_width_on_screen;
		int max_cells_height_on_screen;
    int map_y_boundary;
    int map_x_boundary;

    int max_y() { return map_y_boundary - max_cells_height_on_screen; }
    int max_x() { return map_x_boundary - max_cells_width_on_screen; }
};
#endif
