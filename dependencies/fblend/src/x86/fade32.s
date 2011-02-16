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
rb_mask32:    .long 0x00FF00FF, 0x00FF00FF
g_mask32:     .long 0x0000FF00, 0x0000FF00


.text

#ifdef FBLEND_MMX

FUNC(fblend_fade_mmx_32)

	save_context();

	movl F_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl F_FACT, %ecx;
	negl %ecx;
	addl $256, %ecx;

	movd %ecx, %mm7;
	punpcklwd %mm7, %mm7;
	punpcklwd %mm7, %mm7;         /* mm7 = | 256 - fact | 256 - fact | */
	
	movl F_COLOR, %ecx;
	movd %ecx, %mm6;
	punpckldq %mm6, %mm6;         /* mm6 = | color | color | */
	
	movq (rb_mask32), %mm2;
	movq (g_mask32), %mm3;
	
	pxor %mm0, %mm0;
	
	FADE_32_4_MMX(%mm6, %mm0, %mm2, %mm3, %mm7, %mm0, %mm4, %mm5);

	movd F_FACT, %mm7
	punpcklwd %mm7, %mm7;
	punpckldq %mm7, %mm7          /* mm7 = | fact | fact | */

	FADE_LOOP(fade_mmx_32, 4, 4,

		shrl $3, %ecx;
		jz fade_mmx_32_rest_of_line;

	_align_;
	fade_mmx_32_loop:

		movq (%esi), %mm0;         /* Read source */
		movq 8(%esi), %mm1;
		FADE_32_4_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, %mm6, %mm4, %mm5);
		movq %mm0, %es:(%edi);     /* Write to dest */
		movq %mm1, %es:8(%edi);

		movq 16(%esi), %mm0;         /* Read source */
		movq 24(%esi), %mm1;
		FADE_32_4_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, %mm6, %mm4, %mm5);
		movq %mm0, %es:16(%edi);     /* Write to dest */
		movq %mm1, %es:24(%edi);

		addl $32, %esi;
		addl $32, %edi;

		decl %ecx;
		jnz fade_mmx_32_loop;

	_align_;
	fade_mmx_32_rest_of_line:
		movl F_WIDTH, %ecx;
		andl $7, %ecx;
		jz fade_mmx_32_end_of_line;
		
	fade_mmx_32_one_pixel:
		shrl $1, %ecx;
		jnc fade_mmx_32_two_pixels;
	
		movd (%esi), %mm0;
		FADE_32_4_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, %mm6, %mm4, %mm5);
		movd %mm0, %es:(%edi);
		
		addl $4, %edi;
		addl $4, %esi;
		
	fade_mmx_32_two_pixels:
		shrl $1, %ecx;
		jnc fade_mmx_32_four_pixels;
		
		movq (%esi), %mm0;
		FADE_32_4_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, %mm6, %mm4, %mm5);
		movq %mm0, %es:(%edi);
		
		addl $8, %edi;
		addl $8, %esi;

	_align_;
	fade_mmx_32_four_pixels:
		shrl $1, %ecx;
		jnc fade_mmx_32_end_of_line;
		
		movq (%esi), %mm0;
		movq 8(%esi), %mm1;
		FADE_32_4_MMX(%mm0, %mm1, %mm2, %mm3, %mm7, %mm6, %mm4, %mm5);
		movq %mm0, %es:(%edi);
		movq %mm1, %es:8(%edi);

	_align_;
	fade_mmx_32_end_of_line:
	)
_align_;
fade_mmx_32_end:

	emms;

	restore_context(B_DEST);
	ret

#endif


