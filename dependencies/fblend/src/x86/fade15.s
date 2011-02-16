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
#include "fade.inc"

.data
.balign 32, 0x90
r_mask15:     .long 0x7C007C00, 0x7C007C00
g_mask15:     .long 0x03E003E0, 0x03E003E0
b_mask15:     .long 0x001F001F, 0x001F001F
.balign 32, 0x90
temp:         .long 0, 0


.text

#ifdef FBLEND_MMX

FUNC(fblend_fade_mmx_15)

	save_context();

	movl F_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl F_FACT, %ecx;
	negl %ecx;
	addl $32, %ecx;
	
	movd %ecx, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | 32 - fact | 32 - fact | 32 - fact | 32 - fact | */
	
	movl F_COLOR, %ecx;
	movd %ecx, %mm1;
	punpcklwd %mm1, %mm1;
	punpcklwd %mm1, %mm1;         /* mm1 = | color | color | color | color | */
	
	movq (r_mask15), %mm2;
	movq (g_mask15), %mm3;
	
	pxor %mm0, %mm0;
	
	FADE_16_4_MMX(%mm1, %mm2, %mm3, (b_mask15), %mm7, %mm0, %mm6, %mm4, %mm5, %mm0);

	movq %mm1, (temp);

	movd F_FACT, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */

	FADE_LOOP(fade_mmx_15, 2, 2,

		shrl $3, %ecx;
		jz fade_mmx_15_rest_of_line;

	_align_;
	fade_mmx_15_loop:

		movq (%esi), %mm0;        /* Read source */
		FADE_16_4_MMX(%mm0, %mm2, %mm3, (b_mask15), %mm7, (temp), %mm1, %mm4, %mm5, %mm6);
		movq %mm0, %es:(%edi);    /* Write to dest */

		movq 8(%esi), %mm0;       /* Read source */
		FADE_16_4_MMX(%mm0, %mm2, %mm3, (b_mask15), %mm7, (temp), %mm1, %mm4, %mm5, %mm6);
		movq %mm0, %es:8(%edi);   /* Write to dest */

		addl $16, %esi;
		addl $16, %edi;

		decl %ecx;
		jnz fade_mmx_15_loop;

	_align_;
	fade_mmx_15_rest_of_line:
		movl F_WIDTH, %ecx;
		andl $7, %ecx;
		jz fade_mmx_15_end_of_line;
		
	fade_mmx_15_one_pixel:
		shrl $1, %ecx;
		jnc fade_mmx_15_two_pixels;
	
		movzwl (%esi), %eax;
		movd %eax, %mm0;
		FADE_16_4_MMX(%mm0, %mm2, %mm3, (b_mask15), %mm7, (temp), %mm1, %mm4, %mm5, %mm6);
		movd %mm0, %eax;
		
		addl $2, %edi;
		addl $2, %esi;

		movw %ax, %es:-2(%edi);
			
	fade_mmx_15_two_pixels:
		shrl $1, %ecx;
		jnc fade_mmx_15_four_pixels;
		
		movd (%esi), %mm0;
		FADE_16_4_MMX(%mm0, %mm2, %mm3, (b_mask15), %mm7, (temp), %mm1, %mm4, %mm5, %mm6);
		movd %mm0, %es:(%edi);
		
		addl $4, %edi;
		addl $4, %esi;

	_align_;
	fade_mmx_15_four_pixels:
		shrl $1, %ecx;
		jnc fade_mmx_15_end_of_line;
		
		movq (%esi), %mm0;
		FADE_16_4_MMX(%mm0, %mm2, %mm3, (b_mask15), %mm7, (temp), %mm1, %mm4, %mm5, %mm6);
		movq %mm0, %es:8(%edi);

	_align_;
	fade_mmx_15_end_of_line:
	)
_align_;
fade_mmx_15_end:

	emms;

	restore_context(B_DEST);
	ret

#endif


