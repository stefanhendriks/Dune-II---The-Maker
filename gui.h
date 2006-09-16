// GUI ROUTINES

struct sButton
{
    int iX, iY;
    int iWidth, iHeight;
    bool bHover; //  hovers over this thing.
};

bool GUI_DRAW_FRAME(int x1, int y1, int width, int height);
bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height);



