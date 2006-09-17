/**
	Mouse class

	note: mouse coordinates (x, y, z) are set by MultiMediaEngine. Therefor it is vital
	to have the EventManager do a poll for mouse coordinates.
*/

class cMouse {

private:
	int mouseX;
	int mouseY;
	int mouseZ;		/* Scrollwheel */

	/** Select box coordinates **/
	int mouseSelectX[2];
	int mouseSelectY[2];

public:
	cMouse();

	void think();
	void draw();
	
	/** Called by MME **/
	void setPoll(int x, int y, int z);
};