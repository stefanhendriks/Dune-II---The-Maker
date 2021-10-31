#include "d2tmh.h"


cFlag::cFlag(cPlayer *player, cPoint &absCoords, int frames, int animationDelay) :
        player(player), absCoords(absCoords),
        frames(frames), animationDelay(animationDelay) {
    TIMER_animate = 0;
    frame = 0;
    big = true;
}

void cFlag::draw() {
    BITMAP *flagBitmap = big? player->getFlagBitmap() : player->getFlagSmallBitmap();
    if (!flagBitmap) return;

    int drawX = mapCamera->getWindowXPosition(absCoords.x);
    int drawY = mapCamera->getWindowYPosition(absCoords.y);

    if ((drawX >= 0 && drawX < game.screen_x) &&
        (drawY >= 0 && drawY < game.screen_y)) { // within screen
        // draw it

        int pixelWidth = flagBitmap->w;
        int pixelHeight = flagBitmap->h / frames; // frames in a flag

        // The Y coordinates determine what frame is being drawn.
        // So multiply the height of the flag size times frame
        int iSourceY = pixelHeight * frame;

        int scaledWidth = mapCamera->factorZoomLevel(pixelWidth);
        int scaledHeight = mapCamera->factorZoomLevel(pixelHeight);

        allegroDrawer->maskedStretchBlit(flagBitmap, bmp_screen, 0, iSourceY, pixelWidth, pixelHeight,
                                         drawX, drawY, scaledWidth, scaledHeight);
    }
}

void cFlag::thinkFast() {
    TIMER_animate++;
    if (TIMER_animate > animationDelay) {
        frame++;
        if (frame > (frames-1)) frame = 0;
        TIMER_animate = 0;
    }
}

cFlag *cFlag::createBigFlag(cPlayer * player, cPoint & position) {
    cPoint correctedPoint = cPoint(
            position.x,
            position.y
    );
    correctedPoint.x -= 16; // width of flag
    cFlag *pFlag = new cFlag(player, correctedPoint, 12, 24);
    pFlag->setBig(true);
    return pFlag;
}

cFlag *cFlag::createSmallFlag(cPlayer * player, cPoint & position) {
    cPoint correctedPoint = cPoint(
            position.x,
            position.y
    );
    correctedPoint.x -= 11; // width of flag
    cFlag *pFlag = new cFlag(player, correctedPoint, 12, 24);
    pFlag->setBig(false);
    return pFlag;
}
