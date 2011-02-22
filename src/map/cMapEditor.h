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

		void smoothCell(int cell);
		void smoothMap();
		void smoothCellsAroundCell(int cell);
		void makeRockCellsSandCellWhenRockCellHasLessThanThreeNeighbouringRockCells();

		void createField(int cell, int terraintType, int size);
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

		void clearMap(int terrainType);
	private:

};

#endif /* CMAPEDITOR_H_ */
