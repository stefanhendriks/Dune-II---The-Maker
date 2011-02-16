/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 *
 * Fast primitive color add routines for Allegro
 *
 * This function is intended to replace the awefully slow 16 bit add
 * blender in Allegro.
 */
 
/** \file radd.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"

static void fblend_rect_add_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
static void fblend_rect_add_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
static void fblend_rect_add_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);

#ifdef FBLEND_MMX
extern void fblend_rect_add_mmx_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_add_mmx_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_add_mmx_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
#endif
#ifdef FBLEND_SSE
extern void fblend_rect_add_sse_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_add_sse_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_add_sse_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
#endif



/* void fblend_rect_add(BITMAP *dst, int x, int y, int w, int h, int color, int fact) */
/** \ingroup primitive_blenders
 *  Draws a colored rectangle into the destination bitmap at coordinates
 *  (x,y)-(x+w-1,y+h-1), using add+saturation. This function works for 15,
 *  16 and 32 bpp bitmaps.
 *
 * <pre>
 *  dest_red = saturate(dest_red + color_red * fact / 255)
 * </pre>
 *  Repeat for green and blue, and for all pixels to be displayed.
 *
 *  MMX and SSE will automatically be used if they are present.
 *
 *  \note The green component loses one bit of precision in the 16 bpp C version.
 *        but the MMX and SSE versions display correctly.
 *  \note Drawing into non-memory bitmaps is usually very slow due to system
 *        architecture.
 *
 *  \param dst  The destination bitmap. Must be in 15, 16 or 32 bpp
 *              and linear, but not necessarily a memory bitmap.
 *  \param x    Destination coordinate on the x axis.
 *  \param y    Destination coordinate on the y axis.
 *  \param w    Width of the rectangle, in pixels
 *  \param h    Height of the rectangle, in pixels
 *  \param color Color of the rectangle, as created by makecol().
 *  \param fact Blend factor. Should be in the range 0-255, but no
 *              checking is done.
 */
void fblend_rect_add(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

#ifdef USE_ALLEGRO_BLENDER
	set_add_blender(0, 0, 0, fact);
	drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
	rectfill(dst, x, y, x + w - 1, y + h - 1, color);
	drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
#else

	int dst_depth;
	
	/* Clip the image */
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x + w >= dst->w)
		w -= x + w - dst->w;
	if (y + h >= dst->h)
		h -= y + h - dst->h;

	/* Nothing to do? */
	if (w < 1 || h < 1)
		return;	

	dst_depth = bitmap_color_depth(dst);
	
	/* Incorrct color depths */
	if (dst_depth != 16 && dst_depth != 15 && dst_depth != 32)
		return;
		
	if (dst_depth == 15 || dst_depth == 16)
		/* Adjust factor for 0->32 range */
		fact = (fact + 7) >> 3;

	/* Check if nothing to do */
	if (fact <= 0)
		return;

	acquire_bitmap(dst);

	/* 16 bit code */	
	if (dst_depth == 16) {
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_add_sse_16(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_add_mmx_16(dst, x, y, w, h, color, fact);
			else
		#endif

		fblend_rect_add_16(dst, x, y, w, h, color, fact);
	}
	/* 15 bit code */	
	else if (dst_depth == 15) {

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_add_sse_15(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_add_mmx_15(dst, x, y, w, h, color, fact);
			else
		#endif

		fblend_rect_add_15(dst, x, y, w, h, color, fact);
	}
	/* 32 bit code */	
	else if (dst_depth == 32) {
		fact++;

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_add_sse_32(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_add_mmx_32(dst, x, y, w, h, color, fact);
			else
		#endif
		fblend_rect_add_32(dst, x, y, w, h, color, fact);
	}

	bmp_unwrite_line(dst);
	release_bitmap(dst);

#endif

	return;
}


static void fblend_rect_add_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

	int i, j;

	/* Seperate out the components */
	color = ((color << 16) | color) & 0x7C0F81F;

    if (fact != 32) {
		/* Multiply the source by the factor */
		color = ((color * fact) >> 5) & 0x7C0F81F;    
    }
		
	for (j = 0; j < h; j++) {

		unsigned short *d;
		unsigned long color1;
		unsigned long res;
	
		/* Read src line */
		bmp_select(dst);
		d = (unsigned short*)(bmp_write_line(dst, y + j) + x * sizeof(short));

		for (i = w; i; i--) {
			/* Read data, 1 pixel at a time */
			color1 = *d;

			color1 = ((color1 << 16) | color1) & 0x7C0F81F;
	
			/* Do the conditionless add with saturation */			
			color1 += color;
			res = color1 & 0x8010020;
			res -= (res >> 5);
			color1 |= res;
							
			/* Recombine the components */
			color1 &= 0x7C0F81F;
			color1 |= (color1 >> 16);
			color1 &= 0xFFFF;
	 
 			/* Write the data */
			bmp_write16((unsigned long)d, color1);
			d++;
		}
	}
	
	return;
}


static void fblend_rect_add_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

	int i, j;

	/* Seperate out the components */
	color = ((color << 16) | color) & 0x3E07C1F;

    if (fact != 32) {
		/* Multiply the source by the factor */
		color = ((color * fact) >> 5) & 0x3E07C1F;    
    }
		
	for (j = 0; j < h; j++) {

		unsigned short *d;
		unsigned long color1;
		unsigned long res;
	
		/* Read src line */
		bmp_select(dst);
		d = (unsigned short*)(bmp_write_line(dst, y + j) + x * sizeof(short));

		for (i = w; i; i--) {
			/* Read data, 1 pixel at a time */
			color1 = *d;

			color1 = ((color1 << 16) | color1) & 0x3E07C1F;
	
			/* Do the conditionless add with saturation */			
			color1 += color;
			res = color1 & 0x4008020;
			res -= (res >> 5);
			color1 |= res;
							
			/* Recombine the components */
			color1 &= 0x3E07C1F;
			color1 |= (color1 >> 16);
			color1 &= 0xFFFF;
	 
 			/* Write the data */
			bmp_write16((unsigned long)d, color1);
			d++;
		}
	}
	
	return;
}


static void fblend_rect_add_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

	int i, j;
	uint32_t msb_mask, low_mask;

    if (fact != 256) {
		/* Multiply the source by the factor */
		color = ((((color & 0xFF00FF) * fact) >> 8) & 0xFF00FF) | ((((color & 0x00FF00) * fact) >> 8) & 0x00FF00);
    }
    
    msb_mask = color & 0x808080;
    low_mask = color & 0x7F7F7F;

	for (j = 0; j < h; j++) {

		uint32_t *d;
		uint32_t color1;
		uint32_t temp1, temp2;
	
		/* Read src line */
		d = ((uint32_t *)(bmp_write_line(dst, y + j))) + x;		    


		for (i = w; i; i--) {
			/* Read data, 1 pixel at a time */
			color1 = *d;

			temp1 = color1 & 0x808080;
			color1 = (color1 & 0x7F7F7F) + low_mask;

			temp2 = temp1;
			temp1 = temp1 | msb_mask;
				
			color1 |= (((((temp2 & msb_mask) | (temp1 & color1)) >> 7) + 0x7F7F7F) ^ 0x7F7F7F) | temp1;
	 
 			/* Write the data */
			bmp_write32((unsigned long)d, color1);
			d++;
		}
	}
	
	return;
}

