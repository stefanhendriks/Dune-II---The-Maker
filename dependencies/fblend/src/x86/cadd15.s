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
#include "blit.inc"
#include "cadd.inc"


.data
msb_mask15: .long 0x42104210, 0x42104210
low_mask15: .long 0x3DEF3DEF, 0x3DEF3DEF
rb_mask15:  .long 0x7C1F7C1F, 0x7C1F7C1F
r_mask16:   .long 0x7C007C00, 0x7C007C00
g_mask15:   .long 0x03E003E0, 0x03E003E0
b_mask16:   .long 0x001F001F, 0x001F001F
fact:       .long 0, 0


.text

#ifdef FBLEND_SSE

FUNC(fblend_add_sse_15)

	save_context();

	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7

	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $32, %ecx                /* Special case: factor == 32 */
	je draw_color_add_sse_15_full


	BLIT_LOOP(draw_color_add_sse_15, 2, 2,

		shrl $3, %ecx;
		jz color_add_sse_15_rest_of_line;

	_align_;
	color_add_sse_15_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi),  %mm0;       /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask15), %mm6;
		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		addl $16, %edi;
		addl $16, %esi;

		MULTIPLY_BY_FACTOR_16_8_MMX_1(%mm0, %mm1, %mm7, (rb_mask15), (g_mask15), %mm4, %mm5, %mm6, %mm2, %mm3);

	    movq %es:-16(%edi), %mm2;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi),  %mm3;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		MULTIPLY_BY_FACTOR_16_8_MMX_2(%mm0, %mm1, %mm7, (rb_mask15), (g_mask15), %mm4, %mm5, %mm6);

		movq (msb_mask15), %mm6;
		ADD_SATURATE_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm6, %mm5, %mm4, %mm7);
		movq (fact), %mm7;

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_sse_15_loop;

	color_add_sse_15_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_add_sse_15_end;

		shrl $1, %ecx;
		movq (low_mask15), %mm6;
		movq (msb_mask15), %mm5;
		movq (rb_mask15), %mm4;
		jnc color_add_sse_15_two_pixels;

	color_add_sse_15_one_pixel:

		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movzwl %es:(%edi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm1;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movd %mm1, %eax;

		addl $2, %edi;
		addl $2, %esi;

		movw %ax, %es:-2(%edi);

	color_add_sse_15_two_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_15_four_pixels;

		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movd %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $4, %esi;
		addl $4, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movd %mm1, %es:-4(%edi);  /* Write the results */

	_align_;
	color_add_sse_15_four_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_15_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movq %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $8, %esi;
		addl $8, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movq %mm1, %es:-8(%edi);  /* Write the results */

		_align_;
		color_add_sse_15_end:
	)

	jmp draw_color_add_sse_15_end


	/***********************
	 * Start of special case
	 */
	draw_color_add_sse_15_full:

    movq (msb_mask15), %mm6;

	BLIT_LOOP(draw_color_add_sse_15_full, 2, 2,

		shrl $3, %ecx;
		jz color_add_sse_15_full_rest_of_line;

	_align_;
	color_add_sse_15_full_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask15), %mm2, %mm3);

	    movq %es:(%edi), %mm2;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %esi;
	    addl $16, %edi;

		ADD_SATURATE_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm6, %mm5, %mm4, %mm7);

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_sse_15_full_loop;

	color_add_sse_15_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_add_sse_15_full_end;

		shrl $1, %ecx;
		movq (rb_mask15), %mm4;
		jnc color_add_sse_15_full_two_pixels;

	color_add_sse_15_full_one_pixel:
		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		movzwl %es:(%edi), %eax;
		movd %eax, %mm1;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movd %mm1, %eax;
		addl $2, %esi;
		addl $2, %edi;	

		movw %ax, %es:-2(%edi);     /* Write result */

	color_add_sse_15_full_two_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_15_full_four_pixels;

		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movd %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movd %mm1, %es:-4(%edi);  /* Write the results */

	_align_;
	color_add_sse_15_full_four_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_15_full_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movq %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movq %mm1, %es:-8(%edi);  /* Write the results */

		_align_;
		color_add_sse_15_full_end:
	)

	draw_color_add_sse_15_end:

	emms

	restore_context(B_DEST);
	ret

#endif

#ifdef FBLEND_MMX

FUNC(fblend_add_mmx_15)

	save_context();

	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7

	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $32, %ecx                /* Special case: factor == 32 */
	je draw_color_add_mmx_15_full


	BLIT_LOOP(draw_color_add_mmx_15, 2, 2,

		shrl $3, %ecx;
		jz color_add_mmx_15_rest_of_line;

	_align_;
	color_add_mmx_15_loop:

		movq (%esi),  %mm0;       /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask15), %mm6;
		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		addl $16, %edi;
		addl $16, %esi;

		MULTIPLY_BY_FACTOR_16_8_MMX_1(%mm0, %mm1, %mm7, (rb_mask15), (g_mask15), %mm4, %mm5, %mm6, %mm2, %mm3);

	    movq %es:-16(%edi), %mm2;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi),  %mm3;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		MULTIPLY_BY_FACTOR_16_8_MMX_2(%mm0, %mm1, %mm7, (rb_mask15), (g_mask15), %mm4, %mm5, %mm6);

		movq (msb_mask15), %mm6;
		ADD_SATURATE_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm6, %mm5, %mm4, %mm7);
		movq (fact), %mm7;

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_mmx_15_loop;

	color_add_mmx_15_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_add_mmx_15_end;

		shrl $1, %ecx;
		movq (msb_mask15), %mm5;
		movq (rb_mask15), %mm4;
		jnc color_add_mmx_15_two_pixels;

	color_add_mmx_15_one_pixel:

		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movzwl %es:(%edi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm1;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movd %mm1, %eax;

		addl $2, %edi;
		addl $2, %esi;

		movw %ax, %es:-2(%edi);

	color_add_mmx_15_two_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_15_four_pixels;

		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movd %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $4, %esi;
		addl $4, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movd %mm1, %es:-4(%edi);  /* Write the results */

	_align_;
	color_add_mmx_15_four_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_15_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask15), %mm2, %mm3);

		movq %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $8, %esi;
		addl $8, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm5, %mm2, %mm3);

		movq %mm1, %es:-8(%edi);  /* Write the results */

		_align_;
		color_add_mmx_15_end:
	)

	jmp draw_color_add_mmx_15_end


	/***********************
	 * Start of special case
	 */
	draw_color_add_mmx_15_full:

    movq (msb_mask15), %mm6;

	BLIT_LOOP(draw_color_add_mmx_15_full, 2, 2,

		shrl $3, %ecx;
		jz color_add_mmx_15_full_rest_of_line;

	_align_;
	color_add_mmx_15_full_loop:

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask15), %mm2, %mm3);

	    movq %es:(%edi), %mm2;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %esi;
	    addl $16, %edi;

		ADD_SATURATE_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm6, %mm5, %mm4, %mm7);

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_mmx_15_full_loop;

	color_add_mmx_15_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_add_mmx_15_full_end;

		shrl $1, %ecx;
		movq (rb_mask15), %mm4;
		jnc color_add_mmx_15_full_two_pixels;

	color_add_mmx_15_full_one_pixel:
		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		movzwl %es:(%edi), %eax;
		movd %eax, %mm1;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movd %mm1, %eax;
		addl $2, %esi;
		addl $2, %edi;	

		movw %ax, %es:-2(%edi);     /* Write result */

	color_add_mmx_15_full_two_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_15_full_four_pixels;

		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movd %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movd %mm1, %es:-4(%edi);  /* Write the results */

	_align_;
	color_add_mmx_15_full_four_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_15_full_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movq %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		movq %mm1, %es:-8(%edi);  /* Write the results */

		_align_;
		color_add_mmx_15_full_end:
	)

	draw_color_add_mmx_15_end:

	emms

	restore_context(B_DEST);
	ret

#endif
