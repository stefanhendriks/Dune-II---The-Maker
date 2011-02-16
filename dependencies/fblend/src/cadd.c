/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 *
 * Fast color add routines for Allegro
 *
 * This function is intended to replace the awefully slow 16 bit add
 * blender in Allegro.
 */

/** \file cadd.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"


static void fblend_add_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
static void fblend_add_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
static void fblend_add_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);

#ifdef FBLEND_MMX
extern void fblend_add_mmx_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_add_mmx_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_add_mmx_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
#endif
#ifdef FBLEND_SSE
extern void fblend_add_sse_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_add_sse_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_add_sse_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
#endif



/* void fblend_add(BITMAP *src, BITMAP *dst, int x, int y, int fact) */
/** \ingroup bitmap_blenders
 *  Draws the source bitmap into the destination bitmap at coordinates
 *  (x,y), using add+saturation. This function works for 15, 16 and 32 bpp
 *  bitmaps, but both source and destination must be of the same depth.
 *
 * <pre>
 *  dest_red = saturate(dest_red + src_red * fact / 255)
 * </pre>
 *  Repeat for green and blue, and for all pixels to be displayed.
 *
 *  The 15, 16 or 32bpp mask color is also taken in consideration, so those
 *  pixels aren't blended at all, just like in Allegro.
 *  MMX and SSE will automatically be used if they are present.
 *
 *  \note The green component loses one bit of precision in the 16 bpp C version.
 *        but the MMX and SSE versions display correctly.
 *  \note Drawing into non-memory bitmaps is usually very slow due to system
 *        architecture.
 *
 *  \param src  The source bitmap. Must be a linear memory bitmap or
 *              sub-bitmap thereof, and must be in 15, 16 or 32 bpp.
 *  \param dst  The destination bitmap. Must be in 15, 16 or 32 bpp
 *              and linear, but not necessarily a memory bitmap.
 *  \param x    Destination coordinate on the x axis.
 *  \param y    Destination coordinate on the y axis.
 *  \param fact Blend factor. Should be in the range 0-255, but no
 *              checking is done.
 */
void fblend_add(BITMAP *src, BITMAP *dst, int x, int y, int fact) {

#ifdef USE_ALLEGRO_BLENDER
	set_add_blender(0, 0, 0, fact);
	draw_trans_sprite(dst, src, x, y);
#else

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

	if (dst_depth == 15 || dst_depth == 16)
		/* Adjust factor for 0->32 range */
		fact = (fact + 7) >> 3;

	/* Check if nothing to do */
	if (fact <= 0)
		return;

	acquire_bitmap(dst);

	/* 16 bit code */
	if (src_depth == 16) {

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_add_sse_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_add_mmx_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		fblend_add_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}
	/* 15 bit code */
	else if (src_depth == 15) {
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_add_sse_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_add_mmx_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		fblend_add_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}
	/* 32 bit code */
	else if (src_depth == 32) {

		/* Check if nothing to do */
		if (fact <= 0)
			return;
		else
			fact++;

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_add_sse_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_add_mmx_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		fblend_add_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}

	bmp_unwrite_line(dst);
	release_bitmap(dst);

#endif

	return;
}


static void fblend_add_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {

    if (fact == 32) {
    	int i, j;

		for (j = 0; j < h; j++) {

			unsigned short *s, *d;
			unsigned long color1, color2;
			unsigned long res;

			/* Read src line */

			bmp_select(dst);
			s = (unsigned short*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned short*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color2 = *s;

				if (*s == MASK_COLOR_16) {
					d++; s++;
					continue;
				}

				color1 = *d;

				/* Seperate out the components */
				color2 = ((color2 << 16) | color2) & 0x7C0F81F;
				color1 = ((color1 << 16) | color1) & 0x7C0F81F;

				/* Do the conditionless add with saturation */
				color1 += color2;
				res = color1 & 0x8010020;
				res -= (res >> 5);
				color1 |= res;

				/* Recombine the components */
				color1 &= 0x7C0F81F;
				color1 |= (color1 >> 16);
				color1 &= 0xFFFF;

	 			/* Write the data */
				s++;
				bmp_write16((unsigned long)d, color1);
				d++;
			}
		}
	}
	else {
		int i, j;

		for (j = 0; j < h; j++) {

			unsigned short *s, *d;
			unsigned long color1, color2;
			unsigned long res;

			/* Read src line */

			bmp_select(dst);
			s = (unsigned short*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned short*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color2 = *s;

				if (*s == MASK_COLOR_16) {
					d++; s++;
					continue;
				}

				color1 = *d;

				/* Seperate out the components */
				color2 = ((color2 << 16) | color2) & 0x7C0F81F;

				/* Multiply the source by the factor */
				color2 = ((color2 * fact) >> 5) & 0x7C0F81F;

				color1 = ((color1 << 16) | color1) & 0x7C0F81F;

				/* Do the conditionless add with saturation */
				color1 += color2;
				res = color1 & 0x8010020;
				res -= (res >> 5);
				color1 |= res;

				/* Recombine the components */
				color1 &= 0x7C0F81F;
				color1 |= (color1 >> 16);
				color1 &= 0xFFFF;

	 			/* Write the data */
				s++;
				bmp_write16((unsigned long)d, color1);
				d++;
			}
		}
	}

	return;
}


static void fblend_add_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {

	if (fact == 32) {

    	int i, j;

		for (j = 0; j < h; j++) {

			unsigned short *s, *d;
			unsigned long color1, color2;
			unsigned long res;

			/* Read src line */

			bmp_select(dst);
			s = (unsigned short*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned short*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color2 = *s;

				if (*s == MASK_COLOR_15) {
					d++; s++;
					continue;
				}

				color1 = *d;

				/* Seperate out the components */
				color2 = ((color2 << 16) | color2) & 0x3E07C1F;
				color1 = ((color1 << 16) | color1) & 0x3E07C1F;

				/* Do the conditionless add with saturation */
				color1 += color2;
				res = color1 & 0x4008020;
				res -= (res >> 5);
				color1 |= res;

				/* Recombine the components */
				color1 &= 0x3E07C1F;
				color1 |= (color1 >> 16);
				color1 &= 0x7FFF;

 				/* Write the data */
				s++;
				bmp_write16((unsigned long)d, color1);
				d++;
			}
		}
   	}
   	else {
   		int i, j;
		for (j = 0; j < h; j++) {

			unsigned short *s, *d;
			unsigned long color1, color2;
			unsigned long res;

			/* Read src line */

			bmp_select(dst);
			s = (unsigned short*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned short*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color2 = *s;

				if (*s == MASK_COLOR_15) {
					d++; s++;
					continue;
				}

				color1 = *d;

				/* Seperate out the components */
				color2 = ((color2 << 16) | color2) & 0x3E07C1F;

				/* Multiply the source by the factor */
				color2 = ((color2 * fact) >> 5) & 0x3E07C1F;

				color1 = ((color1 << 16) | color1) & 0x3E07C1F;

				/* Do the conditionless add with saturation */
				color1 += color2;
				res = color1 & 0x4008020;
				res -= (res >> 5);
				color1 |= res;

				/* Recombine the components */
				color1 &= 0x3E07C1F;
				color1 |= (color1 >> 16);
				color1 &= 0x7FFF;

 				/* Write the data */
				s++;
				bmp_write16((unsigned long)d, color1);
				d++;
			}
		}
	}

	return;
}



static void fblend_add_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {

    if (fact == 256) {
    	int i, j;

		for (j = 0; j < h; j++) {

			uint32_t *s, *d;
			uint32_t color1, color2;
			uint32_t temp1, temp2;

			/* Read src line */

			bmp_select(dst);
			s = ((uint32_t *)(bmp_read_line(src, src_y + j))) + src_x;
			d = ((uint32_t *)(bmp_write_line(dst, dst_y + j))) + dst_x;

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color1 = *s;

				if (color1 == MASK_COLOR_32) {
					d++; s++;
					continue;
				}

				color2 = *d;

				temp1 = color1 & 0x808080;
				temp2 = color2 & 0x808080;
				color1 = (color1 & 0x7F7F7F) + (color2 & 0x7F7F7F);

				color2 = temp1;
				temp1 = temp1 | temp2;
				temp2 = color2 & temp2;

				color2 = temp1 & color1;
				temp2 = (temp2 | color2) >> 7;

				color1 |= ((temp2 + 0x7F7F7F) ^ 0x7F7F7F) | temp1;

	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
		}
	}
	else {
		int i, j;

		for (j = 0; j < h; j++) {

			uint32_t *s, *d;
			uint32_t color1, color2;
			uint32_t temp1, temp2;

			/* Read src line */

			bmp_select(dst);
			s = ((uint32_t *)(bmp_read_line(src, src_y + j))) + src_x;
			d = ((uint32_t *)(bmp_write_line(dst, dst_y + j))) + dst_x;

			for (i = w; i; i--) {
				/* Read data, 1 pixel at a time */
				color1 = *s;

				if (color1 == MASK_COLOR_32) {
					d++; s++;
					continue;
				}

				color2 = *d;

				temp1 = color1 & 0xFF00FF;
				temp2 = color1 & 0x00FF00;

				temp1 = ((temp1 * fact) >> 8) & 0xFF00FF;
				temp2 = ((temp2 * fact) >> 8) & 0x00FF00;

				color1 = temp1 | temp2;

				temp1 = color1 & 0x808080;
				temp2 = color2 & 0x808080;
				color1 = (color1 & 0x7F7F7F) + (color2 & 0x7F7F7F);

				color2 = temp1;
				temp1 = temp1 | temp2;
				temp2 = color2 & temp2;

				color2 = temp1 & color1;

				color1 |= ((((temp2 | color2) >> 7) + 0x7F7F7F) ^ 0x7F7F7F) | temp1;

	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
		}
	}

	return;
}
