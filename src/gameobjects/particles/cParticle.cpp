/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks
*/

#include "cParticle.h"

#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "map/cMapCamera.h"
#include "player/cPlayer.h"

#include <allegro.h>
#include <SDL2/SDL.h>

cParticle::cParticle()
{
    dimensions = nullptr;
    init();
}

cParticle::~cParticle()
{
    bmp = nullptr;
    if (dimensions) delete dimensions;
}


// init
void cParticle::init()
{
    boundUnitID = -1;
    boundParticleID = -1;
    oldParticle = true;
    bAlive = false;       // alive (if yes, it is in use, if not it can be used)
    iAlpha = -1;            // alpha number

    x = 0;
    y = 0;              // x and y position to draw (absolute numbers)
    frameIndex = 0;
    iType = 0;          // type

    bmp = nullptr;
    drawXBmpOffset = drawYBmpOffset = 0;

    iHousePal = -1;     // when specified, use this palette for drawing (and its an 8 bit picture then!)

    // TIMERS
    TIMER_frame = 0;    // frame animation timers (when < 0, next frame, etc)
    // when TIMER_dead < 0, the last frame lets this thing die

    TIMER_dead = 0;     // when > -1, this timer will determine when the thing dies

    if (dimensions) delete dimensions;
    dimensions = nullptr;
}

// valid
bool cParticle::isValid()
{
    return bAlive;
}

int cParticle::draw_x()
{
    return mapCamera->getWindowXPositionWithOffset(x, drawXBmpOffset);
}

int cParticle::draw_y()
{
    return mapCamera->getWindowYPositionWithOffset(y, drawYBmpOffset);
}

/**
 * Poor man solution to frequently update the dimensions, better would be using events?
 * (onMove, onViewportMove, onViewportZoom?)
 */
void cParticle::think_position()
{
    if (boundUnitID > -1) {
        cUnit &pUnit = unit[boundUnitID];
        if (!pUnit.isValid()) {
            bindToUnit(-1);
        }
    }

    // keep updating dimensions
    dimensions->move(draw_x(), draw_y());
    if (mapCamera) {
        dimensions->resize(mapCamera->factorZoomLevel(getFrameWidth()),
                           mapCamera->factorZoomLevel(getFrameHeight()));
    }
}

// draw
void cParticle::draw()
{
    s_ParticleInfo particleInfo = getParticleInfo();
    int frameWidth = getFrameWidth();
    int frameHeight = getFrameHeight();

    // valid in boundaries
    SDL_Surface *temp = create_bitmap(frameWidth, frameHeight);

    // transparency
    clear_to_color(temp, makecol(255, 0, 255));

    // now blit it
    if (iHousePal > -1) {
        cPlayer &player = players[iHousePal];
        select_palette(player.pal);
    }

    if (bmp) {
        // new behavior
        renderDrawer->blit(bmp, temp, (frameWidth * frameIndex), 0, frameWidth, frameHeight, 0, 0);
    }
    else {
        // old behavior
        renderDrawer->blitFromGfxData(iType, temp, (frameWidth * frameIndex), 0, frameWidth, frameHeight, 0, 0);
    }

    // create proper sized bitmap
    int bmp_width = mapCamera->factorZoomLevel(frameWidth);
    int bmp_height = mapCamera->factorZoomLevel(frameHeight);

    // create bmp that is the stretched version of temp
    SDL_Surface *stretched = create_bitmap(bmp_width + 1, bmp_height + 1);
    clear_to_color(stretched, makecol(255, 0, 255)); // mask color
    renderDrawer->maskedStretchBlit(temp, stretched, 0, 0, frameWidth, frameHeight, 0, 0, bmp_width, bmp_height);

    // temp is no longer needed
    SDL_FreeSurface(temp);

    int drawX = draw_x();
    int drawY = draw_y();

    if (isUsingAlphaChannel()) {
        if (particleInfo.usesAdditiveBlending) {
            set_add_blender(0, 0, 0, iAlpha);
        }
        else {
            set_trans_blender(0, 0, 0, iAlpha);
        }
        draw_trans_sprite(bmp_screen, stretched, drawX, drawY);
    }
    else {
        renderDrawer->drawSprite(bmp_screen, stretched, drawX, drawY);
    }

    set_trans_blender(0, 0, 0, 128);

    SDL_FreeSurface(stretched);
}

s_ParticleInfo &cParticle::getParticleInfo() const
{
    s_ParticleInfo &particleInfo = sParticleInfo[iType];
    return particleInfo;
}

bool cParticle::isUsingAlphaChannel() const
{
    return iAlpha > -1 && iAlpha < 255;
}

bool cParticle::isWithinViewport(const cRectangle &viewport)
{
    return dimensions->isOverlapping(viewport);
}


// called every 5 ms
void cParticle::thinkFast()
{
    think_position();

    if (!oldParticle) {
        // new thinking
        think_new();
        return;
    }

    // old way
    if (iType == D2TM_PARTICLE_OBJECT_BOOM01 ||
            iType == D2TM_PARTICLE_OBJECT_BOOM02 ||
            iType == D2TM_PARTICLE_OBJECT_BOOM03) {
        TIMER_frame++;

        if (TIMER_frame > 0) {
            TIMER_frame = 0;

            iAlpha -= 2;

            if (iAlpha < 1) {
                die();
                return;
            }
        }
    }

    if (iType == D2TM_PARTICLE_CARRYPUFF) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            frameIndex++;
            TIMER_frame = 100 + rnd(100);

            if (frameIndex > 5) {
                die();
                return;
            }

            iAlpha -= 16;
        }
    }

    // move
    if (iType == D2TM_PARTICLE_MOVE ||
            iType == D2TM_PARTICLE_ATTACK) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            TIMER_frame = 15;
            frameIndex++;
        }

        if (frameIndex > 9) {
            iAlpha -= 35;
            frameIndex = 9;

            if (iAlpha < 10) {
                die();
                return;
            }
        }

        return;
    }

    if (iType == D2TM_PARTICLE_SIEGEDIE) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            frameIndex++;

            if (frameIndex > 2) {
                frameIndex = 2;
                iAlpha -= 10;

                if (iAlpha < 10) {
                    die();
                    return;
                }

                TIMER_frame = 10;
            }
            else {
                TIMER_frame = 250;
            }
        }
    }

    if (iType == D2TM_PARTICLE_EXPLOSION_TRIKE) {
        TIMER_frame--;
        if (TIMER_frame < 0) {
            iAlpha -= 20;
            TIMER_frame = 5;

            if (frameIndex < 1 && iAlpha < 220)
                frameIndex++;

            if (frameIndex >= 1 && iAlpha < 10) {
                die();
                return;
            }
        }
    }

    if (iType == D2TM_PARTICLE_SMOKE ||
            iType == D2TM_PARTICLE_SMOKE_SHADOW) {
        TIMER_frame--;
        TIMER_dead--;

        // particle is not yet going to die, and still not opaque, make it fade-in
        if (TIMER_dead > 0 && iAlpha < 255) {
            if (iType == D2TM_PARTICLE_SMOKE) {
                iAlpha++;
            }
            else if (iType == D2TM_PARTICLE_SMOKE_SHADOW) {
                // shadow remains 128 alpha (never fully opaque)
                if (iAlpha < 128) {
                    iAlpha++;
                }
            }
        }

        // animation timer
        if (TIMER_frame < 0) {
            TIMER_frame = 50;
            frameIndex++;

            if (frameIndex > 2) {
                frameIndex = 0;
            }

            // make it fade-out at the speed we animate
            if (TIMER_dead < 0) {
                TIMER_dead = -1;
                if (iType == D2TM_PARTICLE_SMOKE_SHADOW) {
                    iAlpha -= 10;
                }
                else {
                    iAlpha -= 14;
                }

                if (iAlpha < 10) {
                    die();
                    return;
                }
            }
        }
        return;
    }

    if (iType == D2TM_PARTICLE_TRACK_DIA ||
            iType == D2TM_PARTICLE_TRACK_HOR ||
            iType == D2TM_PARTICLE_TRACK_VER ||
            iType == D2TM_PARTICLE_TRACK_DIA2) {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0) {
            TIMER_frame = 10;
            if (rnd(100) < 10 && iAlpha > 192)
                iAlpha--;

            if (TIMER_dead > 0) {
                if (iAlpha < 255)
                    iAlpha += 2;

            }
            else {
                // its dying
                if (iAlpha > 10) {
                    iAlpha -= 10;
                }
                else {
                    die();
                }

            }
        }
    }

    if (iType == D2TM_PARTICLE_BULLET_PUF) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            TIMER_frame = 15;

            frameIndex++;

            if (frameIndex > 7) {
                die();
            }
        }

    }

    if (iType == D2TM_PARTICLE_DEADINF01 ||
            iType == D2TM_PARTICLE_DEADINF02) {
        TIMER_frame--;
        TIMER_dead--;

        if (TIMER_frame < 0) {
            if (TIMER_dead < 0) {
                TIMER_dead = -1;
                iAlpha -= 5;

                if (iAlpha < 5) {
                    die();
                }

            }

            TIMER_frame = 10;
        }

    }

    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            TIMER_frame = 10;

            frameIndex++;

            if (frameIndex > 1) {
                frameIndex = 1;

                iAlpha -= 25;

                if (iAlpha < 25) {
                    die();
                }
            }
        }
    }

    // normal rocket explosion (of launchers)
    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            TIMER_frame = 10;

            frameIndex++;

            if (frameIndex > 3) {
                frameIndex = 4;

                iAlpha -= 35;

                if (iAlpha < 25) {
                    die();
                }
            }
        }
    }

    if (iType == D2TM_PARTICLE_WORMTRAIL) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            if (frameIndex <= 3)
                TIMER_frame = 100;
            else
                TIMER_frame = 20;

            frameIndex++;

            if (frameIndex > 3) {
                frameIndex = 4;

                iAlpha -= 5;

                if (iAlpha < 5) {
                    die();
                }
            }
        }
    }
    // tank explosion(s)
    if (iType == D2TM_PARTICLE_EXPLOSION_TANK_ONE ||
            iType == D2TM_PARTICLE_EXPLOSION_TANK_TWO ||
            iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE01 ||
            iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE02 ||
            iType == D2TM_PARTICLE_EXPLOSION_GAS) {
        TIMER_frame--;

        if (TIMER_frame < 0) {

            // delay for next frame(s)
            if (frameIndex <= 3) {
                TIMER_frame = 28;
            }
            else {
                TIMER_frame = 10;
            }

            frameIndex++;

            if (frameIndex > 3) {
                frameIndex = 4;

                iAlpha -= 15;

                if (iAlpha < 25) {
                    die();
                }
            }
        }
    }

    if (iType == D2TM_PARTICLE_WORMEAT) {
        TIMER_frame--;

        if (TIMER_frame < 0) {
            frameIndex++;

            // delay for next frame(s)
            if (frameIndex <= 3) {
                // begins slow, and speeds up after each frame
//                TIMER_frame = (4 - frameIndex) * 32;
                TIMER_frame = 80;
            }
            else {
                // frame will stick at 4 (eaten)
                frameIndex = 4;

                // fade out
                iAlpha -= 15;

                // until particle dies
                if (iAlpha < 25) {
                    die();
                }
            }
        }
    }

    // bullets of tanks explode
    if (iType == D2TM_PARTICLE_EXPLOSION_BULLET) {
        TIMER_frame--;
        if (TIMER_frame < 0) {
            TIMER_frame = 10;
            frameIndex++;
            if (frameIndex > 1) {
                die();
            }

        }

    }

    if (iType == D2TM_PARTICLE_SQUISH01 ||
            iType == D2TM_PARTICLE_SQUISH02 ||
            iType == D2TM_PARTICLE_SQUISH03 ||
            iType == D2TM_PARTICLE_EXPLOSION_ORNI) {
        TIMER_frame--;
        if (TIMER_frame < 0) {
            if (iAlpha > 5) {
                iAlpha -= 5;
            }
            else {
                die();
            }

            TIMER_frame = 50;
        }
    }

    if (iType == D2TM_PARTICLE_TANKSHOOT || iType == D2TM_PARTICLE_SIEGESHOOT) {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0) {
            TIMER_frame = 2;
            if (TIMER_dead > 0) {
                if (iAlpha + 5 < 255)
                    iAlpha += 5;
            }
            else {
                TIMER_dead = -1;
                iAlpha -= 15;

                if (iAlpha < 10) {
                    die();
                }
            }
        }
    }


    if (iType == D2TM_PARTICLE_EXPLOSION_FIRE) {
        TIMER_frame--;
        TIMER_dead--;
        if (TIMER_frame < 0) {
            TIMER_frame = 20;

            frameIndex++;


            if (frameIndex > 2)
                frameIndex = 0;

            if (TIMER_dead < 0) {
                iAlpha -= 20;
                if (iAlpha < 10) {
                    die();
                }
            }
            else {
                if (iAlpha + 15 < 255)
                    iAlpha += 15;
                else
                    iAlpha = 255;
            }

        }
    }
}

/**
 * Thoughts 23/12/2021<br/>
 *
 * This should really move to a kind of 'repository' or 'container' thing. Now basically the container is globally
 * scoped. And this factory method finds a new 'free slot' for a particle to be revived. It is a fixed array, no
 * fancy thing. Better would be to have a cParticleContainer/Repository and there add/remove particles. We could
 * query the particles based on viewport coordinates, and do things with them from there (render for instance?).
 * @param x
 * @param y
 * @param iType
 * @param iHouse
 * @param iFrame
 * @return
 */
int cParticle::create(long x, long y, int iType, int iHouse, int iFrame)
{
    return create(x, y, iType, iHouse, iFrame, -1);
}

int cParticle::create(long x, long y, int iType, int iHouse, int iFrame, int iUnitID)
{
    int iNewId = findNewSlot();

    if (iNewId < 0) {
        return -1;
    }

    cParticle &pParticle = particle[iNewId];
    if (iType > -1 && iType < MAX_PARTICLE_TYPES) {
        s_ParticleInfo &sParticle = sParticleInfo[iType];
        pParticle.init(sParticle);
    }
    else {
        pParticle.init();
    }

    pParticle.x = x;
    pParticle.y = y;
    pParticle.boundUnitID = iUnitID;

    pParticle.iType = iType;

    // depending on type, set TIMER_dead & FRAME & FRAME TIME
    pParticle.frameIndex = 0;
    pParticle.TIMER_dead = 0;
    pParticle.TIMER_frame = 10;

    pParticle.iHousePal = iHouse;

    pParticle.bAlive = true;

    pParticle.drawXBmpOffset = (pParticle.getFrameWidth() / 2) * -1;
    pParticle.drawYBmpOffset = (pParticle.getFrameHeight() / 2) * -1;
    pParticle.recreateDimensions();

    if (iType == D2TM_PARTICLE_EXPLOSION_TRIKE) {
        // TODO: Spawn additional particle property
        create(x, y, D2TM_PARTICLE_OBJECT_BOOM03, -1, 0, iUnitID);
    }

    if (iType == D2TM_PARTICLE_SMOKE) {
        pParticle.TIMER_dead = 1500;

        // since x, y is 'center' of particle, we have to compensate. Because smoke "starts" at the bottom (ie, its
        // offset is not in center). So we have to subtract half of the sprite's height
        pParticle.y += pParticle.drawYBmpOffset;
    }

    if (iType == D2TM_PARTICLE_SMOKE_WITH_SHADOW) {
        pParticle.TIMER_dead = 1500;
        int shadowParticleId = create(x + 16, y + 38, D2TM_PARTICLE_SMOKE_SHADOW, -1, -1, iUnitID);

        // since x, y is 'center' of particle, we have to compensate. Because smoke "starts" at the bottom (ie, its
        // offset is not in center). So we have to subtract half of the sprite's height
        pParticle.y += pParticle.drawYBmpOffset;
        pParticle.boundParticleID = shadowParticleId;
        pParticle.iType = D2TM_PARTICLE_SMOKE; // ugly hack for now, but this reduces the need to duplicate if-statements later
    }

    if (iType == D2TM_PARTICLE_SMOKE_SHADOW) {
        pParticle.TIMER_dead = 1500;

        // since x, y is 'center' of particle, we have to compensate. Because smoke "starts" at the bottom (ie, its
        // offset is not in center). So we have to subtract half of the sprite's height
        pParticle.y += pParticle.drawYBmpOffset;
    }

    if (iType == D2TM_PARTICLE_TRACK_DIA || iType == D2TM_PARTICLE_TRACK_HOR || iType == D2TM_PARTICLE_TRACK_VER ||
            iType == D2TM_PARTICLE_TRACK_DIA2) {
        pParticle.TIMER_dead = 2000;
    }


    // trike exploding
    if (iType == D2TM_PARTICLE_EXPLOSION_FIRE) {
        pParticle.iAlpha = 255;
        pParticle.TIMER_dead = 750 + rnd(500);
        pParticle.iAlpha = rnd(255);
    }

    // tanks exploding
    if (iType == D2TM_PARTICLE_WORMEAT) {
        pParticle.TIMER_frame = 80; // 2,5 * 32 (a tad slower than on 3 frames)
    }

    // tanks exploding
    if (iType == D2TM_PARTICLE_EXPLOSION_TANK_ONE ||
            iType == D2TM_PARTICLE_EXPLOSION_TANK_TWO ||
            iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE01 ||
            iType == D2TM_PARTICLE_EXPLOSION_STRUCTURE02 ||
            iType == D2TM_PARTICLE_EXPLOSION_GAS) {

        if (iType != D2TM_PARTICLE_EXPLOSION_STRUCTURE01 && iType != D2TM_PARTICLE_EXPLOSION_STRUCTURE02) {
            create(x, y, D2TM_PARTICLE_OBJECT_BOOM02, -1, 0, iUnitID);
        }

    }

    if (iType == D2TM_PARTICLE_DEADINF01 ||
            iType == D2TM_PARTICLE_DEADINF02) {
        pParticle.TIMER_dead = 500 + rnd(500);
        pParticle.iAlpha = 255;
    }

    if (iType == D2TM_PARTICLE_TANKSHOOT || iType == D2TM_PARTICLE_SIEGESHOOT) {
        pParticle.frameIndex = iFrame;
        pParticle.TIMER_dead = 50;
    }

    if (iType == D2TM_PARTICLE_SQUISH01 ||
            iType == D2TM_PARTICLE_SQUISH02 ||
            iType == D2TM_PARTICLE_SQUISH03 ||
            iType == D2TM_PARTICLE_EXPLOSION_ORNI) {
        pParticle.frameIndex = 0;
        pParticle.TIMER_frame = 50;
    }

    if (iType == D2TM_PARTICLE_SIEGEDIE) {
        pParticle.iAlpha = 255;
        pParticle.TIMER_frame = 500 + rnd(300);

        create(x, y - 18, D2TM_PARTICLE_EXPLOSION_FIRE, -1, -1, iUnitID);
        create(x, y - 18, D2TM_PARTICLE_SMOKE, -1, -1, iUnitID);
        create(x, y, D2TM_PARTICLE_OBJECT_BOOM02, -1, 0, iUnitID);
    }

    if (iType == D2TM_PARTICLE_CARRYPUFF) {
        pParticle.frameIndex = 0;
        pParticle.TIMER_frame = 50 + rnd(50);
        pParticle.iAlpha = 96 + rnd(64);
    }

    if (iType == D2TM_PARTICLE_EXPLOSION_ROCKET || iType == D2TM_PARTICLE_EXPLOSION_ROCKET_SMALL) {
        pParticle.iAlpha = 255;
        // also create bloom
        create(x, y, D2TM_PARTICLE_OBJECT_BOOM03, iHouse, 0, iUnitID);
    }

    return iNewId;
}

int cParticle::findNewSlot()
{
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particle[i].isValid())
            return i;
    }

    return -1;
}

void cParticle::init(const s_ParticleInfo &particleInfo)
{
    init();

    if (particleInfo.bmpIndex > -1) {
        bmp = game.getDataRepository()->getBitmapAt(particleInfo.bmpIndex);
    }

    if (particleInfo.startAlpha > -1 && particleInfo.startAlpha < 256) {
        iAlpha = particleInfo.startAlpha;
    }
    else {
        iAlpha = 255;
    }

    oldParticle = particleInfo.oldParticle;
}

int cParticle::getFrameWidth()
{
    return getParticleInfo().frameWidth;
}

int cParticle::getFrameHeight()
{
    return getParticleInfo().frameHeight;
}

int cParticle::getLayer()
{
    return getParticleInfo().layer;
}

void cParticle::recreateDimensions()
{
    if (dimensions) {
        delete dimensions;
    }
    dimensions = new cRectangle(draw_x(), draw_y(), getFrameWidth(), getFrameHeight());
}

void cParticle::think_new()
{

}

void cParticle::bindToUnit(int unitID)
{
    if (boundUnitID > -1) {
        cUnit &pUnit = unit[boundUnitID];
        if (pUnit.isValid()) {
            pUnit.setBoundParticleId(-1);
        }
    }
    boundUnitID = unitID;
}

void cParticle::addPosX(float d)
{
    this->x += d;
    if (boundParticleID > -1) {
        cParticle &otherParticle = particle[boundParticleID];
        if (otherParticle.isValid()) {
            otherParticle.addPosX(d);
        }
        else {
            boundParticleID = -1;
        }
    }
}

void cParticle::addPosY(float d)
{
    this->y += d;
    if (boundParticleID > -1) {
        cParticle &otherParticle = particle[boundParticleID];
        if (otherParticle.isValid()) {
            otherParticle.addPosY(d);
        }
        else {
            boundParticleID = -1;
        }
    }
}

void cParticle::die()
{
    bindToUnit(-1);
    bAlive = false;
    if (boundParticleID > -1) {
        cParticle &pParticle = particle[boundParticleID];
        if (pParticle.isValid()) {
            pParticle.die();
        }
    }
    boundParticleID = -1;
}
