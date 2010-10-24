/* 
 * File:   cMouseDrawer.cpp
 * Author: el.anormal
 * 
 * Created Oct 23, 2010
 */

#include "../d2tmh.h"

cMouseDrawer::cMouseDrawer() {
}

cMouseDrawer::cMouseDrawer(const cMouseDrawer& orig) {
}

cMouseDrawer::~cMouseDrawer() {
}

void cMouseDrawer::draw() {
    if (mouse_tile == MOUSE_DOWN){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y-16);
    }
    else if (mouse_tile == MOUSE_RIGHT){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y);
    }
    else if (mouse_tile == MOUSE_MOVE || mouse_tile == MOUSE_RALLY){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    }
    else if (mouse_tile == MOUSE_ATTACK){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    }
    else if (mouse_tile == MOUSE_REPAIR){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    }
    else if (mouse_tile == MOUSE_PICK){
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x-16, mouse_y-16);
    }
    else{
        draw_sprite(bmp_screen, (BITMAP *)gfxdata[mouse_tile].dat, mouse_x, mouse_y);
    }
}


