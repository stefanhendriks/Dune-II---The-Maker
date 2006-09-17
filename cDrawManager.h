/**
	Class responsible for all drawing
*/

class cDrawManager {

private:
	MME_SURFACE *buffer;

	void drawMouse();	
	void drawByGameState();

public:
	cDrawManager();

	void drawEverythingOnBuffer();
	void drawBufferOnScreen();
};
