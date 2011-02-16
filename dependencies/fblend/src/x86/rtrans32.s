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

FUNC(fblend_rect_trans_sse_32)
	
	save_context();
   
	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpckldq %mm7, %mm7          /* mm7 = | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	/* Compute source * fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	pand (g_mask32),  %mm1    /*src |.............| 0g01 | 0g02 | */
	pand (rb_mask32), %mm0    /*src |.............| r0b1 | r0b2 | */
	psrlw $8, %mm1
	
	pmullw %mm6, %mm0
	pmullw %mm6, %mm1

	pand (g_mask32), %mm1
	psrlw $8, %mm0
	
	por %mm1, %mm0
	movq %mm0, %mm7;


	/* Get masks */
	movq (rb_mask32), %mm3;
	movq (g_mask32),  %mm4;

	/* Invert factor */
	movl R_FACT, %ecx
	negl %ecx;
	addl $256, %ecx;
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | 256 - fact | 256 - fact | 256 - fact | 256 - fact | */

	RECT_LOOP(trans_sse_32, 4,

		shrl $2, %ecx;
		jz rect_trans_sse_32_rest_of_line;

	_align_;
	rect_trans_sse_32_four_pixels:

		prefetchnta  %es:32(%edi);

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi),  %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %edi;

	    RECT_TRANS_32_8_MMX(%mm0, %mm1, %mm3, %mm4, %mm6, %mm7, %mm2, %mm5);

		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

		decl %ecx;
		jnz rect_trans_sse_32_four_pixels;
	
	_align_;
	rect_trans_sse_32_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $3, %ecx;
		jz rect_trans_32_sse_end_of_line;

		shrl $1, %ecx;
		jnc rect_trans_32_sse_two_pixels;

	rect_trans_sse_32_one_pixel:

	    movd %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $4, %edi;

	    RECT_TRANS_32_4_MMX(%mm0, %mm3, %mm4, %mm6, %mm7, %mm2);

		movd %mm0, %es:-4(%edi);  /* Write back */

	rect_trans_32_sse_two_pixels:
		shrl $1, %ecx;
		jnc rect_trans_32_sse_end_of_line;

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    RECT_TRANS_32_4_MMX(%mm0, %mm3, %mm4, %mm6, %mm7, %mm2);

		movq %mm0, %es:0(%edi);  /* Write back */

	_align_;
	rect_trans_32_sse_end_of_line:	
	)

	rect_trans_sse_32_end:

	emms

	restore_context(R_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_rect_trans_mmx_32)

	save_context();
   
	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpckldq %mm7, %mm7          /* mm7 = | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	/* Compute source * fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	pand (g_mask32),  %mm1    /*src |.............| 0g01 | 0g02 | */
	pand (rb_mask32), %mm0    /*src |.............| r0b1 | r0b2 | */
	psrlw $8, %mm1
	
	pmullw %mm6, %mm0
	pmullw %mm6, %mm1

	pand (g_mask32), %mm1
	psrlw $8, %mm0
	
	por %mm1, %mm0
	movq %mm0, %mm7;


	/* Get masks */
	movq (rb_mask32), %mm3;
	movq (g_mask32),  %mm4;

	/* Invert factor */
	movl R_FACT, %ecx
	negl %ecx;
	addl $256, %ecx;
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | 256 - fact | 256 - fact | 256 - fact | 256 - fact | */

	RECT_LOOP(trans_mmx_32, 4,

		shrl $2, %ecx;
		jz rect_trans_mmx_32_rest_of_line;

	_align_;
	rect_trans_mmx_32_four_pixels:

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi),  %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %edi;

	    RECT_TRANS_32_8_MMX(%mm0, %mm1, %mm3, %mm4, %mm6, %mm7, %mm2, %mm5);

		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

		decl %ecx;
		jnz rect_trans_mmx_32_four_pixels;
	
	_align_;
	rect_trans_mmx_32_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $3, %ecx;
		jz rect_trans_32_mmx_end_of_line;

		shrl $1, %ecx;
		jnc rect_trans_32_mmx_two_pixels;

	rect_trans_mmx_32_one_pixel:

	    movd %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $4, %edi;

	    RECT_TRANS_32_4_MMX(%mm0, %mm3, %mm4, %mm6, %mm7, %mm2);

		movd %mm0, %es:-4(%edi);  /* Write back */

	rect_trans_32_mmx_two_pixels:
		shrl $1, %ecx;
		jnc rect_trans_32_mmx_end_of_line;

	    movq %es:0(%edi),  %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    RECT_TRANS_32_4_MMX(%mm0, %mm3, %mm4, %mm6, %mm7, %mm2);

		movq %mm0, %es:0(%edi);  /* Write back */

	_align_;
	rect_trans_32_mmx_end_of_line:	
	)

	rect_trans_mmx_32_end:

	emms

	restore_context(R_DEST);
	ret


#endif
