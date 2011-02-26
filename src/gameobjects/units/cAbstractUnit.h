/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

#ifndef CABSTRUNIT
#define CABSTRUNIT

class cAbstractUnit {

	public:
		cAbstractUnit();

		virtual void think() = 0;
		virtual void die() = 0;

		// properties of unit , implemented in concrete sub-class
		virtual bool isAirborn() = 0; // carry-all/orni
		virtual bool isUnderground() = 0; // worm
		virtual bool isTracked() = 0; // tank
		virtual bool isWheeled() = 0; // trike/quad
		virtual int getType() = 0; // return the int TYPE (matches the types defined in common.cpp)

		// drawing
		virtual bool draw() = 0;

		// return back coordinates
		int getCellX() {
			return iCellX;
		}
		int getCellY() {
			return iCellY;
		}

		int getOffsetX() {
			return iOffsetX;
		}
		int getOffsetY() {
			return iOffsetY;
		}

	private:
		float fExperience; // experience gained by unit
		bool bSelected; // selected or not?
		int iCell; // cell / location of unit
		int iPlayer; // to what player (id) does this unit belong?
		int iGroup; // to what group (made with CTRL-#) does this unit belong?
		bool isBeingTransported; // if true, the unit is not drawn as it is being transported by a Carry-All

		// location
		int iCellX; // X coordinate of its cell (location)
		int iCellY; // Y coordinate of its cell (location)

		// offset (x, y on cell basis)
		int iOffsetX;
		int iOffsetY;

};

#endif
