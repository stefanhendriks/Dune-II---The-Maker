#include "fblend.h"

//#define COMPARE_WITH_ALLEGRO


typedef struct PARTICLE {
	int x, y;
	int vx, vy;
	int a, a_dir;
	int color;
} PARTICLE;

#define NUM_PARTICLES 200

volatile int chrono = 0;

void the_timer(void) {
	chrono++;
} END_OF_FUNCTION(the_timer);

int main() {
	
	BITMAP *mysha = NULL, *buffer = NULL;
	PARTICLE *particle[NUM_PARTICLES];
	int old_time = 0, old_time2 = 0;
	int i;
	int count = 0;
	int use_alleg = FALSE;
	int depth = 16;
	
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
	
	TRACE("Got here!\n");
	
	for (i = 0; i < NUM_PARTICLES; i++)
		particle[i] = NULL;
		
	TRACE("Still here!\n");
	
	for (i = 0; i < NUM_PARTICLES; i++) {
		int r, g, b;
		
		TRACE("Going for particle: %i\n", i);
	
		particle[i] = malloc(sizeof(PARTICLE));		
		if (!particle[i]) {
			TRACE("Out of memory while creating particle!\n");
			goto Error;
		}
		
		TRACE("Clearing particle\n");
		
		memset(particle[i], 0, sizeof(PARTICLE));
		
		TRACE("Setting up coordinates\n");
			
		particle[i]->x = rand() % SCREEN_W;
		particle[i]->y = rand() % SCREEN_H;
		particle[i]->vx = rand() % 10 - 5;
		particle[i]->vy = rand() % 10 - 5;
		particle[i]->a = rand() & 255;
		particle[i]->a_dir = 1;
		
		TRACE("Setting up colors\n");
		hsv_to_rgb(rand() % 360, 1, 1, &r, &g, &b);
		particle[i]->color = makecol(r, g, b);
	}
	
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
		
	buffer = create_bitmap(SCREEN_W, SCREEN_H);
	
	if (!buffer) {
		TRACE("Out of memory while creating back buffer!\n");
		goto Error;
	}
	
	clear(buffer);

	TRACE("Starting...\n");
	
	install_int(the_timer, 1);
	old_time = chrono;
	
	text_mode(0);
	
	do {
		int chrono2;
	
		/* Tile mysha over the screen */
		#ifndef COMPARE_WITH_ALLEGRO
		blit(mysha, buffer, 0, 0, 0, 0, mysha->w, mysha->h);
		blit(mysha, buffer, 0, 0, 320, 0, mysha->w, mysha->h);
		blit(mysha, buffer, 0, 0, 0, 200, mysha->w, mysha->h);
		blit(mysha, buffer, 0, 0, 320, 200, mysha->w, mysha->h);
		blit(mysha, buffer, 0, 0, 0, 400, mysha->w, mysha->h);
		blit(mysha, buffer, 0, 0, 320, 400, mysha->w, mysha->h);
		#endif
		
		if (use_alleg) {		
			drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
			for (i = 0; i < NUM_PARTICLES; i++) {
				set_add_blender(0, 0, 0, particle[i]->a);
				rectfill(buffer, particle[i]->x - 32, particle[i]->y - 32, particle[i]->x + 31, particle[i]->y + 31, particle[i]->color);
			}
			drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
		}
		else {
			for (i = 0; i < NUM_PARTICLES; i++)
				fblend_rect_add(buffer, particle[i]->x - 32, particle[i]->y - 32, 64, 64, particle[i]->color, particle[i]->a);
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

		#ifdef COMPARE_WITH_ALLEGRO			
		textprintf(screen, font, 0, 0, makecol(255, 255, 255), "%s  %.2f fps (%.3f avg)", use_alleg ? "Using Allegro" : "Using new", (chrono - old_time2) == 0 ? 1000.0 : 1000 / ((double)chrono - old_time2), count * 1000.0 / chrono);
		#else
		textprintf(buffer, font, 0, 0, makecol(255, 255, 255), "%s  %.2f fps (%.3f avg)", use_alleg ? "Using Allegro" : "Using new", (chrono - old_time2) == 0 ? 1000.0 : 1000 / ((double)chrono - old_time2), count * 1000.0 / chrono);
		#endif
							
		old_time2 = chrono;
	
		
		#ifndef COMPARE_WITH_ALLEGRO
		
		/* Draw the buffer */
		blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		
		#endif

		chrono2 = chrono / 40;		
		for (i = old_time/40; i < chrono2; i++) {
			int j;
			for (j = 0; j < NUM_PARTICLES; j++) {
				particle[j]->x += particle[j]->vx;
				particle[j]->y += particle[j]->vy;
				
				if (particle[j]->x <= 0 || particle[j]->x >= SCREEN_W)
					particle[j]->vx = -particle[j]->vx;
				if (particle[j]->y <= 0 || particle[j]->y >= SCREEN_H)
					particle[j]->vy = -particle[j]->vy;
		
				if (particle[j]->a <= 0 || particle[j]->a >= 255)
					particle[j]->a_dir = -particle[j]->a_dir;
					
				particle[j]->a += particle[j]->a_dir;
			}
		}
		old_time = chrono;
		
	} while (!key[KEY_ESC]);
	
	Error:
	
	TRACE("Shutting down.\n");
	
	if (mysha)
		destroy_bitmap(mysha);
	if (buffer)
		destroy_bitmap(buffer);
	
	for (i = 0; i < NUM_PARTICLES; i++) {
		if (particle[i])
			free(particle[i]);
	}

	set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	
	return 0;	
	
} END_OF_MAIN();
