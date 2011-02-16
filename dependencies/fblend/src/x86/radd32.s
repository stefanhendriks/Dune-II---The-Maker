/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 */
 
#include "mmx.h"
#include "sse.h"

#include "asmdefs.inc"
#include "fblend.inc"
#include "rect.inc"

.data
.balign 32, 0x90
g_mask32:  .long 0x0000FF00, 0x0000FF00
rb_mask32: .long 0x00FF00FF, 0x00FF00FF

.text

#ifdef FBLEND_SSE

FUNC(fblend_rect_add_sse_32)
	
	save_context();
   
	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpckldq %mm7, %mm7          /* mm7 = | color | color | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	cmpl $256, %ecx               /* If we don't need to adjust the color, might as well start */
	je rect_add_sse_32_start;

	/* Compute source  fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm6, %mm2
	movq %mm6, %mm3
	pand (g_mask32),  %mm1    /*src |.............| 0g01 | 0g02 | */
	pand (rb_mask32), %mm0    /*src |.............| r0b1 | r0b2 | */
	psrlw $8, %mm1
	
	pmullw %mm0, %mm2
	pmullw %mm1, %mm3

	pand (g_mask32), %mm3
	psrlw $8, %mm2
	
	por %mm2, %mm3
	movq %mm3, %mm7;
	
	rect_add_sse_32_start:

	RECT_LOOP(add_sse_32, 4,

		shrl $3, %ecx;
		jz rect_add_sse_32_rest_of_line;

	_align_;
	rect_add_sse_32_eight_pixels:

		prefetchnta  %es:32(%edi);

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi),  %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:16(%edi), %mm2;   /*dst3| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:24(%edi), %mm3;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $32, %edi;

		paddusb %mm7, %mm0;
		paddusb %mm7, %mm1;
		paddusb %mm7, %mm2;
		paddusb %mm7, %mm3;

		movq %mm0, %es:-32(%edi);
		movq %mm1, %es:-24(%edi);
		movq %mm2, %es:-16(%edi);
		movq %mm3, %es:-8(%edi);

		decl %ecx;
		jnz rect_add_sse_32_eight_pixels;
	
	_align_;
	rect_add_sse_32_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_add_32_sse_end_of_line;

		shrl $1, %ecx;
		jnc rect_add_32_sse_two_pixels;

	rect_add_sse_32_one_pixel:

	    movd %es:(%edi), %mm0;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    addl $4, %edi;
		paddusb %mm7, %mm0;
		movd %mm0, %es:-4(%edi);
		
	rect_add_32_sse_two_pixels:
		shrl $1, %ecx;
		jnc rect_add_32_sse_four_pixels;

	    movq %es:(%edi), %mm0;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    addl $8, %edi;
		paddusb %mm7, %mm0;
		movq %mm0, %es:-8(%edi);

	rect_add_32_sse_four_pixels:
		shrl $1, %ecx;
		jnc rect_add_32_sse_end_of_line;

	    movq %es:(%edi), %mm0;   /*dst1| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
		paddusb %mm7, %mm0;
		paddusb %mm7, %mm1;
		movq %mm0, %es:0(%edi);
		movq %mm1, %es:8(%edi);

	_align_;
	rect_add_32_sse_end_of_line:	
	)

	rect_add_sse_32_end:

	emms
	
	restore_context(R_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_rect_add_mmx_32)
	
	save_context();
   
	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpckldq %mm7, %mm7          /* mm7 = | color | color | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	cmpl $256, %ecx               /* If we don't need to adjust the color, might as well start */
	je rect_add_mmx_32_start;

	/* Compute source  fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm6, %mm2
	movq %mm6, %mm3
	pand (g_mask32),  %mm1    /*src |.............| 0g01 | 0g02 | */
	pand (rb_mask32), %mm0    /*src |.............| r0b1 | r0b2 | */
	psrlw $8, %mm1
	
	pmullw %mm0, %mm2
	pmullw %mm1, %mm3

	pand (g_mask32), %mm3
	psrlw $8, %mm2
	
	por %mm2, %mm3
	movq %mm3, %mm7;

	rect_add_mmx_32_start:

	RECT_LOOP(add_mmx_32, 4,

		shrl $3, %ecx;
		jz rect_add_mmx_32_rest_of_line;

	_align_;
	rect_add_mmx_32_eight_pixels:

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi),  %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:16(%edi), %mm2;   /*dst3| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:24(%edi), %mm3;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $32, %edi;

		paddusb %mm7, %mm0;
		paddusb %mm7, %mm1;
		paddusb %mm7, %mm2;
		paddusb %mm7, %mm3;

		movq %mm0, %es:-32(%edi);
		movq %mm1, %es:-24(%edi);
		movq %mm2, %es:-16(%edi);
		movq %mm3, %es:-8(%edi);

		decl %ecx;
		jnz rect_add_mmx_32_eight_pixels;
	
	_align_;
	rect_add_mmx_32_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_add_32_mmx_end_of_line;

		shrl $1, %ecx;
		jnc rect_add_32_mmx_two_pixels;

	rect_add_mmx_32_one_pixel:

	    movd %es:(%edi), %mm0;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    addl $4, %edi;
		paddusb %mm7, %mm0;
		movd %mm0, %es:-4(%edi);
		
	rect_add_32_mmx_two_pixels:
		shrl $1, %ecx;
		jnc rect_add_32_mmx_four_pixels;

	    movq %es:(%edi), %mm0;   /*dst4| rgb1 | rgb2 | rgb3 | rgb4 | */
	    addl $8, %edi;
		paddusb %mm7, %mm0;
		movq %mm0, %es:-8(%edi);

	rect_add_32_mmx_four_pixels:
		shrl $1, %ecx;
		jnc rect_add_32_mmx_end_of_line;

	    movq %es:(%edi), %mm0;   /*dst1| rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
		paddusb %mm7, %mm0;
		paddusb %mm7, %mm1;
		movq %mm0, %es:0(%edi);
		movq %mm1, %es:8(%edi);

	_align_;
	rect_add_32_mmx_end_of_line:	
	)

	rect_add_mmx_32_end:

	emms
	
	restore_context(R_DEST);
	ret

#endif
