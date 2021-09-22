/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks
*/

#include "../../include/d2tmh.h"

// init
void cParticle::init() {
    bAlive=false;       // alive (if yes, it is in use, if not it can be used)
    iAlpha=-1;			// alpha number

    frameWidth=32;      // default width of frame
    frameHeight=frameWidth;

    x=0;
    y=0;              // x and y position to draw (absolute numbers)
    frameIndex=0;
    iType=0;          // type

    layer=0;          // default layer = 0 (on top)
    bmp = nullptr;

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
    int bmpOffset = (frameWidth / 2) * -1;
    return mapCamera->getWindowXPositionWithOffset(x, bmpOffset);
}

int cParticle::draw_y() {
    int bmpOffset = (frameHeight / 2) * -1;
    return mapCamera->getWindowYPositionWithOffset(y, bmpOffset);
}

// draw
void cParticle::draw() {
    s_ParticleInfo &particleInfo = sParticleInfo[iType];

    // drawX and drawY = is the draw coordinates but centered within cell (frameWidth/Height are the cell size?)
    int drawX = draw_x();
    int drawY = draw_y();

    // TODO: This is culling, and that responsibility should be somewhere else
    if (drawX < 0 || drawX > game.screen_x)
        return;

    // TODO: This is culling, and that responsibility should be somewhere else
    if (drawY < 0 || drawY > game.screen_y)
        return;

    // valid in boundaries
    BITMAP *temp = create_bitmap(frameWidth, frameHeight);

    // transparency
    clear_to_color(temp, makecol(255,0,255));

    // now blit it
    if (iHousePal > 0) {
        cPlayer &player = players[iHousePal];
        select_palette(player.pal);
    }

    if (bmp) {
        // new behavior
        allegroDrawer->blit(bmp, temp, (frameWidth * frameIndex), 0, frameWidth, frameHeight, 0, 0);
    } else {
        // old behavior
        allegroDrawer->blitFromGfxData(iType, temp, (frameWidth * frameIndex), 0, frameWidth, frameHeight, 0, 0);
    }

    // create proper sized bitmap
    int bmp_width = mapCamera->factorZoomLevel(frameWidth);
    int bmp_height = mapCamera->factorZoomLevel(frameHeight);

    // create bmp that is the stretched version of temp
    BITMAP *stretched = create_bitmap(bmp_width + 1, bmp_height + 1);
    clear_to_color(stretched, makecol(255, 0, 255)); // mask color
    allegroDrawer->maskedStretchBlit(temp, stretched, 0, 0, frameWidth, frameHeight, 0, 0, bmp_width, bmp_height);

    // temp is no longer needed
    destroy_bitmap(temp);

    if (isUsingAlphaChannel()) {
		if (particleInfo.usesAdditiveBlending) {
            fblend_add(stretched, bmp_screen, drawX, drawY, iAlpha);
		} else {
            set_trans_blender(0,0,0, iAlpha);
            draw_trans_sprite(bmp_screen, stretched, drawX, drawY);
        }
	} else {
        draw_sprite(bmp_screen, stretched, drawX, drawY);
    }

	set_trans_blender(0,0,0,128);

    destroy_bitmap(stretched);
}

bool cParticle::isUsingAlphaChannel() const { return iAlpha > -1 && iAlpha < 255; }


// think
void cParticle::think() {
	if (iType == D2TM_PARTICLE_OBJECT_BOOM01 || iType == D2TM_PARTICLE_OBJECT_BOOM02 || iType == D2TM_PARTICLE_OBJECT_BOOM03)
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

    if (iType == D2TM_PARTICLE_CARRYPUFF)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            frameIndex++;
            TIMER_frame=100+rnd(100);

            if (frameIndex > 5)
                bAlive=false;

            iAlpha-=16;
        }
    }

    // move
    if (iType == D2TM_PARTICLE_MOVE || iType == D2TM_PARTICLE_ATTACK)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame = 15;
            frameIndex++;
        }

        if (frameIndex > 9)
        {
            iAlpha-=35;
            frameIndex=9;

            if (iAlpha < 10)
                bAlive=false;
        }

        return;
    }

    if (iType == D2TM_PARTICLE_SIEGEDIE)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
         frameIndex++;


         if (frameIndex > 2)
         {
             frameIndex=2;
            iAlpha -=10;

            if (iAlpha < 10)
                bAlive=false;

            TIMER_frame=10;

         }
         else
             TIMER_frame=250;


        }
    }

	if (iType == D2TM_PARTICLE_EXPLOSION_TRIKE)
    {
        TIMER_frame--;
        if (TIMER_frame < 0)
        {
            iAlpha-=20;
            TIMER_frame=5;

            if (frameIndex < 1 && iAlpha < 220)
                frameIndex++;

            if (frameIndex >= 1 && iAlpha < 10)
                bAlive=false;

        }
    }

    if (iType == D2TM_PARTICLE_SMOKE ||
        iType == D2TM_PARTICLE_SMOKE_SHADOW)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (iAlpha < 255 && TIMER_dead > 0)
        {
            if (iType == D2TM_PARTICLE_SMOKE)
                iAlpha++;
            else if (iType == D2TM_PARTICLE_SMOKE_SHADOW)
                if (iAlpha < 128)
                    iAlpha++;
        }

        if (TIMER_frame < 0)
        {
            TIMER_frame = 50;
            frameIndex++;

            if (frameIndex > 2)
                frameIndex = 0;

            if (TIMER_dead < 0)
            {
                TIMER_dead = -1;
                if (iType == D2TM_PARTICLE_SMOKE_SHADOW)
                    iAlpha -=10;
                else
                    iAlpha -= 14;

                if (iAlpha < 10)
                    bAlive=false;
            }
        }
    }

    if (iType == D2TM_PARTICLE_TRACK_DIA || iType == D2TM_PARTICLE_TRACK_HOR || iType == D2TM_PARTICLE_TRACK_VER || iType == D2TM_PARTICLE_TRACK_DIA2)
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

    if (iType == D2TM_PARTICLE_BULLET_PUF)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame = 15;

            frameIndex++;

            if (frameIndex > 7)
                bAlive=false;
        }

    }

    if (iType == D2TM_PARTICLE_DEADINF01 ||
        iType == D2TM_PARTICLE_DEADINF02)
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

    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame=10;

            frameIndex++;

            if (frameIndex > 1)
            {
                frameIndex=1;

                iAlpha-=25;

                if (iAlpha < 25)
                    bAlive=false;
            }
        }
    }

    // normal rocket explosion (of launchers)
    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            TIMER_frame=10;

            frameIndex++;

            if (frameIndex > 3)
            {
                frameIndex=4;

                iAlpha-=35;

                if (iAlpha < 25)
                    bAlive=false;
            }
        }
    }

    if (iType == D2TM_PARTICLE_WORMTRAIL)
    {
        TIMER_frame--;

        if (TIMER_frame < 0)
        {
            if (frameIndex <= 3)
                TIMER_frame=100;
            else
                TIMER_frame=20;

            frameIndex++;

            if (frameIndex > 3)
            {
                frameIndex=4;

                iAlpha-=5;

                if (iAlpha < 5)
                    bAlive=false;
            }
        }
    }
    // tank explosion(s)
    if (iType == D2TM_PARTICLE_EXPLOSION_TANK_ONE ||
        iType == D2TM_PARTICLE_EXPLOSION_TANK_TWO ||
        iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE01 ||
        iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE02 ||
        iType == D2TM_PARTICLE_EXPLOSION_GAS)
    {
        TIMER_frame--;

        if (TIMER_frame < 0) {

            // delay for next frame(s)
            if (frameIndex <= 3) {
                TIMER_frame = 28;
            } else {
                TIMER_frame = 10;
            }

            frameIndex++;

            if (frameIndex > 3) {
                frameIndex = 4;

                iAlpha -= 15;

                if (iAlpha < 25) {
                    bAlive = false;
                }
            }
        }
    }

    if (iType == D2TM_PARTICLE_WORMEAT)
    {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            frameIndex++;

            // delay for next frame(s)
            if (frameIndex <= 3) {
                // begins slow, and speeds up after each frame
//                TIMER_frame = (4 - frameIndex) * 32;
                TIMER_frame = 80;
            } else {
                // frame will stick at 4 (eaten)
                frameIndex = 4;

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
    if (iType == D2TM_PARTICLE_EXPLOSION_BULLET)
    {
        TIMER_frame--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=10;
            frameIndex++;
            if (frameIndex > 1)
                bAlive=false;

        }

    }

    if (iType == D2TM_PARTICLE_SQUISH01 ||
        iType == D2TM_PARTICLE_SQUISH02 ||
        iType == D2TM_PARTICLE_SQUISH03 ||
        iType == D2TM_PARTICLE_EXPLOSION_ORNI)
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

    if (iType == D2TM_PARTICLE_TANKSHOOT || iType == D2TM_PARTICLE_SIEGESHOOT)
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



    if (iType == D2TM_PARTICLE_EXPLOSION_FIRE)
    {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0)
        {
            TIMER_frame=20;

            frameIndex++;



            if (frameIndex > 2)
                frameIndex=0;

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
    if (iType > -1 && iType < MAX_PARTICLE_TYPES) {
        s_ParticleInfo &sParticle = sParticleInfo[iType];
        pParticle.init(sParticle);
    } else {
        pParticle.init();
    }

    pParticle.x = x;
    pParticle.y = y;

    pParticle.iType = iType;

    // depending on type, set TIMER_dead & FRAME & FRAME TIME
    pParticle.frameIndex = 0;
    pParticle.TIMER_dead = 0;
    pParticle.TIMER_frame = 10;

    pParticle.iHousePal = iHouse;

    pParticle.bAlive = true;

    if (iType == D2TM_PARTICLE_EXPLOSION_TRIKE)
    {
        pParticle.iAlpha=255;
        pParticle.frameWidth=48;
        pParticle.frameHeight=48;
        PARTICLE_CREATE(x, y, D2TM_PARTICLE_OBJECT_BOOM03, -1, 0);
    }

    if (iType == D2TM_PARTICLE_SMOKE)
    {
        pParticle.iAlpha=0;
        pParticle.frameWidth=32;
        pParticle.frameHeight=48;
        pParticle.TIMER_dead=900;
        PARTICLE_CREATE(x+16, y+42, D2TM_PARTICLE_SMOKE_SHADOW, -1, -1);
    }

    if (iType == D2TM_PARTICLE_SMOKE_SHADOW)
    {
        pParticle.iAlpha=0;
        pParticle.frameWidth=36;
        pParticle.frameHeight=38;
        pParticle.TIMER_dead=1000;
    }

    if (iType == D2TM_PARTICLE_TRACK_DIA || iType == D2TM_PARTICLE_TRACK_HOR || iType == D2TM_PARTICLE_TRACK_VER || iType == D2TM_PARTICLE_TRACK_DIA2)
    {
        pParticle.iAlpha=128;
        pParticle.TIMER_dead=2000;
        pParticle.layer=1; // other layer
    }

    if (iType == D2TM_PARTICLE_BULLET_PUF)
    {
        pParticle.frameHeight = 18;
        pParticle.frameWidth  = 18;
        pParticle.iAlpha  = -1;
    }

    // trike exploding
    if (iType == D2TM_PARTICLE_EXPLOSION_FIRE)
    {
        pParticle.iAlpha=255;
        pParticle.TIMER_dead= 750 + rnd(500);
        pParticle.iAlpha = rnd(255);
    }

    // tanks exploding
    if (iType == D2TM_PARTICLE_WORMEAT) {
        pParticle.iAlpha=255;
        pParticle.frameWidth=48;
        pParticle.frameHeight=48;
        pParticle.TIMER_frame=80; // 2,5 * 32 (a tad slower than on 3 frames)
    }

    // tanks exploding
    if (iType == D2TM_PARTICLE_EXPLOSION_TANK_ONE ||
        iType == D2TM_PARTICLE_EXPLOSION_TANK_TWO ||
        iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE01 ||
        iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE02 ||
        iType == D2TM_PARTICLE_EXPLOSION_GAS)
    {

        if (iType != D2TM_PARTICLE_EXPLOSION_STRUCTURE01 && iType != D2TM_PARTICLE_EXPLOSION_STRUCTURE02)
            PARTICLE_CREATE(x, y, D2TM_PARTICLE_OBJECT_BOOM02, -1, 0);

        pParticle.iAlpha=255;
        pParticle.frameWidth=48;
        pParticle.frameHeight=48;
    }

    // worm trail
    if (iType == D2TM_PARTICLE_WORMTRAIL)
    {
        pParticle.iAlpha=96;
        pParticle.frameWidth=48;
        pParticle.frameHeight=48;

        pParticle.layer=1; // other layer
    }

    if (iType == D2TM_PARTICLE_DEADINF01 ||
        iType == D2TM_PARTICLE_DEADINF02)
    {
        pParticle.TIMER_dead = 500 + rnd(500);
        pParticle.iAlpha=255;
        pParticle.layer=1; // other layer
    }

    if (iType == D2TM_PARTICLE_TANKSHOOT || iType == D2TM_PARTICLE_SIEGESHOOT)
    {
        pParticle.frameIndex=iFrame;
        pParticle.TIMER_dead = 50;
        pParticle.iAlpha=128;
        pParticle.frameWidth=64;
        pParticle.frameHeight=64;
    }

    if (iType == D2TM_PARTICLE_SQUISH01 ||
        iType == D2TM_PARTICLE_SQUISH02 ||
        iType == D2TM_PARTICLE_SQUISH03 ||
        iType == D2TM_PARTICLE_EXPLOSION_ORNI)
    {
        pParticle.frameIndex=0;
        pParticle.TIMER_frame = 50;
        pParticle.frameWidth=32;
        pParticle.iAlpha=255;
        pParticle.frameHeight=32;
        pParticle.layer=1;
    }

    if (iType == D2TM_PARTICLE_SIEGEDIE) {
        pParticle.iAlpha=255;
        pParticle.TIMER_frame= 500 + rnd(300);

        PARTICLE_CREATE(x, y-18, D2TM_PARTICLE_EXPLOSION_FIRE, -1, -1);
        PARTICLE_CREATE(x, y-18, D2TM_PARTICLE_SMOKE, -1, -1);
        PARTICLE_CREATE(x, y, D2TM_PARTICLE_OBJECT_BOOM02, -1, 0);
    }

    if (iType == D2TM_PARTICLE_CARRYPUFF)
    {
        pParticle.frameIndex=0;
        pParticle.TIMER_frame= 50 + rnd(50);
        pParticle.frameWidth=96;
        pParticle.frameHeight=96;
        pParticle.layer=1;
        pParticle.iAlpha= 96 + rnd(64);
    }

    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET || iType == D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL) {
        pParticle.iAlpha = 255;
        // also create bloom
        PARTICLE_CREATE(x, y, D2TM_PARTICLE_OBJECT_BOOM03, iHouse, 0);
    }

    if (iType == D2TM_PARTICLE_OBJECT_BOOM01)
    {
        pParticle.iAlpha=240;
        pParticle.frameWidth=512;
        pParticle.frameHeight=512;
    }

    if (iType == D2TM_PARTICLE_OBJECT_BOOM02)
    {
        pParticle.iAlpha=230;
        pParticle.frameWidth=256;
        pParticle.frameHeight=256;
    }

    if (iType == D2TM_PARTICLE_OBJECT_BOOM03)
    {
        pParticle.iAlpha=220;
        pParticle.frameWidth=128;
        pParticle.frameHeight=128;
    }
}

int cParticle::findNewSlot() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particle[i].bAlive)
            return i;
    }

    return -1;
}

void cParticle::init(const s_ParticleInfo &particleInfo) {
    init();

    if (particleInfo.bmpIndex > -1) {
        bmp = game.getDataRepository()->getBitmapAt(particleInfo.bmpIndex);
    }

    if (particleInfo.startAlpha > -1 && particleInfo.startAlpha < 256) {
        iAlpha = particleInfo.startAlpha;
    } else {
        iAlpha = 255;
    }

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


