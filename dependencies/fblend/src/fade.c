/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 *
 * Fast translucency routines for Allegro
 *
 * This function is intended to replace the awefully slow 16 bit trans
 * blender in Allegro.
 */

/** \file fade.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"

static void fblend_fade_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);
static void fblend_fade_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);
static void fblend_fade_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);

#ifdef FBLEND_MMX
extern void fblend_fade_mmx_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);
extern void fblend_fade_mmx_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);
extern void fblend_fade_mmx_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact);
#endif



/* void fblend_fade_to_color(BITMAP *src, BITMAP *dst, int x, int y, int color, int fact) */
/** \ingroup faders
 *  Fades source bitmap from/to a color and places output bitmap into the
 *  destination at the specified coordinates. Source and destination
 *  bitmaps must have the same color depth; supported color depths are 15,
 *  16 and 32 bpp. Fact should range 0-255, and it is used to specify the
 *  fade factor with which the destination bitmap will be drawn.
 *  A factor of 255 will render the original bitmap, whereas a factor of 0
 *  will only draw a rectangle of the specified color.
 *
 * <pre>
 *  dest_red = (src_red * fact / 255) + (color * (255 - fact) / 255)
 * </pre>
 *  Repeat for green and blue, and for all pixels to be displayed.
 *
 *  MMX and SSE will automatically be used if they are present.
 *
 *  \param src  The source bitmap. Must be a linear memory bitmap or
 *              sub-bitmap thereof, and must be in 15, 16 or 32 bpp.
 *  \param dst  The destination bitmap. Must be in 15, 16 or 32 bpp
 *              and linear, but not necessarily a memory bitmap.
 *  \param x    The coordinate on the X axis of teh destination bitmap
 *              to place the faded source bitmap at.
 *  \param y    The coordinate on the Y axis of teh destination bitmap
 *              to place the faded source bitmap at.
 *  \param color The color to fade to. This needs to be in the same depth
 *              as the bitmaps.
 *  \param fact Fade factor. Should be in the range 0-255.
 */
void fblend_fade_to_color(BITMAP *src, BITMAP *dst, int x, int y, int color, int fact) {

    int w, h;
	
	int src_x = 0, src_y = 0, dst_x = x, dst_y = y;
	int src_depth, dst_depth;
	
	w = src->w;
	h = src->h;
	
	/* Clip the image */
	if (dst_x < 0) {
		w += dst_x;
		src_x -= dst_x;
		dst_x = 0;
	}
	if (dst_y < 0) {
		h += dst_y;
		src_y -= dst_y;
		dst_y = 0;
	}
	if (dst_x + w >= dst->w)
		w -= dst_x + w - dst->w;
	if (dst_y + h >= dst->h)
		h -= dst_y + h - dst->h;

	/* Nothing to do? */
	if (w < 1 || h < 1)
		return;
	
	src_depth = bitmap_color_depth(src);
	dst_depth = bitmap_color_depth(dst);
	
	/* Incorrct color depths */
	if (!(src_depth == 16 && dst_depth == 16)
			&& !(src_depth == 15 && dst_depth == 15)
			&& !(src_depth == 32 && dst_depth == 32))
		return;
		
	/* Can we do a blit or rectfill instead? */
	if (fact >= 255) {
		blit(src, dst, src_x, src_y, dst_x, dst_y, w, h);
		return;
	}
	if (fact <= 0) {
		rectfill(dst, dst_x, dst_y, dst_x + w - 1, dst_y + h - 1, color);
		return;
	}

	acquire_bitmap(dst);

	/* 16 bit code */	
	if (src_depth == 16) {

		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX) {
				fact = (fact + 7) >> 3;
				fblend_fade_mmx_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
			}
			else
		#endif

		fblend_fade_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
	}

	/* 15 bit code */	
	else if (src_depth == 15) {

		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX) {
				fact = (fact + 7) >> 3;
				fblend_fade_mmx_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
			}
			else
		#endif

		fblend_fade_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
	}

	/* 32 bit code */	
	else if (src_depth == 32) {

		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX) {
				fblend_fade_mmx_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
			}
			else
		#endif

		fblend_fade_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, color, fact);
	}
	
	bmp_unwrite_line(dst);
	release_bitmap(dst);

	return;
}


static void fblend_fade_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact) {

   	unsigned long rbg_source, grb_source;
   	int i, j;
	unsigned long *s, *d;
	unsigned long color1;
	unsigned long temp1;


	/* Map to 0-32 range */
	fact = (fact + 7) >> 3;
	
   	color = color | (color << 16);
   	
   	/* Split up components to allow us to do operations on all of them at the same time */
	rbg_source = color & 0xF81F07E0;
	grb_source = color & 0x07E0F81F;

	/* Multiply the source by the factor */
	rbg_source = ((rbg_source >> 5) * (32 - fact)) & 0xF81F07E0;
	grb_source = ((grb_source * (32 - fact)) >> 5) & 0x07E0F81F;

	for (j = 0; j < h; j++) {

		/* Read src line */
		bmp_select(dst);
		s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
		d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

		for (i = w >> 1; i; i--) {
			/* Read data, 2 pixels at a time */
			color1 = *s;

			temp1 = color1 & 0x7E0F81F;
			color1 &= 0xF81F07E0;

			/* Multiply the source by the factor */
			color1 = ((((color1 >> 5) * fact) & 0xF81F07E0) + rbg_source) & 0xF81F07E0;
			temp1  = ((((temp1 * fact) >> 5)  & 0x07E0F81F) + grb_source) & 0x07E0F81F;

			color1 |= temp1;

 			/* Write the data */
			s++;
			bmp_write32((unsigned long)d, color1);
			d++;
		}
		
		if (w & 1) {
			unsigned long temp1;

			/* Read 1 pixel */
			color1 = *((unsigned short*)s);
				
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


static void fblend_fade_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact) {

   	unsigned long rbg_source, grb_source;
   	int i, j;
	unsigned long *s, *d;
	unsigned long color1;
	unsigned long temp1;


	/* Map to 0-32 range */
	fact = (fact + 7) >> 3;
	
   	color = color | (color << 16);
   	
   	/* Split up components to allow us to do operations on all of them at the same time */
	rbg_source = color & 0x7C1F03E0;
	grb_source = color & 0x03E07C1F;

	/* Multiply the source by the factor */
	rbg_source = ((rbg_source >> 5) * (32 - fact)) & 0x7C1F03E0;
	grb_source = ((grb_source * (32 - fact)) >> 5) & 0x03E07C1F;

	for (j = 0; j < h; j++) {

		/* Read src line */
		bmp_select(dst);
		s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
		d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

		for (i = w >> 1; i; i--) {
			/* Read data, 2 pixels at a time */
			color1 = *s;

			temp1 = color1 & 0x03E07C1F;
			color1 &= 0x7C1F03E0;

			/* Multiply the source by the factor */
			color1 = ((((color1 >> 5) * fact) & 0x7C1F03E0) + rbg_source) & 0x7C1F03E0;
			temp1  = ((((temp1 * fact) >> 5)  & 0x03E07C1F) + grb_source) & 0x03E07C1F;

			color1 |= temp1;

 			/* Write the data */
			s++;
			bmp_write32((unsigned long)d, color1);
			d++;


		}
		
		if (w & 1) {
			unsigned long temp1;

			/* Read 1 pixel */
			color1 = *((unsigned short*)s);
				
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


static void fblend_fade_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int color, int fact) {

   	unsigned long rb_source, g_source;
   	int i, j;
	uint32_t *s, *d;
	uint32_t color1;
	uint32_t temp1;

   	/* Split up components to allow us to do operations on all of them at the same time */
	rb_source = color & 0x00FF00FF;
	g_source  = color & 0x0000FF00;
	
	/* Multiply the source by the factor */
	rb_source = ((rb_source * (256 - fact)) >> 8) & 0xFF00FF;
	g_source =  ((g_source >> 8) * (256 - fact)) & 0x00FF00;

	for (j = 0; j < h; j++) {

		/* Read src line */
		s = ((uint32_t *)(bmp_read_line(src, src_y + j))) + src_x;
		d = ((uint32_t *)(bmp_write_line(dst, dst_y + j))) + dst_x;		    


		for (i = w; i; i--) {
			color1 = *s;

			temp1 = color1 & 0xFF00FF;
			color1 &= 0x00FF00;

			/* Multiply the source by the factor */
			color1 = ((((color1 >> 8) * fact) & 0x00FF00) + g_source)  & 0x00FF00;
			temp1  = ((((temp1 * fact) >> 8)  & 0xFF00FF) + rb_source) & 0xFF00FF;

			color1 |= temp1;

 			/* Write the data */
			s++;
			bmp_write32((unsigned long)d, color1);
			d++;
		}
	}

	return;
}
