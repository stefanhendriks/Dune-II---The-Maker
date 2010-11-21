/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

class cMap
{
public:

	cMap();
	~cMap();

    void init();
    void smooth();
    void draw_bullets();

    void think_minimap();

	bool occupied(int iCell);
	bool occupied(int iCll, int iUnitID);
	bool occupiedInDimension(int iCll, int dimension);
	bool occupiedByType(int iCell);

	void soft();

	void draw_units_2nd();
	void draw_units();

    void smudge_increase(int iType, int iCell);

    int  mouse_cell();

	// Drawing
	int  mouse_draw_x();
	int  mouse_draw_y();

    void thinkInteraction();
	void draw_think();
    void clear_all();
    void clear_spot(int c, int size, int player);

    tCell cell[MAX_CELLS];

    void remove_id(int iIndex, int iIDType);    // removes ID of IDtype (unit/structure), etc

    void increaseScrollTimer() { TIMER_scroll++; }
    void resetScrollTimer() { TIMER_scroll=0; }
    bool isTimeToScroll() { return (TIMER_scroll > iScrollSpeed); }

    bool isVisible(int iCell, int iPlayer) { return iVisible[iCell][iPlayer]; }

    void setVisible(int iCell, int iPlayer, bool flag) { iVisible[iCell][iPlayer] = flag; }

    cCellCalculator * getCellCalculator() { return cellCalculator; }
    void resetCellCalculator();

    int getHeight() { return height; }
    int getWidth() { return width; }

	private:

    	bool iVisible[MAX_CELLS][MAX_PLAYERS];      // visibility for <player>

    	// Scrolling around map, timer based
    	int TIMER_scroll;
    	int iScrollSpeed;

    	// sizes of the map
    	int height, width;

    	cCellCalculator * cellCalculator;
};


