/**
	Class responsible for all drawing
*/

class cDrawManager {

private:
	MME_SURFACE *buffer;

	void drawMouse();

public:
	cDrawManager();

	void drawEverythingOnBuffer();
	void drawBufferOnScreen();
};
