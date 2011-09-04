/* 

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2011 (c) code by Stefan Hendriks

 */

// GUI ROUTINES

struct sButton {
		int iX, iY;
		int iWidth, iHeight;
		bool bHover; //  hovers over this thing.
};

bool GUI_DRAW_FRAME(int x1, int y1, int width, int height);
bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height);

