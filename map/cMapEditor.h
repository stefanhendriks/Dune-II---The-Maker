/*
 * cMapEditor.h
 *
 *  Created on: 16 nov. 2010
 *      Author: Stefan
 */

// the map editor is able to change map charactaristics

#ifndef CMAPEDITOR_H_
#define CMAPEDITOR_H_

class cMapEditor {
	public:
		cMapEditor();
		virtual ~cMapEditor();

		void smoothCell(int cell);		// smooth a specific cell
		void smoothMap();				// smooth the entire map
		void smoothAroundCell(int cell);	// smooth cells above, under, left and right given cell
		void removeSingleRockSpots();		// when single rock cells have less than 3 neighbors, remove them

		void createField(int cell, int terraintType, int size); // create a field on the map

		/**
		 * Sets/changes cell state. If cell < 0 will do nothing.
		 *
		 * @param cell
		 * @param terrainType
		 * @param tile
		 */
		void createCell(int cell, int terrainType, int tile);

		bool isSpecificTerrainType(int cell, int terrainType);

		bool isBelowSpecificTerrainType(int sourceCell, int terrainType);
		bool isAboveSpecificTerrainType(int sourceCell, int terrainType);
		bool isLeftSpecificTerrainType(int sourceCell, int terrainType);
		bool isRightSpecificTerrainType(int sourceCell, int terrainType);

		int getDefaultTerrainIndex(bool up, bool down, bool left, bool right);
		int getWallTerrainIndex(bool up, bool down, bool left, bool right);

		int smoothRockCell(int cell);
		int smoothMountainCell(int cell);
		int smoothSpiceCell(int cell);
		int smoothSpiceHillCell(int cell);
		int smoothSandHillCell(int cell);
		int smoothWallCell(int cell);

	private:

};

#endif /* CMAPEDITOR_H_ */
