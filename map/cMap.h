/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

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
	bool occupiedInDimension(int iCell, int dimension);
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

    /**
     * returns pointer to cell or nullptr when not valid
     * @param cellNr
     * @return
     */
    tCell * getCell(int cellNr) {
        if (cellNr < 0) return nullptr;
        if (cellNr >= MAX_CELLS) return nullptr;
        return &cell[cellNr];
    }
    
    int getCellType(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->type;
        return -1;
    }

    int cellGetIdFromLayer(int cellNr, int idLayer) {
        if (idLayer < 0 || idLayer >= 4) return -1; // safeguard layers
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->id[idLayer];
        return -1;
    }

    void cellSetIdForLayer(int cellNr, int idLayer, int id) {
        if (idLayer < 0 || idLayer >= 4) return; // safeguard layers
        tCell *pCell = getCell(cellNr);
        if (!pCell) return;
        pCell->id[idLayer] = id;
    }

    void cellResetIdFromLayer(int cellNr, int idLayer) {
        if (idLayer < 0 || idLayer >= 4) return; // safeguard layers
        tCell *pCell = getCell(cellNr);
        if (!pCell) return;
        pCell->id[idLayer] = -1;
    }

    int getCellTile(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->tile;
        return -1;
    }

    int getCellHealth(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->health;
        return -1;
    }

    int getCellCredits(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->credits;
        return -1;
    }

    bool isCellPassable(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (pCell) return pCell->passable;
        return false;
    }

    /**
     * returns if id of layer units is filled (if so, a unit is recorded to be on this cell)
     * @param cellNr
     * @return
     */
    int getCellIdUnitLayer(int cellNr) {
        return cellGetIdFromLayer(cellNr, MAPID_UNITS);
    }

    /**
     * returns if id of layer airborn units is filled (if so, an airborn unit is recorded to be on this cell)
     * @param cellNr
     * @return
     */
    int getCellIdAirUnitLayer(int cellNr) {
        return cellGetIdFromLayer(cellNr, MAPID_AIR);
    }

    /**
     * returns if id of layer worms is filled (if so, a worm is recorded to be on this cell)
     * @param cellNr
     * @return
     */
    int getCellIdWormsLayer(int cellNr) {
        return cellGetIdFromLayer(cellNr, MAPID_AIR);
    }

    /**
     * returns if id of layer structures is filled (if so, a structure is recorded to be on this cell)
     * @param cellNr
     * @return
     */
    int getCellIdStructuresLayer(int cellNr) {
        return cellGetIdFromLayer(cellNr, MAPID_STRUCTURES);
    }

    void cellTakeDamage(int cellNr, int damage) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->health -= damage;
    }

    void cellTakeCredits(int cellNr, int amount) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->credits -= amount;
    }

    void cellGiveHealth(int cellNr, int hp) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->health += hp;
    }

    void cellGiveCredits(int cellNr, int amount) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->credits += amount;
    }

    void cellChangeType(int cellNr, int type) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->type = type;
    }

    void cellChangeHealth(int cellNr, int value) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->health = value;
    }

    void cellChangeTile(int cellNr, int tile) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->tile = tile;
    }

    void cellChangeCredits(int cellNr, int value) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->credits = value;
    }

    void cellChangeSmudgeTile(int cellNr, int value) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->smudgetile = value;
    }

    void cellChangeSmudgeType(int cellNr, int value) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->smudgetype = value;
    }

    void cellChangePassable(int cellNr, bool value) {
        tCell *pCell = getCell(cellNr);
        if (pCell) pCell->passable = value;
    }

    void cellInit(int cellNr) {
        tCell *pCell = getCell(cellNr);
        if (!pCell) return; // bail

        pCell->credits = 0;
        pCell->health = 0;
        pCell->passable = true;
        pCell->tile = 0;
        pCell->type = TERRAIN_SAND;    // refers to gfxdata!

        pCell->smudgetile = -1;
        pCell->smudgetype = -1;

        // clear out the ID stuff
        memset(pCell->id, -1, sizeof(pCell->id));

        for (int i = 0; i < MAX_PLAYERS; i++) {
            setVisible(cellNr, i, false);
        }
    }

    void remove_id(int iIndex, int iIDType);    // removes ID of IDtype (unit/structure), etc

    void increaseScrollTimer() { TIMER_scroll++; }
    void resetScrollTimer() { TIMER_scroll=0; }
    bool isTimeToScroll() { return (TIMER_scroll > iScrollSpeed); }

    bool isVisible(int iCell, int iPlayer) {
        if (iCell < 0 || iCell >= MAX_CELLS) return false;
        if (iPlayer < 0 || iPlayer >= MAX_PLAYERS) return false;
        return iVisible[iCell][iPlayer];
    }

    void setVisible(int iCell, int iPlayer, bool flag) { iVisible[iCell][iPlayer] = flag; }

    cCellCalculator * getCellCalculator() { return cellCalculator; }
    void resetCellCalculator();

    int getHeight() { return height; }
    int getWidth() { return width; }

	private:
        tCell cell[MAX_CELLS];

    	bool iVisible[MAX_CELLS][MAX_PLAYERS];      // visibility for <player>

    	// Scrolling around map, timer based
    	int TIMER_scroll;
    	int iScrollSpeed;

    	// sizes of the map
    	int height, width;

    	cCellCalculator * cellCalculator;
};


