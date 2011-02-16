/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 *
 * Fast 2x image stretcher
 *
 */
 
/** \file 2xstretch.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"


#ifdef FBLEND_MMX
extern void fblend_2x_stretch_16_mmx(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int s_w, int s_h, int d_w, int d_h);
extern void fblend_2x_stretch_32_mmx(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int s_w, int s_h, int d_w, int d_h);
#endif
#ifdef FBLEND_SSE
extern void fblend_2x_stretch_16_sse(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int s_w, int s_h, int d_w, int d_h);
extern void fblend_2x_stretch_32_sse(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int s_w, int s_h, int d_w, int d_h);
#endif


/* void fblend_2x_stretch(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int w, int h) */
/** \ingroup stretchers
 *  Stretch-blits the source bitmap over the destination bitmap. This is
 *  equivalent to Allegro's
 *  <pre>
 *    stretch_blit(src, dst, s_x, s_y, w, h, d_x, d_y, w * 2, h * 2);
 *  </pre>
 *
 *  Only 2x stretching is supported by this function.
 *
 *  Both source and destination bitmaps must be of the same color depth.
 *
 *  \param src  The source bitmap. Must be in 15, 16 or 32 bpp
 *              and linear, but not necessarily a memory bitmap.
 *  \param dst  The destination bitmap. Must be in 15, 16 or 32 bpp
 *              and linear, but not necessarily a memory bitmap.
 *  \param s_x  Source coordinate on the x axis.
 *  \param s_y  Source coordinate on the y axis.
 *  \param d_x  Destination coordinate on the x axis.
 *  \param d_y  Destination coordinate on the y axis.
 *  \param w    Width of the source area to stretch, in pixels
 *  \param h    Height of the source area to stretch, in pixels
 */
void fblend_2x_stretch(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int w, int h) {

#ifdef USE_ALLEGRO_BLENDER
	stretch_blit(src, dst, s_x, s_y, w, h, d_x, d_y, w << 1, h << 1);
#else

	int dst_depth, src_depth;
	
	/* check for ridiculous cases */
	if ((s_x >= src->cr) || (s_y >= src->cb) ||
		(d_x >= dst->cr) || (d_y >= dst->cb))
		return;

	if ((s_x + w < src->cl) || (s_y + h < src->ct) ||
		(d_x + w * 2 < dst->cl) || (d_y + h * 2 < dst->ct))
		return;

	/* clip src left */
	if (s_x < src->cl) {
		w += s_x - src->cl;
		d_x -= (s_x - src->cl) * 2;
		s_x = src->cl;
	}

	/* clip src top */
	if (s_y < src->ct) {
		h += s_y - src->ct;
		d_y -= (s_y - src->ct) * 2;
		s_y = src->ct;
	}

	/* clip src right */
	if (s_x + w > src->cr)
		w = src->cr - s_x;

	/* clip src bottom */
	if (s_y + h > src->cb)
		h = src->cb - s_y;

	w <<= 1;  h <<= 1;

	/* clip dest left */
	if (d_x < dst->cl) {
		w += d_x - dst->cl;
		s_x -= (d_x - dst->cl + 1) >> 1;
		d_x = dst->cl;
	}

	/* clip dest top */
	if (d_y < dst->ct) {
		h += d_y - dst->ct;
		s_y -= (d_y - dst->ct + 1) >> 1;
		d_y = dst->ct;
	}

	/* clip dest right */
	if (d_x + w > dst->cr)
		w = dst->cr - d_x;

	/* clip dest bottom */
	if (d_y + h > dst->cb)
		h = dst->cb - d_y;

	/* bottle out if zero size */
	if ((w < 1) || (h < 1))
		return;
		
	dst_depth = bitmap_color_depth(dst);
	src_depth = bitmap_color_depth(src);

	/* Incorrct color depths */
	if (!(src_depth == 16 && dst_depth == 16)
			&& !(src_depth == 15 && dst_depth == 15)
			&& !(src_depth == 32 && dst_depth == 32))
		return;

	acquire_bitmap(dst);

	/* 15/16 bit code */	
	if (dst_depth == 16 || dst_depth == 15) {
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE) {
				fblend_2x_stretch_16_sse(src, dst, s_x, s_y, d_x, d_y, w >> 1, h >> 1, w, h);
			}
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX) {
				fblend_2x_stretch_16_mmx(src, dst, s_x, s_y, d_x, d_y, w >> 1, h >> 1, w, h);
			}
			else
		#endif
		stretch_blit(src, dst, s_x, s_y, w >> 1, h >> 1, d_x, d_y, w, h);
	}
	/* 32 bit code */	
	else if (dst_depth == 32) {
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE) {
				fblend_2x_stretch_32_sse(src, dst, s_x, s_y, d_x, d_y, w >> 1, h >> 1, w, h);
			}
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX) {
				fblend_2x_stretch_32_mmx(src, dst, s_x, s_y, d_x, d_y, w >> 1, h >> 1, w, h);
			}
			else
		#endif
		stretch_blit(src, dst, s_x, s_y, w >> 1, h >> 1, d_x, d_y, w, h);
	}

	bmp_unwrite_line(dst);
	release_bitmap(dst);

#endif

	return;
}

