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

class MapCamera {

	public:
		MapCamera(int x, int y, SDL_Surface * screen);

		int getX() { return x; }
		int getY() { return y; }

		int getWidth() { return max_cells_width_on_screen; }
		int getHeight() { return max_cells_height_on_screen; }

	private:
		int x, y;
		int max_cells_width_on_screen;
		int max_cells_height_on_screen;

};

class Map {

   public:
     Map();

	  void setBoundaries(int max_width, int max_height);

     // TODO: camera coordinates
     void draw(SDL_Surface* tileset, SDL_Surface* screen, MapCamera* map_camera);

   private:
     Cell cells[MAP_MAX_SIZE]; 
	  int max_width;
	  int max_height;
	
};

#endif
