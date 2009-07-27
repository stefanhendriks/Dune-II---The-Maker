/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */


struct tCell
{
    int credits;        // harvesting
    int type;           // refers to gfxdata
    int tile;           // the tile to draw (* 32 (on x scale))
    int smudgetile;     // smudge tile id (drawn upon the tile)
    int smudgetype;     // smudge type id
    int health;         // hitpoints for specific tile (used for walls)
    bool passable;      // passable? > pathfinder (TRUE = default)

    // there are 4 'dimensions' of this cell. Each represents the perspective of
    // an entity that is either a unit, structure, the 'worms' dimension and the 'air' dimension.
    int id[4];          // ID of
                        // 0 = unit
                        // 1 = structure
                        // 2 = air
                        // 3 = worm
};


class cMap
{
public:

    void init();
    void smooth();
    void draw();
    void draw_shroud();
	void draw_structures(int iStage);
	void draw_particles(int layer);
    void draw_bullets();

    void think_minimap();

	bool occupied(int iCell);
	bool occupied(int iCll, int iUnitID);
	bool occupiedInDimension(int iCll, int dimension);
	bool occupiedByType(int iCell);

	void soft();

	void draw_minimap();
	void draw_units_2nd();
	void draw_units();

    void smudge_increase(int iType, int iCell);
    void draw_structures_health();


    void smooth_cell(int c);
    void smooth_spot(int c);
    void create_spot(int c, int type, int tile);
    int  mouse_cell();

	// Drawing
	int  mouse_draw_x();
	int  mouse_draw_y();

    void randommap();
    void think();
	void draw_think();
    void create_field(int type, int cell, int size);
    void clear_all();
    void clear_spot(int c, int size, int player);

	void set_pos(int x, int y, int cell);

	// Scrolling
    int scroll_x, scroll_y;            // scrolling (per tile)

    // static animation
    int iStaticFrame;
    int iStatus;                        // 0 = show minimap , -1 = no minimap (static animation, of going down)
                                        // 1 = static animation 'getting up'...
    int iTrans;                         // trasparancy...


    tCell cell[MAX_CELLS];

    void remove_id(int iIndex, int iIDType);    // removes ID of IDtype (unit/structure), etc

    bool iVisible[MAX_CELLS][MAX_PLAYERS];      // visibility for <player>

    private:


};


