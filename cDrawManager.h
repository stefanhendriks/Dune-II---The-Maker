/**
	Class responsible for all drawing
*/

class cDrawManager {

private:
	MME_SURFACE buffer;

	void drawMouse();
	void drawByGameState();

	void drawEverythingOnBuffer();
	void drawBufferOnScreen();
	
public:
	cDrawManager();
	MME_SURFACE getBuffer();

	void draw();
};
