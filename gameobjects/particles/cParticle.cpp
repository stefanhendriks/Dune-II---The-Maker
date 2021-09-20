/*
/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  ---------------------------------------------------
  Particle = every effect in the game:

 - smoke pufs
 - move indicator
 etc.

 most stuff = hardcoded

 Nice thing is, every particle = related with the bitmap id. Each bitmap can behave differently;
 by adding code to the 'think' function.

 TODO: make main class and sub-classes?
*/


#include "../../include/d2tmh.h"


// TODO: constructor/destructor

// init
void cParticle::init() {
    // used or not:
    bAlive=false;       // alive

    iAlpha=-1;			// alpha

    iWidth=32;          // default width of frame
    iHeight=iWidth;

    // Drawing effects
    x=0;
    y=0;              // x and y position to draw (absolute numbers)
    iFrame=0;         // frame
    iType=0;          // type

    layer=0;          // default layer = 0 (on top)

    iHousePal=-1;     // when specified, use this palette for drawing (and its an 8 bit picture then!)

    // TIMERS
    TIMER_frame=0;    // frame animation timers (when < 0, next frame, etc)
                      // when TIMER_dead < 0, the last frame lets this thing die

    TIMER_dead=0;     // when > -1, this timer will determine when the thing dies
}

// valid
bool cParticle::isValid() {
    return bAlive;
}

int cParticle::draw_x() {
    int bmpOffset = (iWidth/2) * -1;
    return mapCamera->getWindowXPositionWithOffset(x, bmpOffset);
}

int cParticle::draw_y() {
    int bmpOffset = (iHeight/2) * -1;
    return mapCamera->getWindowYPositionWithOffset(y, bmpOffset);
}

// draw
void cParticle::draw() {
    int dx = draw_x();
    int dy = draw_y();

    // TODO: This is culling, and that responsibility should be somewhere else
    if (dx < 0 || dx > game.screen_x)
        return;

    // TODO: This is culling, and that responsibility should be somewhere else
    if (dy < 0 || dy > game.screen_y)
        return;

    // valid in boundaries
    BITMAP *temp = create_bitmap(iWidth, iHeight);

    // transparency
    clear_to_color(temp, makecol(255,0,255));

    // now blit it
    if (iHousePal > 0) {
        cPlayer &player = players[iHousePal];
        select_palette(player.pal);
    }

    allegroDrawer->blitFromGfxData(iType, temp, (iWidth * iFrame), 0, iWidth, iHeight, 0, 0);

    // create proper sized bitmap
    int bmp_width = mapCamera->factorZoomLevel(iWidth);
    int bmp_height = mapCamera->factorZoomLevel(iHeight);

    // create bmp that is the stretched version of temp
    BITMAP *stretched = create_bitmap(bmp_width + 1, bmp_height + 1);
    clear_to_color(stretched, makecol(255, 0, 255)); // mask color
    allegroDrawer->maskedStretchBlit(temp, stretched, 0, 0, iWidth, iHeight, 0, 0, bmp_width, bmp_height);

    // temp is no longer needed
    destroy_bitmap(temp);

    // drawX and drawY = is the draw coordinates but centered within cell (iWidth/Height are the cell size?)
    int drawX = dx;
    int drawY = dy;

    if (iAlpha > -1) {
		if (iType != OBJECT_BOOM01 && iType != OBJECT_BOOM02 && iType != OBJECT_BOOM03) {
            set_trans_blender(0,0,0, iAlpha);
			draw_trans_sprite(bmp_screen, stretched, drawX, drawY);
		} else {
            fblend_add(stretched, bmp_screen, drawX, drawY, iAlpha);
        }
	} else {
        draw_sprite(bmp_screen, stretched, drawX, drawY);
    }

	set_trans_blender(0,0,0,128);

    destroy_bitmap(stretched);
}


// think
void cParticle::think() {
	if (iType == OBJECT_BOOM01 || iType == OBJECT_BOOM02 || iType == OBJECT_BOOM03)
	{
		TIMER_frame++;

		if (TIMER_frame > 0)
		{
			TIMER_frame=0;

			iAlpha-=2;

			if (iAlpha < 1)
				bAlive=false;
		}
	}

	// repair blink
	if (iType == PARTYPE_REPAIRBLINK)
	{
		// lower iAlpha

		TIMER_frame++;

		if (TIMER_frame > 5)
		{

		 iAlpha--;
		 TIMER_frame = 0;

		if (iAlpha < 1)
			bAlive=false;
        }

		return;
	}

    if (iType == OBJECT_CARRYPUFF)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            iFrame++;
            TIMER_frame=100+rnd(100);

            if (iFrame > 5)
                bAlive=false;

            iAlpha-=16;
        }
    }

    // move
    if (iType == MOVE_INDICATOR || iType == ATTACK_INDICATOR)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame = 15;
            iFrame++;
        }

        if (iFrame > 9)
        {
            iAlpha-=35;
            iFrame=9;

            if (iAlpha < 10)
                bAlive=false;
        }

        return;
    }

    if (iType == OBJECT_SIEGEDIE)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
         iFrame++;


         if (iFrame > 2)
         {
            iFrame=2;
            iAlpha -=10;

            if (iAlpha < 10)
                bAlive=false;

            TIMER_frame=10;

         }
         else
             TIMER_frame=250;


        }
    }

	if (iType == EXPLOSION_TRIKE)
    {
        TIMER_frame--;
        if (TIMER_frame < 0)
        {
            iAlpha-=20;
            TIMER_frame=5;

            if (iFrame < 1 && iAlpha < 220)
                iFrame++;

            if (iFrame >= 1 && iAlpha < 10)
                bAlive=false;

        }
    }

    if (iType == OBJECT_SMOKE ||
        iType == OBJECT_SMOKE_SHADOW)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (iAlpha < 255 && TIMER_dead > 0)
        {
            if (iType == OBJECT_SMOKE)
                iAlpha++;
            else if (iType == OBJECT_SMOKE_SHADOW)
                if (iAlpha < 128)
                    iAlpha++;
        }

        if (TIMER_frame < 0)
        {
            TIMER_frame = 50;
            iFrame++;

            if (iFrame > 2)
                iFrame = 0;

            if (TIMER_dead < 0)
            {
                TIMER_dead = -1;
                if (iType == OBJECT_SMOKE_SHADOW)
                    iAlpha -=10;
                else
                    iAlpha -= 14;

                if (iAlpha < 10)
                    bAlive=false;
            }
        }
    }

    if (iType == TRACK_DIA || iType == TRACK_HOR || iType == TRACK_VER || iType == TRACK_DIA2)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=10;
            if (rnd(100) < 10 && iAlpha > 192)
                iAlpha--;

            if (TIMER_dead > 0)
            {
                if (iAlpha < 255)
                    iAlpha+=2;

            }
            else
            {
                // its dying
                if (iAlpha > 10)
                    iAlpha-=10;
                else
                    bAlive=false;

            }
        }
    }

    if (iType == BULLET_PUF)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame = 15;

            iFrame++;

            if (iFrame > 7)
                bAlive=false;
        }

    }

    if (iType == OBJECT_DEADINF01 ||
        iType == OBJECT_DEADINF02)
    {
        TIMER_frame--;
        TIMER_dead--;

        if (TIMER_frame < 0)
        {
            if (TIMER_dead < 0)
            {
                TIMER_dead = -1;
                iAlpha-=5;

                if (iAlpha < 5)
                    bAlive=false;
            }

            TIMER_frame=10;
        }

    }

    if (iType == EXPLOSION_ROCKET_SMALL)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame=10;

            iFrame++;

            if (iFrame > 1)
            {
                iFrame=1;

                iAlpha-=25;

                if (iAlpha < 25)
                    bAlive=false;
            }
        }
    }

    // normal rocket explosion (of launchers)
    if (iType == EXPLOSION_ROCKET)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame=10;

            iFrame++;

            if (iFrame > 3)
            {
                iFrame=4;

                iAlpha-=35;

                if (iAlpha < 25)
                    bAlive=false;
            }
        }
    }

    if (iType == OBJECT_WORMTRAIL)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            if (iFrame <= 3)
                TIMER_frame=100;
            else
                TIMER_frame=20;

            iFrame++;

            if (iFrame > 3)
            {
                iFrame=4;

                iAlpha-=5;

                if (iAlpha < 5)
                    bAlive=false;
            }
        }
    }
    // tank explosion(s)
    if (iType == EXPLOSION_TANK_ONE ||
        iType == EXPLOSION_TANK_TWO ||
        iType == EXPLOSION_STRUCTURE01 ||
        iType == EXPLOSION_STRUCTURE02 ||
        iType == EXPLOSION_GAS)
    {
        TIMER_frame--;

        if (TIMER_frame < 0) {

            // delay for next frame(s)
            if (iFrame <= 3) {
                TIMER_frame = 28;
            } else {
                TIMER_frame = 10;
            }

            iFrame++;

            if (iFrame > 3) {
                iFrame = 4;

                iAlpha -= 15;

                if (iAlpha < 25) {
                    bAlive = false;
                }
            }
        }
    }

    if (iType == OBJECT_WORMEAT)
    {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            iFrame++;

            // delay for next frame(s)
            if (iFrame <= 3) {
                // begins slow, and speeds up after each frame
//                TIMER_frame = (4 - iFrame) * 32;
                TIMER_frame = 80;
            } else {
                // frame will stick at 4 (eaten)
                iFrame = 4;

                // fade out
                iAlpha -= 15;

                // until particle dies
                if (iAlpha < 25) {
                    bAlive = false;
                }
            }
        }
    }

    // bullets of tanks explode
    if (iType == EXPLOSION_BULLET)
    {
        TIMER_frame--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=10;
            iFrame++;
            if (iFrame > 1)
                bAlive=false;

        }

    }

    if (iType == EXPLOSION_SQUISH01 ||
        iType == EXPLOSION_SQUISH02 ||
        iType == EXPLOSION_SQUISH03 ||
        iType == EXPLOSION_ORNI)
    {
        TIMER_frame--;
        if (TIMER_frame < 0)
        {
           if (iAlpha > 5)
               iAlpha-=5;
           else
               bAlive=false;

           TIMER_frame=50;
        }
    }

    if (iType == OBJECT_TANKSHOOT || iType == OBJECT_SIEGESHOOT)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=2;
            if (TIMER_dead > 0)
            {
                if (iAlpha+5 < 255)
                    iAlpha += 5;
            }
            else
            {
                TIMER_dead=-1;
            iAlpha-=15;

            if (iAlpha < 10)
                bAlive=false;
            }
        }
    }



    if (iType == EXPLOSION_FIRE)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=20;

            iFrame++;



            if (iFrame > 2)
                iFrame=0;

            if (TIMER_dead < 0)
            {
                iAlpha-=20;
                if (iAlpha < 10)
                    bAlive=false;
            }
            else
            {
             if (iAlpha+15 < 255)
                 iAlpha +=15;
             else
                 iAlpha=255;
            }

        }
    }

}

void cParticle::create(long x, long y, int iType, int iHouse, int iFrame) {
    int iNewId = findNewSlot();

    if (iNewId < 0)
        return;

    cParticle &pParticle = particle[iNewId];
    pParticle.init();

    pParticle.x = x;
    pParticle.y = y;

    pParticle.iType = iType;

    // depending on type, set TIMER_dead & FRAME & FRAME TIME
    pParticle.iFrame = 0;
    pParticle.TIMER_dead = 0;
    pParticle.TIMER_frame = 10;

    pParticle.iHousePal = iHouse;

    pParticle.iAlpha = -1;

    pParticle.bAlive = true;

    if (iType == PARTYPE_REPAIRBLINK)
        pParticle.iAlpha = 255;

    if (iType == MOVE_INDICATOR || iType == ATTACK_INDICATOR)
        pParticle.iAlpha = 128;

    if (iType == EXPLOSION_TRIKE)
    {
        pParticle.iAlpha=255;
        pParticle.iWidth=48;
        pParticle.iHeight=48;
        PARTICLE_CREATE(x, y, OBJECT_BOOM03, -1, 0);
    }

    if (iType == OBJECT_SMOKE)
    {
        pParticle.iAlpha=0;
        pParticle.iWidth=32;
        pParticle.iHeight=48;
        pParticle.TIMER_dead=900;
        PARTICLE_CREATE(x+16, y+42, OBJECT_SMOKE_SHADOW, -1, -1);
    }

    if (iType == OBJECT_SMOKE_SHADOW)
    {
        pParticle.iAlpha=0;
        pParticle.iWidth=36;
        pParticle.iHeight=38;
        pParticle.TIMER_dead=1000;
    }

    if (iType == TRACK_DIA || iType == TRACK_HOR || iType == TRACK_VER || iType == TRACK_DIA2)
    {
        pParticle.iAlpha=128;
        pParticle.TIMER_dead=2000;
        pParticle.layer=1; // other layer
    }

    if (iType == BULLET_PUF)
    {
        pParticle.iHeight = 18;
        pParticle.iWidth  = 18;
        pParticle.iAlpha  = -1;
    }

    // bullets of tanks explode
    if (iType == EXPLOSION_BULLET)
    {

    }

    // trike exploding
    if (iType == EXPLOSION_FIRE)
    {
        pParticle.iAlpha=255;
        pParticle.TIMER_dead= 750 + rnd(500);
        pParticle.iAlpha = rnd(255);
    }

    // tanks exploding
    if (iType == OBJECT_WORMEAT) {
        pParticle.iAlpha=255;
        pParticle.iWidth=48;
        pParticle.iHeight=48;
        pParticle.TIMER_frame=80; // 2,5 * 32 (a tad slower than on 3 frames)
    }

    // tanks exploding
    if (iType == EXPLOSION_TANK_ONE ||
        iType == EXPLOSION_TANK_TWO ||
        iType == EXPLOSION_STRUCTURE01 ||
        iType == EXPLOSION_STRUCTURE02 ||
        iType == EXPLOSION_GAS)
    {

        if (iType != EXPLOSION_STRUCTURE01 && iType != EXPLOSION_STRUCTURE02)
            PARTICLE_CREATE(x, y, OBJECT_BOOM02, -1, 0);

        pParticle.iAlpha=255;
        pParticle.iWidth=48;
        pParticle.iHeight=48;
    }

    // worm trail
    if (iType == OBJECT_WORMTRAIL)
    {
        pParticle.iAlpha=96;
        pParticle.iWidth=48;
        pParticle.iHeight=48;

        pParticle.layer=1; // other layer
    }

    if (iType == OBJECT_DEADINF01 ||
        iType == OBJECT_DEADINF02)
    {
        pParticle.TIMER_dead = 500 + rnd(500);
        pParticle.iAlpha=255;
        pParticle.layer=1; // other layer
    }

    if (iType == OBJECT_TANKSHOOT || iType == OBJECT_SIEGESHOOT)
    {
        pParticle.iFrame=iFrame;
        pParticle.TIMER_dead = 50;
        pParticle.iAlpha=128;
        pParticle.iWidth=64;
        pParticle.iHeight=64;
    }

    if (iType == EXPLOSION_SQUISH01 ||
        iType == EXPLOSION_SQUISH02 ||
        iType == EXPLOSION_SQUISH03 ||
        iType == EXPLOSION_ORNI)
    {
        pParticle.iFrame=0;
        pParticle.TIMER_frame = 50;
        pParticle.iWidth=32;
        pParticle.iAlpha=255;
        pParticle.iHeight=32;
        pParticle.layer=1;
    }

    if (iType == OBJECT_SIEGEDIE) {
        pParticle.iAlpha=255;
        pParticle.TIMER_frame= 500 + rnd(300);

        PARTICLE_CREATE(x, y-18, EXPLOSION_FIRE, -1, -1);
        PARTICLE_CREATE(x, y-18, OBJECT_SMOKE, -1, -1);
        PARTICLE_CREATE(x, y, OBJECT_BOOM02, -1, 0);
    }

    if (iType == OBJECT_CARRYPUFF)
    {
        pParticle.iFrame=0;
        pParticle.TIMER_frame= 50 + rnd(50);
        pParticle.iWidth=96;
        pParticle.iHeight=96;
        pParticle.layer=1;
        pParticle.iAlpha= 96 + rnd(64);
    }

    if (iType == EXPLOSION_ROCKET || iType == EXPLOSION_ROCKET_SMALL) {
        pParticle.iAlpha = 255;
        // also create bloom
        PARTICLE_CREATE(x, y, OBJECT_BOOM03, iHouse, 0);
    }

    if (iType == OBJECT_BOOM01)
    {
        pParticle.iAlpha=240;
        pParticle.iWidth=512;
        pParticle.iHeight=512;
    }

    if (iType == OBJECT_BOOM02)
    {
        pParticle.iAlpha=230;
        pParticle.iWidth=256;
        pParticle.iHeight=256;
    }

    if (iType == OBJECT_BOOM03)
    {
        pParticle.iAlpha=220;
        pParticle.iWidth=128;
        pParticle.iHeight=128;
    }
}

int cParticle::findNewSlot() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particle[i].bAlive)
            return i;
    }

    return -1;
}

/**
 * Creates a particle at exact coordinates
 * @param x
 * @param y
 * @param iType
 * @param iHouse
 * @param iFrame
 */
void PARTICLE_CREATE(long x, long y, int iType, int iHouse, int iFrame) {
    cParticle::create(x, y, iType, iHouse, iFrame);
}


