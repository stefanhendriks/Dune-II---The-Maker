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
#include "stretch.inc"

.data
.balign 32, 0x90

.text


#ifdef FBLEND_SSE

FUNC(fblend_2x_stretch_16_sse)

	save_context();

	movl S_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	pushl $0

	STRETCH_LOOP(stretch_16_sse, 2,
		shrl $4, %ecx;
		jz stretch_16_sse_rest_of_line;

	_align_;
	stretch_16_sse_sixteen_pixels:

		prefetcht0  32(%esi);

		movq   (%esi), %mm0;
		movq  8(%esi), %mm1;
		movq 16(%esi), %mm2;
		movq 24(%esi), %mm3;

		movq %mm0, %mm4;
		punpcklwd %mm0, %mm0;
		movq %mm1, %mm5;
		punpcklwd %mm1, %mm1;
		movq %mm2, %mm6;
		punpcklwd %mm2, %mm2;
		movq %mm3, %mm7;
		punpcklwd %mm3, %mm3;
		addl $32, %esi;
		punpckhwd %mm4, %mm4;
		addl $64, %edi;
		punpckhwd %mm5, %mm5;
		punpckhwd %mm6, %mm6;
		punpckhwd %mm7, %mm7;

		movq %mm0, %es:-64(%edi);
		movq %mm4, %es:-56(%edi);
		movq %mm1, %es:-48(%edi);
		movq %mm5, %es:-40(%edi);
		movq %mm2, %es:-32(%edi);
		movq %mm6, %es:-24(%edi);
		movq %mm3, %es:-16(%edi);

		decl %ecx;
		movq %mm7, %es: -8(%edi);

		jnz stretch_16_sse_sixteen_pixels;

	_align_;
	stretch_16_sse_rest_of_line:

		movl S_SOURCE_WIDTH, %ecx;
		andl $15, %ecx;
		jz stretch_16_sse_last_pixel;

		shrl $1, %ecx;
		jnc stretch_16_sse_two_pixels;

	stretch_16_sse_one_pixel:
		movzwl (%esi), %eax;
		addl $2, %esi;
		movl %eax, %edx;
		shll $16, %eax;
		addl $4, %edi;
		orl %edx, %eax;
		
		movl %eax, %es:-4(%edi);

	_align_;
	stretch_16_sse_two_pixels:
		shrl $1, %ecx;
		jnc stretch_16_sse_four_pixels;

		movd (%esi), %mm0;
		addl $8, %edi;
		punpcklwd %mm0, %mm0;
		addl $4, %esi;
		movq %mm0, %es:-8(%edi);

	_align_;
	stretch_16_sse_four_pixels:
		shrl $1, %ecx;
		jnc stretch_16_sse_eight_pixels;

		movq (%esi), %mm0;
		addl $16, %edi;
		movq %mm0, %mm1;
		punpcklwd %mm0, %mm0;
		punpckhwd %mm1, %mm1;
		addl $8, %esi;
		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

	_align_;
	stretch_16_sse_eight_pixels:
		shrl $1, %ecx;
		jnc stretch_16_sse_last_pixel;

		movq  (%esi), %mm0;
		movq 8(%esi), %mm1;
		addl $32, %edi;
		movq %mm0, %mm2;
		movq %mm1, %mm3;
		punpcklwd %mm0, %mm0;
		punpcklwd %mm1, %mm1;
		punpckhwd %mm2, %mm2;
		punpckhwd %mm3, %mm3;
		addl $16, %esi;
		movq %mm0, %es:-32(%edi);
		movq %mm2, %es:-24(%edi);
		movq %mm1, %es:-16(%edi);
		movq %mm3, %es: -8(%edi);

	_align_;
	stretch_16_sse_last_pixel:
		movl S_SOURCE_WIDTH, %ecx;
		shll $1, %ecx;
		cmpl S_DEST_WIDTH, %ecx;
		je stretch_16_sse_end_of_line;

		movw (%esi), %ax;
		movw %ax, %es:(%edi);

	_align_;
	stretch_16_sse_end_of_line:

	);

	_align_
	stretch_16_sse_end:

	popl %eax;
	
	emms

	restore_context(S_DEST);
	ret


FUNC(fblend_2x_stretch_32_sse)

	save_context();

	movl S_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	pushl $0

	STRETCH_LOOP(stretch_32_sse, 4,
		shrl $3, %ecx;
		jz stretch_32_sse_rest_of_line;

	_align_;
	stretch_32_sse_eight_pixels:
	
		prefetcht0 32(%esi);

		movq   (%esi), %mm0;
		movq  8(%esi), %mm1;
		movq 16(%esi), %mm2;
		movq 24(%esi), %mm3;

		movq %mm0, %mm4;
		punpckldq %mm0, %mm0;
		movq %mm1, %mm5;
		punpckldq %mm1, %mm1;
		movq %mm2, %mm6;
		punpckldq %mm2, %mm2;
		movq %mm3, %mm7;
		punpckldq %mm3, %mm3;
		addl $32, %esi;
		punpckhdq %mm4, %mm4;
		addl $64, %edi;
		punpckhdq %mm5, %mm5;
		punpckhdq %mm6, %mm6;
		punpckhdq %mm7, %mm7;

		movq %mm0, %es:-64(%edi);
		movq %mm4, %es:-56(%edi);
		movq %mm1, %es:-48(%edi);
		movq %mm5, %es:-40(%edi);
		movq %mm2, %es:-32(%edi);
		movq %mm6, %es:-24(%edi);
		movq %mm3, %es:-16(%edi);

		decl %ecx;
		movq %mm7, %es: -8(%edi);

		jnz stretch_32_sse_eight_pixels;

	_align_;
	stretch_32_sse_rest_of_line:
		movl S_SOURCE_WIDTH, %ecx;
		andl $7, %ecx;
		jz stretch_32_sse_last_pixel;
		shrl $1, %ecx;
		jnc stretch_32_sse_two_pixels;

	stretch_32_sse_one_pixel:
		movd (%esi), %mm0;
		addl $4, %esi;
		punpckldq %mm0, %mm0;
		addl $8, %edi;

		movq %mm0, %es:-8(%edi);

	_align_;
	stretch_32_sse_two_pixels:
		shrl $1, %ecx;
		jnc stretch_32_sse_four_pixels;

		movq (%esi), %mm0;
		addl $16, %edi;
		movq %mm0, %mm1;
		punpckldq %mm0, %mm0;
		punpckhdq %mm1, %mm1;
		addl $8, %esi;
		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

	_align_;
	stretch_32_sse_four_pixels:
		shrl $1, %ecx;
		jnc stretch_32_sse_last_pixel;

		movq (%esi), %mm0;
		movq (%esi), %mm1;
		addl $32, %edi;
		movq %mm0, %mm2;
		movq %mm1, %mm3;
		punpckldq %mm0, %mm0;
		punpckldq %mm1, %mm1;
		punpckhdq %mm2, %mm2;
		punpckhdq %mm3, %mm3;
		addl $16, %esi;
		movq %mm0, %es:-32(%edi);
		movq %mm2, %es:-24(%edi);
		movq %mm1, %es:-16(%edi);
		movq %mm3, %es: -8(%edi);

	_align_;
	stretch_32_sse_last_pixel:
		movl S_SOURCE_WIDTH, %ecx;
		shll $1, %ecx;
		cmpl S_DEST_WIDTH, %ecx;
		je stretch_32_sse_end_of_line;

		movl (%esi), %eax;
		movl %eax, %es:(%edi);

	_align_;
	stretch_32_sse_end_of_line:

	);

	_align_
	stretch_32_sse_end:
	
	popl %eax;
	
	emms

	restore_context(S_DEST);
	ret


#endif

#ifdef FBLEND_MMX

FUNC(fblend_2x_stretch_16_mmx)

	save_context();

	movl S_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	pushl $0

	STRETCH_LOOP(stretch_16_mmx, 2,
		shrl $4, %ecx;
		jz stretch_16_mmx_rest_of_line;

	_align_;
	stretch_16_mmx_sixteen_pixels:
	
		movq   (%esi), %mm0;
		movq  8(%esi), %mm1;
		movq 16(%esi), %mm2;
		movq 24(%esi), %mm3;

		movq %mm0, %mm4;
		punpcklwd %mm0, %mm0;
		movq %mm1, %mm5;
		punpcklwd %mm1, %mm1;
		movq %mm2, %mm6;
		punpcklwd %mm2, %mm2;
		movq %mm3, %mm7;
		punpcklwd %mm3, %mm3;
		addl $32, %esi;
		punpckhwd %mm4, %mm4;
		addl $64, %edi;
		punpckhwd %mm5, %mm5;
		punpckhwd %mm6, %mm6;
		punpckhwd %mm7, %mm7;

		movq %mm0, %es:-64(%edi);
		movq %mm4, %es:-56(%edi);
		movq %mm1, %es:-48(%edi);
		movq %mm5, %es:-40(%edi);
		movq %mm2, %es:-32(%edi);
		movq %mm6, %es:-24(%edi);
		movq %mm3, %es:-16(%edi);

		decl %ecx;
		movq %mm7, %es: -8(%edi);

		jnz stretch_16_mmx_sixteen_pixels;

	_align_;
	stretch_16_mmx_rest_of_line:

		movl S_SOURCE_WIDTH, %ecx;
		andl $15, %ecx;
		jz stretch_16_mmx_last_pixel;

		shrl $1, %ecx;
		jnc stretch_16_mmx_two_pixels;

	stretch_16_mmx_one_pixel:
		movzwl (%esi), %eax;
		addl $2, %esi;
		movl %eax, %edx;
		shll $16, %eax;
		addl $4, %edi;
		orl %edx, %eax;
		
		movl %eax, %es:-4(%edi);

	_align_;
	stretch_16_mmx_two_pixels:
		shrl $1, %ecx;
		jnc stretch_16_mmx_four_pixels;

		movd (%esi), %mm0;
		addl $8, %edi;
		punpcklwd %mm0, %mm0;
		addl $4, %esi;
		movq %mm0, %es:-8(%edi);

	_align_;
	stretch_16_mmx_four_pixels:
		shrl $1, %ecx;
		jnc stretch_16_mmx_eight_pixels;

		movq (%esi), %mm0;
		addl $16, %edi;
		movq %mm0, %mm1;
		punpcklwd %mm0, %mm0;
		punpckhwd %mm1, %mm1;
		addl $8, %esi;
		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

	_align_;
	stretch_16_mmx_eight_pixels:
		shrl $1, %ecx;
		jnc stretch_16_mmx_last_pixel;

		movq  (%esi), %mm0;
		movq 8(%esi), %mm1;
		addl $32, %edi;
		movq %mm0, %mm2;
		movq %mm1, %mm3;
		punpcklwd %mm0, %mm0;
		punpcklwd %mm1, %mm1;
		punpckhwd %mm2, %mm2;
		punpckhwd %mm3, %mm3;
		addl $16, %esi;
		movq %mm0, %es:-32(%edi);
		movq %mm2, %es:-24(%edi);
		movq %mm1, %es:-16(%edi);
		movq %mm3, %es: -8(%edi);

	_align_;
	stretch_16_mmx_last_pixel:
		movl S_SOURCE_WIDTH, %ecx;
		shll $1, %ecx;
		cmpl S_DEST_WIDTH, %ecx;
		je stretch_16_mmx_end_of_line;

		movw (%esi), %ax;
		movw %ax, %es:(%edi);

	_align_;
	stretch_16_mmx_end_of_line:

	);

	_align_
	stretch_16_mmx_end:

	popl %eax;
	
	emms

	restore_context(S_DEST);
	ret


FUNC(fblend_2x_stretch_32_mmx)

	save_context();

	movl S_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	pushl $0

	STRETCH_LOOP(stretch_32_mmx, 4,
		shrl $3, %ecx;
		jz stretch_32_mmx_rest_of_line;

	_align_;
	stretch_32_mmx_eight_pixels:

		movq   (%esi), %mm0;
		movq  8(%esi), %mm1;
		movq 16(%esi), %mm2;
		movq 24(%esi), %mm3;

		movq %mm0, %mm4;
		punpckldq %mm0, %mm0;
		movq %mm1, %mm5;
		punpckldq %mm1, %mm1;
		movq %mm2, %mm6;
		punpckldq %mm2, %mm2;
		movq %mm3, %mm7;
		punpckldq %mm3, %mm3;
		addl $32, %esi;
		punpckhdq %mm4, %mm4;
		addl $64, %edi;
		punpckhdq %mm5, %mm5;
		punpckhdq %mm6, %mm6;
		punpckhdq %mm7, %mm7;

		movq %mm0, %es:-64(%edi);
		movq %mm4, %es:-56(%edi);
		movq %mm1, %es:-48(%edi);
		movq %mm5, %es:-40(%edi);
		movq %mm2, %es:-32(%edi);
		movq %mm6, %es:-24(%edi);
		movq %mm3, %es:-16(%edi);

		decl %ecx;
		movq %mm7, %es: -8(%edi);

		jnz stretch_32_mmx_eight_pixels;

	_align_;
	stretch_32_mmx_rest_of_line:
		movl S_SOURCE_WIDTH, %ecx;
		andl $7, %ecx;
		jz stretch_32_mmx_last_pixel;
		shrl $1, %ecx;
		jnc stretch_32_mmx_two_pixels;

	stretch_32_mmx_one_pixel:
		movd (%esi), %mm0;
		addl $4, %esi;
		punpckldq %mm0, %mm0;
		addl $8, %edi;

		movq %mm0, %es:-8(%edi);

	_align_;
	stretch_32_mmx_two_pixels:
		shrl $1, %ecx;
		jnc stretch_32_mmx_four_pixels;

		movq (%esi), %mm0;
		addl $16, %edi;
		movq %mm0, %mm1;
		punpckldq %mm0, %mm0;
		punpckhdq %mm1, %mm1;
		addl $8, %esi;
		movq %mm0, %es:-16(%edi);
		movq %mm1, %es: -8(%edi);

	_align_;
	stretch_32_mmx_four_pixels:
		shrl $1, %ecx;
		jnc stretch_32_mmx_last_pixel;

		movq (%esi), %mm0;
		movq (%esi), %mm1;
		addl $32, %edi;
		movq %mm0, %mm2;
		movq %mm1, %mm3;
		punpckldq %mm0, %mm0;
		punpckldq %mm1, %mm1;
		punpckhdq %mm2, %mm2;
		punpckhdq %mm3, %mm3;
		addl $16, %esi;
		movq %mm0, %es:-32(%edi);
		movq %mm2, %es:-24(%edi);
		movq %mm1, %es:-16(%edi);
		movq %mm3, %es: -8(%edi);

	_align_;
	stretch_32_mmx_last_pixel:
		movl S_SOURCE_WIDTH, %ecx;
		shll $1, %ecx;
		cmpl S_DEST_WIDTH, %ecx;
		je stretch_32_mmx_end_of_line;

		movl (%esi), %eax;
		movl %eax, %es:(%edi);

	_align_;
	stretch_32_mmx_end_of_line:

	);

	_align_
	stretch_32_mmx_end:
	
	popl %eax;
	
	emms

	restore_context(S_DEST);
	ret

#endif
