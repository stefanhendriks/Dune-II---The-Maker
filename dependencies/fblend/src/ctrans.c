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

/** \file ctrans.c
 */

#include "allegro.h"
#include "fblend.h"
#include "mmx.h"
#include "sse.h"


static void fblend_trans_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
static void fblend_trans_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
static void fblend_trans_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);

#ifdef FBLEND_MMX
extern void fblend_trans_mmx_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_trans_mmx_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_trans_mmx_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
#endif
#ifdef FBLEND_SSE
extern void fblend_trans_sse_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_trans_sse_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
extern void fblend_trans_sse_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact);
#endif



/* void fblend_trans(BITMAP *src, BITMAP *dst, int x, int y, int fact) */
/** \ingroup bitmap_blenders
 *  Draws the source bitmap into the destination bitmap at coordinates
 *  (x,y), using translucency. This function works for 15, 16 and 32 bpp
 *  bitmaps, but both source and destination must be of the same depth.
 *
 * <pre>
 *  dest_red = (src_red * fact / 255) + (dest_red * (255 - fact) / 255)
 * </pre>
 *  Repeat for green and blue, and for all pixels to be displayed.
 *
 *  The 15, 16 or 32 bpp mask color is also taken in consideration, so those
 *  pixels aren't blended at all, just like in Allegro.
 *  MMX and SSE will automatically be used if they are present.
 *
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
void fblend_trans(BITMAP *src, BITMAP *dst, int x, int y, int fact) {

#ifdef USE_ALLEGRO_BLENDER
	set_trans_blender(0, 0, 0, fact);
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
		
	if (dst_depth == 15 || dst_depth == 16) {
		/* Adjust factor for 0->32 range */
		fact = (fact + 7) >> 3;
		/* Can we do a masked_blit instead? */
		if (fact == 32) {
			masked_blit(src, dst, src_x, src_y, dst_x, dst_y, w, h);
			return;
		}
	}
	else {
		/* Can we do a masked_blit instead? */
		if (fact == 255) {
			masked_blit(src, dst, src_x, src_y, dst_x, dst_y, w, h);
			return;
		}
	}

	/* Check if nothing to do */
	if (fact <= 0)
		return;
	

	acquire_bitmap(dst);

	/* 16 bit code */	
	if (src_depth == 16) {

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_trans_sse_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_trans_mmx_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif

		fblend_trans_16(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}
	/* 15 bit code */	
	else if (src_depth == 15) {

		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_trans_sse_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_trans_mmx_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif

		fblend_trans_15(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}
	/* 32 bit code */
	else if (src_depth == 32) {
		#ifdef FBLEND_SSE
			if (cpu_capabilities & CPU_SSE)
				fblend_trans_sse_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		#ifdef FBLEND_MMX
			if (cpu_capabilities & CPU_MMX)
				fblend_trans_mmx_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
			else
		#endif
		fblend_trans_32(src, dst, src_x, src_y, dst_x, dst_y, w, h, fact);
	}
	
	bmp_unwrite_line(dst);
	release_bitmap(dst);


	#endif

	return;
}


static void fblend_trans_16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {
	if (fact == 16) {
	   	int i, j;
    	
		for (j = 0; j < h; j++) {

			unsigned long *s, *d;
			unsigned long color1, color2;
		
			/* Read src line */			
			bmp_select(dst);
			s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));
		    
			for (i = w >> 1; i; i--) {
 
				/* Read data, 2 pixels at a time */
				color2 = *s;
				color1 = *d;
					
				if ((color2 & 0xFFFF) == MASK_COLOR_16) {
					color2 &= 0xFFFF0000;
					color2 |= (color1 & 0xFFFF);
				}
				if ((color2 >> 16) == MASK_COLOR_16) {
					color2 &= 0xFFFF;
					color2 |= (color1 << 16);
				}

				color1 = (((color1 & 0xF7DEF7DE) >> 1)
						+ ((color2 & 0xF7DEF7DE) >> 1)
						+ (color1 & color2 & 0x08210821));

	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
			
			if (w & 1) {
				/* Read 1 pixel */
				color2 = *((unsigned short*)s);
				color1 = *((unsigned short*)d);
				
				if (color2 == MASK_COLOR_16)
					color2 = color1;

				color1 = (((color1 & 0xF7DE) >> 1)
						+ ((color2 & 0xF7DE) >> 1)
						+ (color1 & color2 & 0x0821));
				
	 			/* Write the data */
				bmp_write16((unsigned long)d, color1);
			}
		}
	}
	else {
    	int i, j;
    	
		for (j = 0; j < h; j++) {

			unsigned long *s, *d;
			unsigned long color1, color2;
	
			/* Read src line */
			
			bmp_select(dst);
			s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));
	    
			for (i = w >> 1; i; i--) {
				unsigned long temp1, temp2;
 
				/* Read data, 2 pixels at a time */
				color2 = *s;
				color1 = *d;
				
				if ((color2 & 0xFFFF) == MASK_COLOR_16) {
					color2 &= 0xFFFF0000;
					color2 |= (color1 & 0xFFFF);
				}
				if ((color2 >> 16) == MASK_COLOR_16) {
					color2 &= 0xFFFF;
					color2 |= (color1 << 16);
				}
				
				temp2 = color2 & 0x7E0F81F;
				color2 &= 0xF81F07E0;
				temp1 = color1 & 0x7E0F81F;
				color1 &= 0xF81F07E0;

				/* Multiply the source by the factor */
				color1 = ((((color2 >> 5) - (color1 >> 5)) * fact) + color1) & 0xF81F07E0;
				color2 = ((((temp2 - temp1) * fact) >> 5) + temp1) & 0x7E0F81F;
				
				color1 |= color2;
				
	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
		
			if (w & 1) {
				unsigned long temp1, temp2;

				/* Read 1 pixel */
				color2 = *((unsigned short*)s);
				color1 = *((unsigned short*)d);
				
				if (color2 == MASK_COLOR_16)
					color2 = color1;

				temp2 = color2 & 0xF81F;
				color2 &= 0x07E0;
				temp1 = color1 & 0xF81F;
				color1 &= 0x07E0;

				/* Multiply the source by the factor */
				color1 = ((((color2 >> 5) - (color1 >> 5)) * fact) + color1) & 0x07E0;
				color2 = ((((temp2 - temp1) * fact) >> 5) + temp1) & 0xF81F;
				
				color1 |= color2;
				
	 			/* Write the data */
				bmp_write16((unsigned long)d, color1);
			}
		}
	}
	
	return;
}


static void fblend_trans_15(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {

	if (fact == 16) {
    	int i, j;
    	
		for (j = 0; j < h; j++) {
			unsigned long *s, *d;
			unsigned long color1, color2;
	
			/* Read src line */
			
			bmp_select(dst);
			s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));
		    
			for (i = w >> 1; i; i--) {
 
				/* Read data, 2 pixels at a time */
				color2 = *s;
				color1 = *d;
				
				if ((color2 & 0xFFFF) == MASK_COLOR_15) {
					color2 &= 0xFFFF0000;
					color2 |= (color1 & 0xFFFF);
				}
				if ((color2 >> 16) == MASK_COLOR_15) {
					color2 &= 0xFFFF;
					color2 |= (color1 << 16);
				}

				color1 = (((color1 & 0x7BDE7BDE) >> 1)
						+ ((color2 & 0x7BDE7BDE) >> 1)
						+ (color1 & color2 & 0x04210421));
	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
		
			if (w & 1) {

				/* Read 1 pixel */
				color2 = *((unsigned short*)s);
				color1 = *((unsigned short*)d);
				
				if (color2 == MASK_COLOR_15)
					color2 = color1;

				color1 = (((color1 & 0x7BDE) >> 1)
						+ ((color2 & 0x7BDE) >> 1)
						+ (color1 & color2 & 0x0421));
				
	 			/* Write the data */
				bmp_write16((unsigned long)d, color1);
			}
		}
	}
	else {
    	int i, j;
   	
		for (j = 0; j < h; j++) {
			unsigned long *s, *d;
			unsigned long color1, color2;
	
			/* Read src line */
			
			bmp_select(dst);
			s = (unsigned long*)(src->line[src_y + j] + src_x * sizeof(short));
			d = (unsigned long*)(bmp_write_line(dst, dst_y + j) + dst_x * sizeof(short));
	    
			for (i = w >> 1; i; i--) {
				unsigned long temp1, temp2;
 
				/* Read data, 2 pixels at a time */
				color2 = *s;
				color1 = *d;
				
				if ((color2 & 0xFFFF) == MASK_COLOR_15) {
					color2 &= 0xFFFF0000;
					color2 |= (color1 & 0xFFFF);
				}
				if ((color2 >> 16) == MASK_COLOR_15) {
					color2 &= 0xFFFF;
					color2 |= (color1 << 16);
				}
				
				temp2 = color2 & 0x3E07C1F;
				color2 &= 0x7C1F03E0;
				temp1 = color1 & 0x3E07C1F;
				color1 &= 0x7C1F03E0;

				/* Multiply the source by the factor */
				color1 = ((((color2 >> 5) - (color1 >> 5)) * fact) + color1) & 0x7C1F03E0;
				color2 = ((((temp2 - temp1) * fact) >> 5) + temp1) & 0x3E07C1F;
				
				color1 |= color2;
				
	 			/* Write the data */
					s++;
				bmp_write32((unsigned long)d, color1);
				d++;
			}
		
			if (w & 1) {
				unsigned long temp1, temp2;

				/* Read 1 pixel */
					color2 = *((unsigned short*)s);
				color1 = *((unsigned short*)d);
				
				if (color2 == MASK_COLOR_15)
					color2 = color1;

				temp2 = color2 & 0x7C1F;
				color2 &= 0x03E0;
				temp1 = color1 & 0x7C1F;
				color1 &= 0x03E0;

				/* Multiply the source by the factor */
				color1 = ((((color2 >> 5) - (color1 >> 5)) * fact) + color1) & 0x03E0;
				color2 = ((((temp2 - temp1) * fact) >> 5) + temp1) & 0x7C1F;
				
				color1 |= color2;
				
	 			/* Write the data */
				bmp_write16((unsigned long)d, color1);
			}
		}
	}
	return;
}


static void fblend_trans_32(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h, int fact) {
	if (fact == 128 || fact == 127) {
	   	int i, j;
    	
		for (j = 0; j < h; j++) {

			uint32_t *s, *d;
			uint32_t color1, color2;
		
			/* Read src line */			
			s = ((uint32_t*)(bmp_read_line(src, src_y + j))) + src_x;
			d = ((uint32_t*)(bmp_write_line(dst, dst_y + j))) + dst_x;		    
			for (i = w; i; i--) {
 
				/* Read data, 1 pixel at a time */
				color2 = *s;
				color1 = *d;

				if (color2 == MASK_COLOR_32) {
					s++;
					d++;
					continue;
				}

				color1 =  ((color1 & 0xFEFEFE) >> 1)
						+ ((color2 & 0xFEFEFE) >> 1)
						+ (color1 & color2 & 0x010101);

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
	
			/* Read src line */
			
			s = ((uint32_t *)(bmp_read_line(src, src_y + j))) + src_x;
			d = ((uint32_t *)(bmp_write_line(dst, dst_y + j))) + dst_x;
	    
			for (i = w; i; i--) {
				uint32_t temp1, temp2;
 
				/* Read data, 1 pixel at a time */
				color2 = *s;
				color1 = *d;
				
				if (color2 == MASK_COLOR_32) {
					s++;
					d++;
					continue;
				}
				
				/* Mutiply by the factor */
				temp2 = color1 & 0xFF00FF;
				temp1 = (color2 & 0xFF00FF) - temp2;
				temp1 = (((temp1 * fact) >> 8) + temp2) & 0xFF00FF;
				color1 &= 0xFF00;
				color2 &= 0xFF00;
				temp2 = ((((color2 - color1) * fact) >> 8) + color1) & 0xFF00;

	 			/* Write the data */
				s++;
				bmp_write32((unsigned long)d, temp1 | temp2);
				d++;
			}
		}
	}

	return;
}
