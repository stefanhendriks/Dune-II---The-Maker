/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@dune2k.com / stefanhen83@planet.nl
  Website: http://d2tm.duneii.com

  2001 - 2006 (c) code by Stefan Hendriks

  */

// - see in .cpp file what it is

//

class cParticle
{
public:

    // used or not:
    bool bAlive;        // alive

    // Drawing effects
    long x, y;        // x and y position to draw (absolute numbers)
    int iFrame;         // frame
    int iType;          // type
	int iAlpha;			// alpha

    int iWidth;         // width of frame
    int iHeight;        // height of frame
    
    // TIMERS
    int TIMER_frame;    // frame animation timers (when < 0, next frame, etc)
                        // when TIMER_dead < 0, the last frame lets this thing die

    int TIMER_dead;     // when > -1, this timer will determine when the thing dies

    int layer;          // when to draw
    
    int iHousePal;      // when specified, use this palette for drawing (and its an 8 bit picture then!)

    // ---------
    int draw_x();
    int draw_y();
    void draw();
    bool isValid();        
    void init();
    void think();       // particles do some sort of thinking , really! :)
};

void PARTICLE_CREATE(long x, long y, int iType, int iHouse, int iFrame);
