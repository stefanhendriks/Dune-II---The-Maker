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
msb_mask16: .long 0x84108410, 0x84108410
low_mask16: .long 0x7BEF7BEF, 0x7BEF7BEF
rb_mask16:  .long 0xF81FF81F, 0xF81FF81F
g_mask16:   .long 0x07E007E0, 0x07E007E0
source_msb: .long 0, 0

.text

#ifdef FBLEND_SSE

FUNC(fblend_rect_add_sse_16)
	
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

	cmpl $32, %ecx                /* If we don't need to adjust the color, might as well start */
	je rect_add_sse_16_start;
	
	/* Compute source  fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm6, %mm2
	movq %mm6, %mm3
	movq %mm6, %mm4
	pand (rb_mask16), %mm0    /*src |.............| r0b1 | r0b2 | */
	pand (g_mask16),  %mm1    /*src |.............| 0g01 | 0g02 | */

	pmullw %mm0, %mm2         /* Get low words of multiply */

	psrlw $5, %mm0            /* Combine the words to get mm0 * fact */
	pmullw %mm0, %mm3         /* Get high words of multiply */

	psrlw $5, %mm2

	por %mm3, %mm2            /* Place high-word correctly */
	pand (rb_mask16), %mm2
		
	pmullw %mm1, %mm4         /* Get green factor */
	psrlw $5, %mm4
	pand (g_mask16), %mm4
		
	por %mm4, %mm2            /*src |.............|rgb1*f|rgb2*f| */
	movq %mm2, %mm7
	
	/* Now, %mm6 has the factor (ignorable), and %mm7 has source*fact */
	_align_	
	rect_add_sse_16_start:
	
	movq %mm7, %mm6
	pand (low_mask16), %mm7
	pand (msb_mask16), %mm6
	movq %mm6, (source_msb)
	
	movq (low_mask16), %mm5;

	/* %mm7 has the lower bits of the color, and %mm6 has the MSB of the color
	 * We do the computation here to save some cycles later on.
	 */

	RECT_LOOP(add_sse_16, 2,
	
		shrl $3, %ecx;
		jz rect_add_sse_16_rest_of_line;

	_align_;
	rect_add_sse_16_eight_pixels:

		prefetchnta  %es:32(%edi);

	    movq %es:0(%edi), %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		addl $16, %edi;
		RECT_ADD_16_8_MMX(%mm0, %mm1, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm6, %mm4);

		movq %mm0, %es:-16(%edi);
		movq %mm1, %es:-8(%edi);

		decl %ecx;

		jnz rect_add_sse_16_eight_pixels;
	
	_align_;
	rect_add_sse_16_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_add_16_sse_end_of_line;
		
		shrl $1, %ecx;
		jnc rect_add_16_sse_two_pixels;
		
	rect_add_sse_16_one_pixel:
		movzwl (%edi), %eax;
		addl $2, %edi;

		movd %eax, %mm0;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movd %mm0, %eax;

		movw %ax, %es:-2(%edi);
		
	rect_add_16_sse_two_pixels:
		shrl $1, %ecx;
		jnc rect_add_16_sse_four_pixels;

		movd %es:(%edi), %mm0;
		addl $4, %edi;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movd %mm0, %es:-4(%edi);

	rect_add_16_sse_four_pixels:
		shrl $1, %ecx;
		jnc rect_add_16_sse_end_of_line;

		movq %es:(%edi), %mm0;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movq %mm0, %es:(%edi);
	
	_align_;
	rect_add_16_sse_end_of_line:	
	)

	rect_add_sse_16_end:

	emms
	
	restore_context(R_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_rect_add_mmx_16)
	
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

	cmpl $32, %ecx                /* If we don't need to adjust the color, might as well start */
	je rect_add_mmx_16_start;
	
	/* Compute source  fact */
	movq %mm7, %mm0
	movq %mm7, %mm1
	movq %mm6, %mm2
	movq %mm6, %mm3
	movq %mm6, %mm4
	pand (rb_mask16), %mm0    /*src |.............| r0b1 | r0b2 | */
	pand (g_mask16),  %mm1    /*src |.............| 0g01 | 0g02 | */

	pmullw %mm0, %mm2         /* Get low words of multiply */

	psrlw $5, %mm0            /* Combine the words to get mm0 * fact */
	pmullw %mm0, %mm3         /* Get high words of multiply */

	psrlw $5, %mm2

	por %mm3, %mm2            /* Place high-word correctly */
	pand (rb_mask16), %mm2
		
	pmullw %mm1, %mm4         /* Get green factor */
	psrlw $5, %mm4
	pand (g_mask16), %mm4
		
	por %mm4, %mm2            /*src |.............|rgb1*f|rgb2*f| */
	movq %mm2, %mm7
	
	/* Now, %mm6 has the factor (ignorable), and %mm7 has source*fact */
	_align_	
	rect_add_mmx_16_start:
	
	movq %mm7, %mm6
	pand (low_mask16), %mm7
	pand (msb_mask16), %mm6
	movq %mm6, (source_msb)
	
	movq (low_mask16), %mm5;

	/* %mm7 has the lower bits of the color, and %mm6 has the MSB of the color
	 * We do the computation here to save some cycles later on.
	 */

	RECT_LOOP(add_mmx_16, 2,
	
		shrl $3, %ecx;
		jz rect_add_mmx_16_rest_of_line;

	_align_;
	rect_add_mmx_16_eight_pixels:

	    movq %es:0(%edi), %mm0;   /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm1;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		addl $16, %edi;
		RECT_ADD_16_8_MMX(%mm0, %mm1, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm6, %mm4);

		movq %mm0, %es:-16(%edi);
		movq %mm1, %es:-8(%edi);

		decl %ecx;

		jnz rect_add_mmx_16_eight_pixels;
	
	_align_;
	rect_add_mmx_16_rest_of_line:
		movl R_WIDTH, %ecx;
		andl $7, %ecx;
		jz rect_add_16_mmx_end_of_line;
		
		shrl $1, %ecx;
		jnc rect_add_16_mmx_two_pixels;
		
	rect_add_mmx_16_one_pixel:
		movzwl (%edi), %eax;
		addl $2, %edi;

		movd %eax, %mm0;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movd %mm0, %eax;

		movw %ax, %es:-2(%edi);
		
	rect_add_16_mmx_two_pixels:
		shrl $1, %ecx;
		jnc rect_add_16_mmx_four_pixels;

		movd %es:(%edi), %mm0;
		addl $4, %edi;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movd %mm0, %es:-4(%edi);

	rect_add_16_mmx_four_pixels:
		shrl $1, %ecx;
		jnc rect_add_16_mmx_end_of_line;

		movq %es:(%edi), %mm0;

		RECT_ADD_16_4_MMX(%mm0, %mm5, (msb_mask16), %mm7, %mm6, %mm2, %mm3, %mm4);

		movq %mm0, %es:(%edi);
	
	_align_;
	rect_add_16_mmx_end_of_line:	
	)

	rect_add_mmx_16_end:

	emms
	
	restore_context(R_DEST);
	ret

#endif
