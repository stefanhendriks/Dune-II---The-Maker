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
_fblend_static_buffer: .long  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


.text


FUNC(fblend_add_sse_16)

	save_context();
	
	movl B_DEST, %edx
	movl B_SOURCE, %ebx

	load_bitmap_segment(%edx, %es);
	load_bitmap_segment(%ebx, %fs);

	movl B_FACT, %ecx
	movd %ecx, %mm7
	
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

/*
 * When the user code is entered, the following resgiters hold:
 *  %ds:%eax - Destination buffer for blend data
 *  %fs:%esi - Source bitmap pointer
 *  %es:%edi - Destination bitmap pointer
 *      %ecx - Number of pixels to blend
 *      %edx - Source bitmap pointer
 */
	BITMAP_BLEND_LOOP(draw_color_add_sse_16, 2, 2,
	
		pushl %ecx;

		shrl $3, %ecx;
		jz color_add_sse_16_rest_of_line;

	_align_;
	color_add_sse_16_loop:

		prefetchnta  %fs:32(%esi);
		prefetchnta  %es:32(%edi);

		movq %fs:(%esi),  %mm0;    /*src  | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq %fs:8(%esi), %mm1;    /*src2 | rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask16), %mm6;
		ZERO_ON_EQUAL_16_8_MMX(%mm0, %mm1, %mm6, %mm2, %mm3);

		addl $16, %edi;
		addl $16, %esi;

		MULTIPLY_BY_FACTOR_16_8_MMX_1(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6, %mm2, %mm3);

	    movq %es:-16(%edi), %mm2;  /*dst  | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi),  %mm3;  /*dst2 | rgb1 | rgb2 | rgb3 | rgb4 | */

		MULTIPLY_BY_FACTOR_16_8_MMX_2(%mm0, %mm1, %mm7, (rb_mask16), (g_mask16), %mm4, %mm5, %mm6);

		movq (low_mask16), %mm6;
		addl $16, %eax;
		ADD_SATURATE_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm6, (msb_mask16), %mm5, %mm4, %mm7);
		movq (fact), %mm7;

		movq %mm2, -16(%eax);      /* Write results */
		movq %mm3, -8(%eax);
		decl %ecx;

		jnz color_add_sse_16_loop;

	_align_;
	color_add_sse_16_rest_of_line:
		popl %ecx;
		andl $7, %ecx;               /* Check if there are pixels left */
		jz color_add_sse_16_end_blend;

		testl $4, %ecx;
		movq (low_mask16), %mm6;
		movq (msb_mask16), %mm5;
		movq (rb_mask16), %mm4;
		jz color_add_sse_16_two_pixels;

	_align_;
	color_add_sse_16_four_pixels:

		movq %fs:(%esi), %mm0;     /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movq %es:(%edi), %mm1;     /*src |......| rgb1 | */

		addl $8, %esi;
		addl $8, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm5, %mm2, %mm3);
		addl $8, %eax;

		movq %mm1, -8(%eax);        /* Write the results */

	_align_;
	color_add_sse_16_two_pixels:
		testl $2, %ecx;
		jz color_add_sse_16_one_pixel;

		movd %fs:(%esi), %mm0;      /*src |.............| rgb1 | rgb2 | */

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movd %es:(%edi), %mm1;      /*src |......| rgb1 | */

		addl $4, %esi;
		addl $4, %edi;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm5, %mm2, %mm3);
		addl $4, %eax;

		movd %mm1, -4(%eax);        /* Write the results */
	
	_align_;
	color_add_sse_16_one_pixel:
		testl $1, %ecx;
		jz color_add_sse_16_end_blend;

		movzwl %fs:(%esi), %edx;    /*src |......| rgb1 | */
		movd %edx, %mm0;

		ZERO_ON_EQUAL_16_4_MMX(%mm0, %mm4, %mm2);

		MULTIPLY_BY_FACTOR_16_4_MMX(%mm0, %mm7, %mm4, (g_mask16), %mm2, %mm3);

		movzwl %es:(%edi), %edx;    /*src |......| rgb1 | */
		movd %edx, %mm1;

		ADD_SATURATE_16_4_MMX(%mm0, %mm1, %mm6, %mm5, %mm2, %mm3);

		movd %mm1, %edx;

		addl $2, %edi;
		addl $2, %esi;
		addl $2, %eax;

		movw %dx, -2(%eax);

	_align_;
	color_add_sse_16_end_blend:
	)

	emms

	restore_context(B_DEST);
	ret

