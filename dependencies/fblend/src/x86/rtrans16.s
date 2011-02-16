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
r_mask16:     .long 0xF800F800, 0xF800F800
g_mask16:     .long 0x07E007E0, 0x07E007E0
b_mask16:     .long 0x001F001F, 0x001F001F
temp:         .long 0, 0


.text

#ifdef FBLEND_SSE

FUNC(fblend_rect_trans_sse_16)
	
	save_context();

	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | color | color | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	/* Get masks */
	movq (r_mask16), %mm3
	movq (g_mask16), %mm4
	movq (b_mask16), %mm5

	/* Compute source * fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm7, %mm2
	pand %mm4, %mm0     /*src |.............| 0g01 | 0g02 | */
	pand %mm3, %mm1     /*src |.............| r001 | r002 | */
	pand %mm5, %mm2     /*src |.............| 00b1 | 00b2 | */
	psrlw $5, %mm0
	psrlw $5, %mm1

	pmullw %mm6, %mm0
	pmullw %mm6, %mm1
	pmullw %mm6, %mm2

	pand %mm4, %mm0
	psrlw $5, %mm2
	pand %mm3, %mm1
	pand %mm5, %mm2
	
	por %mm1, %mm0
	por %mm2, %mm0
	movq %mm0, %mm7


	/* Invert factor */
	movl R_FACT, %ecx
	negl %ecx
	addl $32, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | 32 - fact | 32 - fact | 32 - fact | 32 - fact | */
	
	RECT_LOOP(trans_sse_16, 2,

		shrl $3, %ecx;
		jz rect_trans_sse_16_rest_of_line;

	_align_;
	rect_trans_sse_16_eight_pixels:

		prefetchnta  %es:32(%edi);

	    movq %es:(%edi),  %mm0;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %edi;
	    movq %mm1, (temp);
	    
	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-16(%edi);

		movq (temp), %mm0;
		
	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-8(%edi);

		decl %ecx;
		jnz rect_trans_sse_16_eight_pixels;

	_align_;
	rect_trans_sse_16_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_trans_sse_16_end_of_line;

		shrl $1, %ecx;
		jnc rect_trans_sse_16_two_pixels;

	rect_trans_sse_16_one_pixel:
		movzwl %es:(%edi), %eax;
	    addl $2, %edi;

		movd %eax, %mm0;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);
		movd %mm0, %eax;
		movw %ax, %es:-2(%edi);
	
	rect_trans_sse_16_two_pixels:
		shrl $1, %ecx;
		jnc rect_trans_sse_16_four_pixels;

		movd %es:(%edi), %mm0;
	    addl $4, %edi;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movd %mm0, %es:-4(%edi);

	_align_;
	rect_trans_sse_16_four_pixels:
		shrl $1, %ecx;
		jnc rect_trans_sse_16_end_of_line;

		movq %es:(%edi), %mm0;
	    addl $8, %edi;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-8(%edi);

	_align_;
	rect_trans_sse_16_end_of_line:	
	)

	rect_trans_sse_16_end:

	emms

	restore_context(R_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_rect_trans_mmx_16)
	
	save_context();

	movl R_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */

	movl R_COLOR, %ecx
	movd %ecx, %mm7

	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | color | color | color | color | */

	movl R_FACT, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | fact | fact | fact | fact | */

	/* Get masks */
	movq (r_mask16), %mm3
	movq (g_mask16), %mm4
	movq (b_mask16), %mm5

	/* Compute source * fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm7, %mm2
	pand %mm4, %mm0     /*src |.............| 0g01 | 0g02 | */
	pand %mm3, %mm1     /*src |.............| r001 | r002 | */
	pand %mm5, %mm2     /*src |.............| 00b1 | 00b2 | */
	psrlw $5, %mm0
	psrlw $5, %mm1

	pmullw %mm6, %mm0
	pmullw %mm6, %mm1
	pmullw %mm6, %mm2

	pand %mm4, %mm0
	psrlw $5, %mm2
	pand %mm3, %mm1
	pand %mm5, %mm2
	
	por %mm1, %mm0
	por %mm2, %mm0
	movq %mm0, %mm7


	/* Invert factor */
	movl R_FACT, %ecx
	negl %ecx
	addl $32, %ecx
	movd %ecx, %mm6

	punpcklwd %mm6, %mm6
	punpcklwd %mm6, %mm6          /* mm6 = | 32 - fact | 32 - fact | 32 - fact | 32 - fact | */
	
	RECT_LOOP(trans_mmx_16, 2,

		shrl $3, %ecx;
		jz rect_trans_mmx_16_rest_of_line;

	_align_;
	rect_trans_mmx_16_eight_pixels:

	    movq %es:(%edi),  %mm0;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %edi;
	    movq %mm1, (temp);

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-16(%edi);

		movq (temp), %mm0;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-8(%edi);

		decl %ecx;
		jnz rect_trans_mmx_16_eight_pixels;
	
	_align_;
	rect_trans_mmx_16_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_trans_mmx_16_end_of_line;

		shrl $1, %ecx;
		jnc rect_trans_mmx_16_two_pixels;

	rect_trans_mmx_16_one_pixel:
		movzwl %es:(%edi), %eax;
	    addl $2, %edi;

		movd %eax, %mm0;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movd %mm0, %eax;
		movw %ax, %es:-2(%edi);
	
	rect_trans_mmx_16_two_pixels:
		shrl $1, %ecx;
		jnc rect_trans_mmx_16_four_pixels;

		movd %es:(%edi), %mm0;
	    addl $4, %edi;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movd %mm0, %es:-4(%edi);

	_align_;
	rect_trans_mmx_16_four_pixels:
		shrl $1, %ecx;
		jnc rect_trans_mmx_16_end_of_line;

		movq %es:(%edi), %mm0;
	    addl $8, %edi;

	    RECT_TRANS_16_4_MMX(%mm0, %mm3, %mm4, %mm5, %mm6, %mm7, %mm1, %mm2);

		movq %mm0, %es:-8(%edi);

	_align_;
	rect_trans_mmx_16_end_of_line:	
	)

	rect_trans_mmx_16_end:

	emms

	restore_context(R_DEST);
	ret

#endif
