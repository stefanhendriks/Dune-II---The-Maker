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
#include "trans.inc"

.data
.balign 32, 0x90
r_mask16:     .long 0xF800F800, 0xF800F800
g_mask16:     .long 0x07E007E0, 0x07E007E0
b_mask16:     .long 0x001F001F, 0x001F001F
rb_mask16:    .long 0xF81FF81F, 0xF81FF81F
high_mask16:  .long 0xF7DEF7DE, 0xF7DEF7DE
low_mask16:   .long 0x08210821, 0x08210821
fact:         .long 0, 0
temp:         .long 0, 0

.text

#ifdef FBLEND_SSE

FUNC(fblend_trans_sse_16)

	save_context();

	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $16, %ecx
	je draw_trans_sse_16_half


	BLIT_LOOP(draw_trans_sse_16, 2, 2,

		shrl $3, %ecx;
		jz trans_sse_16_rest_of_line;

	_align_;
	trans_sse_16_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    SET_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3, %mm4, %mm5, %mm6, %mm7);
	    
	    movq (fact), %mm7;
	    movq %mm3, (temp);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movq (temp), %mm3;
	    addl $16, %esi;
	    addl $16, %edi;

		TRANSLUCENT_BLEND_16_4_MMX(%mm1, %mm3, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm2, %mm4, %mm5, %mm6)

		movq %mm0, %es:-16(%edi);  /* Write results back */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz trans_sse_16_loop;
		
	trans_sse_16_rest_of_line:
		movl B_WIDTH, %ecx;
		andl $7, %ecx;
		jz trans_sse_16_end;

	trans_sse_16_one_pixel:		
		shrl $1, %ecx;
		jnc trans_sse_16_two_pixels;

		movzwl (%esi), %eax;
		movzwl %es:(%edi), %edx;
		
		addl $2, %esi;
		addl $2, %edi;

		movd %eax, %mm0;
		movd %edx, %mm2;
		
	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movd %mm0, %eax;

		movw %ax,  %es:-2(%edi);  /* Write results back */
		
	trans_sse_16_two_pixels:
		shrl $1, %ecx;
		jnc trans_sse_16_four_pixels;

		movd (%esi), %mm0;
		movd %es:(%edi), %mm2;
		
		addl $4, %esi;
		addl $4, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movd %mm0,  %es:-4(%edi);  /* Write results back */

	_align_;
	trans_sse_16_four_pixels:
		
		shrl $1, %ecx;
		jnc trans_sse_16_end;

		movq (%esi), %mm0;
		movq %es:(%edi), %mm2;
		
		addl $8, %esi;
		addl $8, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movq %mm0,  %es:-8(%edi);  /* Write results back */
	
	_align_;
	trans_sse_16_end:
	
	)

	jmp draw_trans_16_sse_end

	/****************
	 * Special code 
	 */
	_align_
	draw_trans_sse_16_half:

	movq (high_mask16), %mm7;

	BLIT_LOOP(draw_trans_sse_16_half, 2, 2,

		shrl $3, %ecx;
		jz trans_sse_16_half_rest_of_line;

	_align_;
	trans_sse_16_half_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		addl $16, %esi;
		addl $16, %edi;

	    SET_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3, %mm4, %mm5, %mm6, %mm7);

		movq (high_mask16), %mm7;
	    
		TRANSLUCENT_HALF_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, (low_mask16), %mm4, %mm5, %mm6);

		movq %mm5, %es:-16(%edi);   /* Write results back */
		movq %mm6, %es:-8(%edi);
		decl %ecx;

		jnz trans_sse_16_half_loop;

	trans_sse_16_half_rest_of_line:

		movl B_WIDTH, %ecx;
		andl $7, %ecx;
		jz trans_sse_16_half_end;

		shrl $1, %ecx;
		jnc trans_sse_16_half_two_pixels;

	trans_sse_16_half_one_pixel:
		movzwl (%esi), %eax;        /* Read one pixel */
		movzwl %es:(%edi), %edx;

		movd %eax, %mm0;
		movd %edx, %mm1;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movd %mm1, %eax;
		addl $2, %esi;
		addl $2, %edi;
		movw %ax, %es:-2(%edi);     /* Write back */

	trans_sse_16_half_two_pixels:
		shrl $1, %ecx;
		jnc trans_sse_16_half_four_pixels;

		movd (%esi), %mm0;        /* mm0 = src |......|......| rgb1 | rgb2 | */
	    movd %es:(%edi), %mm1;    /* mm1 = dst |......|......| rgb2 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movd %mm1, %es:-4(%edi);    /* Write back */

	_align_;
	trans_sse_16_half_four_pixels:
		shrl $1, %ecx;
		jnc trans_sse_16_half_end;

		movq (%esi), %mm0;        /* mm0 = src |......|......| rgb1 | rgb2 | */
	    movq %es:(%edi), %mm1;    /* mm1 = dst |......|......| rgb2 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movq %mm1, %es:-8(%edi);    /* Write back */

		_align_;
		trans_sse_16_half_end:
	)

	_align_
	draw_trans_16_sse_end:

	emms

	restore_context(B_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_trans_mmx_16)

	save_context();

	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $16, %ecx
	je draw_trans_mmx_16_half


	BLIT_LOOP(draw_trans_mmx_16, 2, 2,

		shrl $3, %ecx;
		jz trans_mmx_16_rest_of_line;

	_align_;
	trans_mmx_16_loop:

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    SET_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3, %mm4, %mm5, %mm6, %mm7);
	    
	    movq (fact), %mm7;
	    movq %mm3, (temp);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movq (temp), %mm3;
	    addl $16, %esi;
	    addl $16, %edi;

		TRANSLUCENT_BLEND_16_4_MMX(%mm1, %mm3, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm2, %mm4, %mm5, %mm6)

		movq %mm0, %es:-16(%edi);  /* Write results back */
		movq %mm1, %es:-8(%edi);
		decl %ecx;

		jnz trans_mmx_16_loop;
		
	trans_mmx_16_rest_of_line:
		movl B_WIDTH, %ecx;
		andl $7, %ecx;
		jz trans_mmx_16_end;

	trans_mmx_16_one_pixel:		
		shrl $1, %ecx;
		jnc trans_mmx_16_two_pixels;

		movzwl (%esi), %eax;
		movzwl %es:(%edi), %edx;
		
		addl $2, %esi;
		addl $2, %edi;

		movd %eax, %mm0;
		movd %edx, %mm2;
		
	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movd %mm0, %eax;

		movw %ax,  %es:-2(%edi);  /* Write results back */
		
	trans_mmx_16_two_pixels:
		shrl $1, %ecx;
		jnc trans_mmx_16_four_pixels;

		movd (%esi), %mm0;
		movd %es:(%edi), %mm2;
		
		addl $4, %esi;
		addl $4, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movd %mm0,  %es:-4(%edi);  /* Write results back */

	_align_;
	trans_mmx_16_four_pixels:
		
		shrl $1, %ecx;
		jnc trans_mmx_16_end;

		movq (%esi), %mm0;
		movq %es:(%edi), %mm2;
		
		addl $8, %esi;
		addl $8, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm2, %mm4, %mm5);
	    
		TRANSLUCENT_BLEND_16_4_MMX(%mm0, %mm2, %mm7, (r_mask16), (g_mask16), (b_mask16), %mm3, %mm4, %mm5, %mm6)
		
		movq %mm0,  %es:-8(%edi);  /* Write results back */
	
	_align_;
	trans_mmx_16_end:
	
	)

	jmp draw_trans_16_mmx_end

	/****************
	 * Special code 
	 */
	_align_
	draw_trans_mmx_16_half:

	movq (high_mask16), %mm7;

	BLIT_LOOP(draw_trans_mmx_16_half, 2, 2,

		shrl $3, %ecx;
		jz trans_mmx_16_half_rest_of_line;

	_align_;
	trans_mmx_16_half_loop:

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */

		addl $16, %esi;
		addl $16, %edi;

	    SET_ON_EQUAL_16_8_MMX(%mm0, %mm1, (rb_mask16), %mm2, %mm3, %mm4, %mm5, %mm6, %mm7);

		movq (high_mask16), %mm7;
	    
		TRANSLUCENT_HALF_16_8_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, (low_mask16), %mm4, %mm5, %mm6);

		movq %mm5, %es:-16(%edi);   /* Write results back */
		movq %mm6, %es:-8(%edi);
		decl %ecx;

		jnz trans_mmx_16_half_loop;

	trans_mmx_16_half_rest_of_line:

		movl B_WIDTH, %ecx;
		andl $7, %ecx;
		jz trans_mmx_16_half_end;

		shrl $1, %ecx;
		jnc trans_mmx_16_half_two_pixels;

	trans_mmx_16_half_one_pixel:
		movzwl (%esi), %eax;        /* Read one pixel */
		movzwl %es:(%edi), %edx;

		movd %eax, %mm0;
		movd %edx, %mm1;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movd %mm1, %eax;
		addl $2, %esi;
		addl $2, %edi;
		movw %ax, %es:-2(%edi);     /* Write back */

	trans_mmx_16_half_two_pixels:
		shrl $1, %ecx;
		jnc trans_mmx_16_half_four_pixels;

		movd (%esi), %mm0;        /* mm0 = src |......|......| rgb1 | rgb2 | */
	    movd %es:(%edi), %mm1;    /* mm1 = dst |......|......| rgb2 | rgb2 | */

		addl $4, %esi;
		addl $4, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movd %mm1, %es:-4(%edi);    /* Write back */

	_align_;
	trans_mmx_16_half_four_pixels:
		shrl $1, %ecx;
		jnc trans_mmx_16_half_end;

		movq (%esi), %mm0;        /* mm0 = src |......|......| rgb1 | rgb2 | */
	    movq %es:(%edi), %mm1;    /* mm1 = dst |......|......| rgb2 | rgb2 | */

		addl $8, %esi;
		addl $8, %edi;

	    SET_ON_EQUAL_16_4_MMX(%mm0, (rb_mask16), %mm1, %mm2, %mm3);

		TRANSLUCENT_HALF_16_4_MMX(%mm0, %mm1, %mm7, (low_mask16), %mm2, %mm3);

		movq %mm1, %es:-8(%edi);    /* Write back */

		_align_;
		trans_mmx_16_half_end:
	)

	_align_
	draw_trans_16_mmx_end:

	emms

	restore_context(B_DEST);
	ret

#endif

