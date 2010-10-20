/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2009 (c) code by Stefan Hendriks

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



#include "d2tmh.h"

// TODO: constructor/destructor

// init
void cParticle::init()
{
    // used or not:
    bAlive=false;        // alive

    iAlpha=-1;			// alpha

    iWidth=32;          // default width of frame
    iHeight=iWidth;
    // Drawing effects
    x=0;
    y=0;        // x and y position to draw (absolute numbers)
    iFrame=0;         // frame
    iType=0;          // type

    layer=0;        // default layer = 0 (on top)

    iHousePal=-1;      // when specified, use this palette for drawing (and its an 8 bit picture then!)

    // TIMERS
    TIMER_frame=0;    // frame animation timers (when < 0, next frame, etc)
                        // when TIMER_dead < 0, the last frame lets this thing die

    TIMER_dead=0;     // when > -1, this timer will determine when the thing dies
}

// valid
bool cParticle::isValid()
{
    return bAlive;
}

// absolute pixel position
int cParticle::draw_x()
{
    return (x - (mapCamera->getX()*32));
}

// absolute pixel position
int cParticle::draw_y()
{
    return (y - (mapCamera->getY()*32));
}

// draw
void cParticle::draw()
{

    int dx = draw_x();
    int dy = draw_y();

    if (dx < 0 || dx > game.screen_x)
        return;

    if (dy < 0 || dy > game.screen_y)
        return;

    /*
    if (iType == OBJECT_WORMTRAIL)
    {
        Shimmer((16/(iFrame+1)), dx, dy);
    }*/



    // valid in boundries
    BITMAP *temp = create_bitmap(iWidth,iHeight);

    // transparency
    clear_to_color(temp, makecol(255,0,255));

    // now blit it
    if (iHousePal < 0)
        blit((BITMAP *)gfxdata[iType].dat, temp, (iWidth*iFrame), 0, 0, 0, iWidth, iHeight);
    else
    {
        select_palette(  player[iHousePal].pal  );
        blit((BITMAP *)gfxdata[iType].dat, temp, (iWidth*iFrame), 0, 0, 0, iWidth, iHeight);
    }

	if (iAlpha > -1)
	{
		set_trans_blender(0,0,0,iAlpha);

		if (iType != OBJECT_BOOM01 && iType != OBJECT_BOOM02 && iType != OBJECT_BOOM03)
        {
			draw_trans_sprite(bmp_screen, temp, dx-(iWidth/2), dy-(iHeight/2));

			if (iType == EXPLOSION_ROCKET || iType == EXPLOSION_ROCKET_SMALL)
            {
                fblend_add((BITMAP *)gfxdata[OBJECT_BOOM03].dat, bmp_screen,  dx-(64), dy-(64), (iAlpha/2));
            }

        }
		else
			fblend_add(temp, bmp_screen,  dx-(iWidth/2), dy-(iHeight/2), iAlpha);
	}
	else
		draw_sprite(bmp_screen, temp, dx-(iWidth/2), dy-(iHeight/2));

	set_trans_blender(0,0,0,128);

    destroy_bitmap(temp);

}


// think
void cParticle::think()
{/*
    if (iType == PARTYPE_SMOKE)
    {


        return;
    }*/

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

	//	y-= 16;
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
            //logbook("Particle died");
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
        iType == EXPLOSION_GAS ||
        iType == OBJECT_WORMEAT)
    {
        TIMER_frame--;



        if (TIMER_frame < 0)
        {
            if (iFrame <= 3)
                TIMER_frame=28;
            else
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




int PARTICLE_NEW()
{
    for (int i=0; i < MAX_PARTICLES; i++)
    {
        if (particle[i].bAlive==false)
            return i;
    }

    return -1;
}

void PARTICLE_CREATE(long x, long y, int iType, int iHouse, int iFrame)
{
    int iNewId = PARTICLE_NEW();

    if (iNewId < 0)
        return;

    particle[iNewId].init();

    particle[iNewId].x = x;
    particle[iNewId].y = y;

    particle[iNewId].iType = iType;

     // depending on type, set TIMER_dead & FRAME & FRAME TIME

    particle[iNewId].iFrame = 0;
    particle[iNewId].TIMER_dead = 0;
    particle[iNewId].TIMER_frame = 10;

    particle[iNewId].iHousePal = iHouse;

	particle[iNewId].iAlpha = -1;

    particle[iNewId].bAlive = true;

	if (iType == PARTYPE_REPAIRBLINK)
		particle[iNewId].iAlpha = 255;

	if (iType == MOVE_INDICATOR || iType == ATTACK_INDICATOR)
		particle[iNewId].iAlpha = 128;

    if (iType == EXPLOSION_TRIKE)
    {
        particle[iNewId].iAlpha=255;
        particle[iNewId].iWidth=48;
        particle[iNewId].iHeight=48;
		PARTICLE_CREATE(x, y, OBJECT_BOOM03, -1, 0);
    }


    if (iType == OBJECT_SMOKE)
    {
        particle[iNewId].iAlpha=0;
        particle[iNewId].iWidth=32;
        particle[iNewId].iHeight=48;
        particle[iNewId].TIMER_dead=900;
        PARTICLE_CREATE(x+16, y+42, OBJECT_SMOKE_SHADOW, -1, -1);
    }

    if (iType == OBJECT_SMOKE_SHADOW)
    {
        particle[iNewId].iAlpha=0;
        particle[iNewId].iWidth=36;
        particle[iNewId].iHeight=38;
        particle[iNewId].TIMER_dead=1000;
    }

    if (iType == TRACK_DIA || iType == TRACK_HOR || iType == TRACK_VER || iType == TRACK_DIA2)
    {
        particle[iNewId].iAlpha=128;
        particle[iNewId].TIMER_dead=2000;
        particle[iNewId].layer=1; // other layer
    }

    if (iType == BULLET_PUF)
    {
        particle[iNewId].iHeight = 18;
        particle[iNewId].iWidth  = 18;
        particle[iNewId].iAlpha  = -1;
    }

    // bullets of tanks explode
    if (iType == EXPLOSION_BULLET)
    {

    }

    // trike exploding
    if (iType == EXPLOSION_FIRE)
    {
        particle[iNewId].iAlpha=255;
        particle[iNewId].TIMER_dead=750+rnd(500);
        particle[iNewId].iAlpha = rnd(255);
    }

    // tanks exploding
    if (iType == EXPLOSION_TANK_ONE ||
        iType == EXPLOSION_TANK_TWO ||
        iType == EXPLOSION_STRUCTURE01 ||
        iType == EXPLOSION_STRUCTURE02 ||
        iType == EXPLOSION_GAS ||
        iType == OBJECT_WORMEAT)
    {

		if (iType != EXPLOSION_STRUCTURE01 && iType != EXPLOSION_STRUCTURE02)
			PARTICLE_CREATE(x, y, OBJECT_BOOM02, -1, 0);

		particle[iNewId].iAlpha=255;
        particle[iNewId].iWidth=48;
        particle[iNewId].iHeight=48;
    }

    // worm trail
    if (iType == OBJECT_WORMTRAIL)
    {
        particle[iNewId].iAlpha=96;
        particle[iNewId].iWidth=48;
        particle[iNewId].iHeight=48;

		particle[iNewId].layer=1; // other layer
    }

    if (iType == OBJECT_DEADINF01 ||
        iType == OBJECT_DEADINF02)
    {
        particle[iNewId].TIMER_dead = 500 + rnd(500);
        particle[iNewId].iAlpha=255;
        particle[iNewId].layer=1; // other layer
    }

    if (iType == OBJECT_TANKSHOOT || iType == OBJECT_SIEGESHOOT)
    {
        particle[iNewId].iFrame=iFrame;
        particle[iNewId].TIMER_dead = 50;
        particle[iNewId].iAlpha=128;
        particle[iNewId].iWidth=64;
        particle[iNewId].iHeight=64;
    }

    if (iType == EXPLOSION_SQUISH01 ||
        iType == EXPLOSION_SQUISH02 ||
        iType == EXPLOSION_SQUISH03 ||
        iType == EXPLOSION_ORNI)
    {
        particle[iNewId].iFrame=0;
        particle[iNewId].TIMER_frame = 50;
        particle[iNewId].iWidth=32;
        particle[iNewId].iAlpha=255;
        particle[iNewId].iHeight=32;
        particle[iNewId].layer=1;
    }


    if (iType == OBJECT_SIEGEDIE)
    {
        particle[iNewId].iAlpha=255;
        particle[iNewId].TIMER_frame=500 + rnd(300);
        PARTICLE_CREATE(x, y-18, EXPLOSION_FIRE, -1, -1);
        PARTICLE_CREATE(x, y-18, OBJECT_SMOKE, -1, -1);

		PARTICLE_CREATE(x, y, OBJECT_BOOM02, -1, 0);
    }

    if (iType == OBJECT_CARRYPUFF)
    {
        particle[iNewId].iFrame=0;
        particle[iNewId].TIMER_frame=50+rnd(50);
        particle[iNewId].iWidth=96;
        particle[iNewId].iHeight=96;
        particle[iNewId].layer=1;
        particle[iNewId].iAlpha=96+rnd(64);
    }

    if (iType == EXPLOSION_ROCKET || iType == EXPLOSION_ROCKET_SMALL)
        particle[iNewId].iAlpha = 255;

	if (iType == OBJECT_BOOM01)
	{
		particle[iNewId].iAlpha=240;
		particle[iNewId].iWidth=512;
		particle[iNewId].iHeight=512;
	}

	if (iType == OBJECT_BOOM02)
	{
		particle[iNewId].iAlpha=230;
		particle[iNewId].iWidth=256;
		particle[iNewId].iHeight=256;
	}

	if (iType == OBJECT_BOOM03)
	{
		particle[iNewId].iAlpha=220;
		particle[iNewId].iWidth=128;
		particle[iNewId].iHeight=128;
	}

 //   logbook("Particle created");
}


