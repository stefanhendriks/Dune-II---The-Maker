/*
 * cRandomMapGenerator.h
 *
 *  Created on: 16 nov. 2010
 *      Author: Stefan
 */

#ifndef CRANDOMMAPGENERATOR_H_
#define CRANDOMMAPGENERATOR_H_

class cRandomMapGenerator {
	public:
		cRandomMapGenerator();
		virtual ~cRandomMapGenerator();

		void generateRandomMap();

		void smoothCell(int cell);		// smooth a specific cell
		void smoothMap();				// smooth the entire map
		void smoothAroundCell(int cell);	// smooth cells above, under, left and right given cell
		void removeSingleRockSpots();		// when single rock cells have less than 3 neighbors, remove them

		void createField(int terrainType, int cell, int size); // create a field on the map

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

#endif /* CRANDOMMAPGENERATOR_H_ */
