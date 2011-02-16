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
 
/** \file rtrans.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"

static void fblend_rect_trans_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
static void fblend_rect_trans_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
static void fblend_rect_trans_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);

#ifdef FBLEND_MMX
extern void fblend_rect_trans_mmx_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_trans_mmx_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_trans_mmx_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
#endif
#ifdef FBLEND_SSE
extern void fblend_rect_trans_sse_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_trans_sse_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
extern void fblend_rect_trans_sse_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
#endif



/* void fblend_rect_trans(BITMAP *dst, int x, int y, int w, int h, int color, int fact) */
/** \ingroup primitive_blenders
 *  Draws a colored rectangle into the destination bitmap at coordinates
 *  (x,y)-(x+w-1,y+h-1), using translucency. This function works for 15,
 *  16 and 32 bpp bitmaps.
 *
 * <pre>
 *  dest_red = (color_red * fact / 255) + (dest_red * (255 - fact) / 255)
 * </pre>
 *  Repeat for green and blue, and for all pixels to be displayed.
 *
 *  MMX and SSE will automatically be used if they are present.
 *
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
void fblend_rect_trans(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

#ifdef USE_ALLEGRO_BLENDER
	set_trans_blender(0, 0, 0, fact);
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
		if (fact == 32) {
			rectfill(dst, x, y, w + x - 1, h + y - 1, color);
			return;
		}

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_trans_sse_16(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_trans_mmx_16(dst, x, y, w, h, color, fact);
			else
		#endif
		fblend_rect_trans_16(dst, x, y, w, h, color, fact);
	}
	/* 15 bit code */	
	else if (dst_depth == 15) {
		if (fact == 32) {
			rectfill(dst, x, y, w + x - 1, h + y - 1, color);
			return;
		}

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_trans_sse_15(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_trans_mmx_15(dst, x, y, w, h, color, fact);
			else
		#endif
		fblend_rect_trans_15(dst, x, y, w, h, color, fact);
	}
	/* 32 bit code */	
	else if (dst_depth == 32) {
		fact++;

		if (fact == 256) {
			rectfill(dst, x, y, w + x - 1, h + y - 1, color);
			return;
		}

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_rect_trans_sse_32(dst, x, y, w, h, color, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_rect_trans_mmx_32(dst, x, y, w, h, color, fact);
			else
		#endif
		fblend_rect_trans_32(dst, x, y, w, h, color, fact);
	}

	bmp_unwrite_line(dst);
	release_bitmap(dst);

#endif

	return;
}


static void fblend_rect_trans_16(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

   	int i, j;
   	
   	unsigned long rbg_source, grb_source;
   	
   	color = color | (color << 16);
   	
   	/* Split up components to allow us to do operations on all of them at the same time */
	rbg_source = color & 0xF81F07E0,
	grb_source = color & 0x07E0F81F;

	/* Multiply the source by the factor */
	rbg_source = ((rbg_source >> 5) * fact) & 0xF81F07E0;
	grb_source = ((grb_source * fact) >> 5) & 0x07E0F81F;

	/* Invert factor */
    fact = 32 - fact;

	for (j = 0; j < h; j++) {

		unsigned long *d;
		unsigned long color1;

		/* Read src line */

		bmp_select(dst);
		d = (unsigned long*)(bmp_write_line(dst, y + j) + x * sizeof(short));

		for (i = w >> 1; i; i--) {
			unsigned long temp1;

			/* Read data, 2 pixels at a time */
			color1 = *d;

			/* Split up RGB -> R-B and G */
			temp1 = color1 & 0x7E0F81F;
			color1 &= 0xF81F07E0;

			/* Multiply the source by the factor */
			color1 = ((((color1 >> 5) * fact) & 0xF81F07E0) + rbg_source) & 0xF81F07E0;
			temp1  = ((((temp1 * fact) >> 5)  & 0x07E0F81F) + grb_source) & 0x07E0F81F;

			color1 |= temp1;

 			/* Write the data */
			bmp_write32((unsigned long)d, color1);
			d++;
		}
		
		if (w & 1) {
			unsigned long temp1;

			/* Read data, 1 pixel at a time */
			color1 = *((unsigned short*)d);

			temp1 = color1 & 0xF81F;
			color1 &= 0x07E0;

			/* Multiply the source by the factor */
			color1 = (((color1 >> 5) * fact) + rbg_source) & 0x07E0;
			temp1  = (((temp1 * fact) >> 5)  + grb_source) & 0xF81F;

			color1 |= temp1;

 			/* Write the data */
			bmp_write16((unsigned long)d, color1);
		}
	}
	
	return;
}



static void fblend_rect_trans_15(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

   	int i, j;
   	
   	unsigned long rbg_source, grb_source;
   	
   	color = color | (color << 16);
   	
   	/* Split up components to allow us to do operations on all of them at the same time */
	rbg_source = color & 0x7C1F03E0,
	grb_source = color & 0x03E07C1F;

	/* Multiply the source by the factor */
	rbg_source = ((rbg_source >> 5) * fact) & 0x7C1F03E0;
	grb_source = ((grb_source * fact) >> 5) & 0x03E07C1F;

	/* Invert factor */
    fact = 32 - fact;

	for (j = 0; j < h; j++) {

		unsigned long *d;
		unsigned long color1;

		/* Read src line */

		bmp_select(dst);
		d = (unsigned long*)(bmp_write_line(dst, y + j) + x * sizeof(short));

		for (i = w >> 1; i; i--) {
			unsigned long temp1;

			/* Read data, 2 pixels at a time */
			color1 = *d;

			/* Split up RGB -> R-B and G */
			temp1 = color1 & 0x3E07C1F;
			color1 &= 0x7C1F03E0;

			/* Multiply the source by the factor */
			color1 = ((((color1 >> 5) * fact) & 0x7C1F03E0) + rbg_source) & 0x7C1F03E0;
			temp1  = ((((temp1 * fact) >> 5)  & 0x03E07C1F) + grb_source) & 0x03E07C1F;

			color1 |= temp1;

 			/* Write the data */
			bmp_write32((unsigned long)d, color1);
			d++;
		}
		
		if (w & 1) {
			unsigned long temp1;

			/* Read data, 1 pixel at a time */
			color1 = *((unsigned short*)d);

			temp1 = color1 & 0x7C1F;
			color1 &= 0x03E0;

			/* Multiply the source by the factor */
			color1 = (((color1 >> 5) * fact) + rbg_source) & 0x03E0;
			temp1  = (((temp1 * fact) >> 5)  + grb_source) & 0x7C1F;

			color1 |= temp1;

 			/* Write the data */
			bmp_write16((unsigned long)d, color1);
		}
	}
	
	return;
}



static void fblend_rect_trans_32(BITMAP *dst, int x, int y, int w, int h, int color, int fact) {

	int i, j;
	uint32_t rb_source = color & 0xFF00FF,
			g_source = color & 0x00FF00;

    if (fact != 256) {
		/* Multiply the source by the factor */
		rb_source = (((rb_source * fact) >> 8) & 0xFF00FF);
		g_source =  (((g_source  * fact) >> 8) & 0x00FF00);
    }
    
    fact = 256 - fact;

	for (j = 0; j < h; j++) {

		uint32_t *d;
		uint32_t color1;
		uint32_t temp1;
	
		/* Read src line */
		d = ((uint32_t *)bmp_write_line(dst, y + j)) + x;

		for (i = w; i; i--) {
			/* Read data, 1 pixel at a time */
			color1 = *d;

			temp1 = color1 & 0xFF00FF;
			color1 &= 0x00FF00;
			
			temp1  = (((temp1  * fact) >> 8) + rb_source) & 0xFF00FF;
			color1 = (((color1 * fact) >> 8) + g_source)  & 0x00FF00;
	 
 			/* Write the data */
			bmp_write32((unsigned long) d, color1 | temp1);
			d++;
		}
	}

	return;
}

