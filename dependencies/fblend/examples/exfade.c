#include <allegro.h>
#include "fblend.h"

struct FADE_REGION {
	int x, y;
	BITMAP *src;
	int fact;
	int dir;
	int color;
} fade[4];

volatile int chrono = 0;

void the_timer(void) {
	chrono++;
} END_OF_FUNCTION(the_timer);


int main() {

	BITMAP *backbuffer = NULL, *mysha;
	int count = 0;
	int old_time = 0, old_time2 = 0;
	int depth;
	int i;
	int use_alleg = 0;
	
	allegro_init();
	
	install_keyboard();
	install_timer();
	
	set_config_file("examples.cfg");
	depth = get_config_int("examples", "depth", 16);

	set_color_depth(depth);	
	if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
		set_color_depth(16);
			if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
			set_color_depth(15);
			if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
				set_color_depth(32);
				if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)) {
					allegro_message("Unable to set 640x480 screen mode!\n");
					return -1;
				}
			}
		}
	}
	
	mysha = load_bitmap("mysha.pcx", NULL);
	
	if (!mysha) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Unable to load mysha.pcx. Copy it from the allegro examples directory.\n");
		return -2;
	}
	
	backbuffer = create_bitmap(SCREEN_W, SCREEN_H);
	
	if (!backbuffer) {
		destroy_bitmap(mysha);
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Not enough memory to create backbuffer.\n");
		return -3;
	}
	
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);

	stretch_blit(mysha, backbuffer, 0, 0, mysha->w, mysha->h, 0, 0, SCREEN_W, SCREEN_H);
	
	for (i = 0; i < 4; i++) {
		fade[i].x = (SCREEN_W/2) * (i & 1);
		fade[i].y = (SCREEN_H/2) * (i & 2) >> 1;
		fade[i].fact = rand() & 255;
		fade[i].dir = (rand() & 1) ? 1 : -1;
		fade[i].src = create_sub_bitmap(backbuffer, fade[i].x, fade[i].y, SCREEN_W/2, SCREEN_H/2);
		fade[i].color = makecol((i & 1) * 255, ((i & 2) >> 1) * 255, 0);
	}
		

	install_int(the_timer, 1);
	old_time = chrono;

	while (!key[KEY_ESC]) {
		if (use_alleg) {
			stretch_blit(mysha, backbuffer, 0, 0, mysha->w, mysha->h, 0, 0, SCREEN_W, SCREEN_H);
		}
		textprintf(backbuffer, font, 0, 0, makecol(255, 255, 255), "%s  %.2f fps (%.3f avg)", use_alleg ? "Using Allegro" : "Using FBlend", (chrono - old_time2) == 0 ? 1000.0 : 1000 / ((double)chrono - old_time2), count * 1000.0 / chrono);

		if (use_alleg) {
			drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
			for (i = 0; i < 4; i++) {
				set_trans_blender(0, 0, 0, 255 - fade[i].fact);
				rectfill(fade[i].src, 0, 0, fade[i].src->w - 1, fade[i].src->h - 1, fade[i].color);
			}
			drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
		}
		else {
			for (i = 0; i < 4; i++) {
				fblend_fade_to_color(fade[i].src, screen, fade[i].x, fade[i].y, fade[i].color, fade[i].fact);
			}
		}

		if (key[KEY_SPACE]) {
			use_alleg = !use_alleg;
			key[KEY_SPACE] = 0;
			chrono = 0;
			count = 0;
			old_time = 0;
			old_time2 = 0;
		}
		count++;

		old_time2 = chrono;

		
		/* Draw the buffer */
		if (use_alleg) {
			blit(backbuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		}
		
		for (i = 0; i < 4; i++) {
			fade[i].fact += fade[i].dir;
			if (fade[i].fact >= 255 || fade[i].fact <= 0)
				fade[i].dir = -fade[i].dir;
			fade[i].fact = MID(0, fade[i].fact, 255);
		}
	}
	for (i = 0; i < 4; i++) {
		destroy_bitmap(fade[i].src);
	}
	
	destroy_bitmap(backbuffer);
	destroy_bitmap(mysha);
	
	return 0;
}
END_OF_MAIN();
