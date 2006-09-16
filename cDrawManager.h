/**
	Class responsible for all drawing
*/

class cDrawManager {

private:
	MME_SURFACE *buffer;

public:
	cDrawManager();

	void drawEverythingOnBuffer();
	void drawBufferOnScreen();
};