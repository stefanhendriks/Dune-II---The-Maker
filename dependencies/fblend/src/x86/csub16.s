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
.balign 32, 0x90
msb_mask16: .long 0x84108410, 0x84108410
low_mask16: .long 0x7BEF7BEF, 0x7BEF7BEF
rb_mask16:  .long 0xF81FF81F, 0xF81FF81F
r_mask16:   .long 0xF800F800, 0xF800F800
g_mask16:   .long 0x07E007E0, 0x07E007E0
b_mask16:   .long 0x001F001F, 0x001F001F
fact:       .long 0, 0

.text

#ifdef FBLEND_SSE

FUNC(fblend_sub_sse_16)

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
	je draw_color_sub_sse_16_full
	

	BLIT_LOOP(draw_color_sub_sse_16, 2, 2,

		movq (msb_mask16), %mm6;

		shrl $3, %ecx;
		jz color_sub_sse_16_rest_of_line;

	_align_;
	color_sub_sse_16_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi),  %mm0;       /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		addl $16, %edi;
		addl $16, %esi;

		MULTIPLY_BY_FACTOR_16_8_MMX_1(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6, %mm2, %mm3);

	    movq %es:-16(%edi), %mm2;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi),  %mm3;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		MULTIPLY_BY_FACTOR_16_8_MMX_2(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6);

		movq (msb_mask16), %mm6;
		SUB_SATURATE_16_4_MMX(%mm2, %mm0, %mm6, %mm5, %mm4);
		SUB_SATURATE_16_4_MMX(%mm3, %mm1, %mm6, %mm5, %mm4);
		movq (rb_mask16), %mm6;

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz color_sub_sse_16_loop;

	color_sub_sse_16_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_sub_sse_16_end;

		shrl $1, %ecx;
		movq (msb_mask16), %mm5;
		jnc color_sub_sse_16_two_pixels;
	
	color_sub_sse_16_one_pixel:

		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movzwl %es:(%edi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm1;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm5, %mm2, %mm3);

		movd %mm0, %eax;

		addl $2, %edi;
		addl $2, %esi;

		movw %ax, %es:-2(%edi);

	color_sub_sse_16_two_pixels:
		shrl $1, %ecx;
		jnc color_sub_sse_16_four_pixels;

		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movd %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $4, %esi;
		addl $4, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm5, %mm2, %mm3);

		movd %mm0, %es:-4(%edi);  /* Write the results */

	_align_;
	color_sub_sse_16_four_pixels:
		shrl $1, %ecx;
		jnc color_sub_sse_16_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movq %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $8, %esi;
		addl $8, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm5, %mm2, %mm3);

		movq %mm0, %es:-8(%edi);  /* Write the results */

		_align_;
		color_sub_sse_16_end:
	)
	
	jmp draw_color_sub_sse_16_end
	
	
	/***********************
	 * Start of special case
	 */
	draw_color_sub_sse_16_full:

    movq (msb_mask16), %mm6;
	
	BLIT_LOOP(draw_color_sub_sse_16_full, 2, 2,
	
		shrl $3, %ecx;
		jz color_sub_sse_16_full_rest_of_line;

	_align_;
	color_sub_sse_16_full_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3);

	    movq %es:(%edi), %mm2;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %esi;
	    addl $16, %edi;

		SUB_SATURATE_16_8_MMX(%mm2, %mm3, %mm0, %mm1, %mm6, %mm5, %mm4, %mm7);

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz color_sub_sse_16_full_loop;

	color_sub_sse_16_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_sub_sse_16_full_end;

		shrl $1, %ecx;
		movq (rb_mask16), %mm4;
		jnc color_sub_sse_16_full_two_pixels;

	color_sub_sse_16_full_one_pixel:
		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		movzwl %es:(%edi), %eax;
		movd %eax, %mm1;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm6, %mm2, %mm3);

		movd %mm0, %eax;
		addl $2, %esi;
		addl $2, %edi;		

		movw %ax, %es:-2(%edi);     /* Write result */

	color_sub_sse_16_full_two_pixels:
		shrl $1, %ecx;
		jnc color_sub_sse_16_full_four_pixels;
		
		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movd %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm6, %mm2, %mm3);
		
		movd %mm0, %es:-4(%edi);  /* Write the results */

	_align_;
	color_sub_sse_16_full_four_pixels:
		shrl $1, %ecx;
		jnc color_sub_sse_16_full_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movq %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm6, %mm2, %mm3);

		movq %mm0, %es:-8(%edi);  /* Write the results */

		_align_;
		color_sub_sse_16_full_end:
	)

	draw_color_sub_sse_16_end:
	
	emms

	restore_context(B_DEST);
	ret

#endif

#ifdef FBLEND_MMX

FUNC(fblend_sub_mmx_16)

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
	je draw_color_sub_mmx_16_full
	

	BLIT_LOOP(draw_color_sub_mmx_16, 2, 2,
	
		movq (rb_mask16), %mm6;

		shrl $3, %ecx;
		jz color_sub_mmx_16_rest_of_line;

	_align_;
	color_sub_mmx_16_loop:

		movq (%esi),  %mm0;       /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		addl $16, %edi;
		addl $16, %esi;

		MULTIPLY_BY_FACTOR_16_8_MMX_1(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6, %mm2, %mm3);

	    movq %es:-16(%edi), %mm2;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi),  %mm3;  /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		MULTIPLY_BY_FACTOR_16_8_MMX_2(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6);

		movq (msb_mask16), %mm6;
		SUB_SATURATE_16_4_MMX(%mm2, %mm0, %mm6, %mm5, %mm4);
		SUB_SATURATE_16_4_MMX(%mm3, %mm1, %mm6, %mm5, %mm4);
		movq (rb_mask16), %mm6;

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz color_sub_mmx_16_loop;

	color_sub_mmx_16_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_sub_mmx_16_end;

		shrl $1, %ecx;
		movq (msb_mask16), %mm4;
		jnc color_sub_mmx_16_two_pixels;
	
	color_sub_mmx_16_one_pixel:

		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm6, (g_mask16), %mm2, %mm3);

		movzwl %es:(%edi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm1;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm4, %mm2, %mm3);

		movd %mm0, %eax;

		addl $2, %edi;
		addl $2, %esi;

		movw %ax, %es:-2(%edi);

	color_sub_mmx_16_two_pixels:
		shrl $1, %ecx;
		jnc color_sub_mmx_16_four_pixels;

		movd %es:(%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm6, (g_mask16), %mm2, %mm3);
		movd (%edi), %mm1;       /*src |......| rgb1 | */

		addl $4, %esi;
		addl $4, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm4, %mm2, %mm3);

		movd %mm0, %es:-4(%edi);  /* Write the results */

	_align_;
	color_sub_mmx_16_four_pixels:
		shrl $1, %ecx;
		jnc color_sub_mmx_16_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm6, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm6, (g_mask16), %mm2, %mm3);

		movq %es:(%edi), %mm1;       /*src |......| rgb1 | */

		addl $8, %esi;
		addl $8, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm4, %mm2, %mm3);
		
		movq %mm0, %es:-8(%edi);  /* Write the results */

		_align_;
		color_sub_mmx_16_end:
	)
	
	jmp draw_color_sub_mmx_16_end
	
	
	/***********************
	 * Start of special case
	 */
	draw_color_sub_mmx_16_full:

    movq (msb_mask16), %mm7;
	
	BLIT_LOOP(draw_color_sub_mmx_16_full, 2, 2,
	
		shrl $3, %ecx;
		jz color_sub_mmx_16_full_rest_of_line;

	_align_;
	color_sub_mmx_16_full_loop:

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3);

	    movq %es:(%edi), %mm2;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    addl $16, %esi;
	    addl $16, %edi;

		SUB_SATURATE_16_8_MMX(%mm2, %mm3, %mm0, %mm1, %mm7, %mm5, %mm4, %mm6);

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz color_sub_mmx_16_full_loop;

	color_sub_mmx_16_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $7, %ecx;
		jz color_sub_mmx_16_full_end;

		shrl $1, %ecx;
		movq (rb_mask16), %mm4;
		jnc color_sub_mmx_16_full_two_pixels;

	color_sub_mmx_16_full_one_pixel:
		movzwl (%esi), %eax;       /*src |......| rgb1 | */
		movd %eax, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		movzwl %es:(%edi), %eax;
		movd %eax, %mm1;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm7, %mm2, %mm3);

		movd %mm0, %eax;
		addl $2, %esi;
		addl $2, %edi;		

		movw %ax, %es:-2(%edi);     /* Write result */

	color_sub_mmx_16_full_two_pixels:
		shrl $1, %ecx;
		jnc color_sub_mmx_16_full_four_pixels;
		
		movd (%esi), %mm0;        /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movd %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm7, %mm2, %mm3);
		
		movd %mm0, %es:-4(%edi);  /* Write the results */

	_align_;
	color_sub_mmx_16_full_four_pixels:
		shrl $1, %ecx;
		jnc color_sub_mmx_16_full_end;

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

	    movq %es:(%edi), %mm1;    /*dst |.............| rgb1 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

		SUB_SATURATE_16_4_MMX(%mm1, %mm0, %mm7, %mm2, %mm3);

		movq %mm0, %es:-8(%edi);  /* Write the results */

		_align_;
		color_sub_mmx_16_full_end:
	)

	draw_color_sub_mmx_16_end:
	
	emms

	restore_context(B_DEST);
	ret

#endif
