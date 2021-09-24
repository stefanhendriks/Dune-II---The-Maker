/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

class cParticle {
public:

    cParticle();
    ~cParticle();

    // Factory methods
    static void create(long x, long y, int iType, int iHouse, int iFrame);
    static int findNewSlot();

    void init();

    void draw();

    bool isWithinViewport(cRectangle *viewport);

    void think();

    bool isValid();
    int getLayer();

private:
    int getFrameWidth();
    int getFrameHeight();

    s_ParticleInfo& getParticleInfo() const;

    // x and y position to draw (absolute numbers).
    // This is the 'center' of a particle from a drawing perspective. Meaning the center of the
    // bitmap rendered is at this x,y coordinate
    long x, y;

    // the actual position + dimensions of particle
    cRectangle * dimensions;

    int iType;

    // used or not:
    bool bAlive;        // alive

    // Drawing effects
    sBitmap * bmp;          // pointer to bitmap to draw (not owner of bitmap!)
    int iAlpha;             // alpha

    int frameIndex;         // frame to draw

    // TIMERS
    int TIMER_frame;    // frame animation timers (when < 0, next frame, etc)
    // when TIMER_dead < 0, the last frame lets this thing die

    int TIMER_dead;     // when > -1, this timer will determine when the thing dies

    int iHousePal;      // when specified, use this palette for drawing (and its an 8 bit picture then!)

    int drawXBmpOffset;
    int drawYBmpOffset;

    bool oldParticle;

    int draw_x();

    int draw_y();

    void init(const s_ParticleInfo &particleInfo);
    void think_position();
    bool isUsingAlphaChannel() const;

    void recreateDimensions();
};


