/**
	Mouse class

	note: mouse coordinates (x, y, z) are set by MultiMediaEngine. Therefor it is vital
	to have the EventManager do a poll for mouse coordinates.
*/

// Mouse status
#define MOUSE_STATUS_NORMAL  0
#define MOUSE_STATUS_MOVE    1
#define MOUSE_STATUS_ATTACK  2
#define MOUSE_STATUS_PLACE   3

class cMouse {

private:
	int mouseX;
	int mouseY;
	int mouseZ;		/* Scrollwheel */

	/** Select box coordinates **/
	int mouseSelectX[2];
	int mouseSelectY[2];

	/** Mouse status **/
	int status;

	/** Mouse sprite to draw **/
	int sprite;

public:
	cMouse();

	void think();
	void draw();
	
	/** Called by MME **/
	void setPoll(int x, int y, int z);
};