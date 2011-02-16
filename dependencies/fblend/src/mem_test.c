
#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"

extern void fblend_mem_test_mmx_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_mem_test_sse_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);

void mem_test(BITMAP *src, BITMAP *dst, int x, int y) {

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
	if (!(src_depth == 16 && dst_depth == 16))
		return;
		
	acquire_bitmap(dst);

	/* 16 bit code */	
	if (src_depth == 16) {
		
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_mem_test_sse_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, 0);
			else
		#endif
		
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_mem_test_mmx_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, 0);
			else
		#endif
		
		{} //fblend_mem_test_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, 0);
	}

	bmp_unwrite_line(dst);
	release_bitmap(dst);

	return;
}
