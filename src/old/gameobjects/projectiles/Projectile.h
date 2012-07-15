/* 

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

class Projectile {

	public:

		void init();
		void draw();
		int draw_x();
		int draw_y();
		void think(); // think (when dying, doing damage, etc)
		void think_move(); // thinking for movement

		bool bAlive; // alive or not?, when dying, a particle is created

		int iCell; // cell of bullet
		int iType; // type of bullet

		int iOwnerUnit; // unit who shoots
		int iOwnerStructure;// structure who shoots (rocket turret?)
		int iPlayer; // what player shot this? (for damage control)


		// when set, the bullet will track down the bastard, but only for a while
		int iHoming; // homing to unit...
		int TIMER_homing; // when timer set, > 0 means homing

		// Movement
		int iGoalCell; // the goal cell (goal of path)
		float iOffsetX; // X offset
		float iOffsetY; // Y offset

		int iFrame; // frame for rockets

		int TIMER_frame;

	private:

};

