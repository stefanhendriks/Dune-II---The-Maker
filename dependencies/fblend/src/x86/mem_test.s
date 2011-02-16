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
buffer:  .long 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
_fblend_static_buffer: .long  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0


.text

#ifdef FBLEND_SSE

FUNC(fblend_mem_test_sse_16)

	save_context();
   
	movl B_DEST, %edx
	//movl B_SOURCE, %ebx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */
	//load_bitmap_segment(%edx, %es);
	//load_bitmap_segment(%ebx, %fs);


	BLIT_LOOP(mem_test_16_sse, 2, 2,
	
		leal (buffer), %eax;

		shrl $4, %ecx;
		jz mem_test_16_sse_rest_of_line;
		pushl %ecx;

	_align_;
	mem_test_16_sse_loop:
		prefetcht0 32(%esi);

		movq (%esi),   %mm0;
		movq 8(%esi),  %mm1;

		movq %es:(%edi),   %mm4;
		movq %es:8(%edi),  %mm5;
		
		paddw %mm4, %mm0;
		paddw %mm5, %mm1;
		
		//movq %mm0,   (%eax);
		//movq %mm1,  8(%eax);
		movq %mm0,  %es:(%edi);
		movq %mm1,  %es:8(%edi);
		
		prefetcht0 %es:32(%edi);

		movq 16(%esi), %mm0;
		movq 24(%esi), %mm1;
		addl $32, %esi;

		movq %es:16(%edi), %mm4;
		movq %es:24(%edi), %mm5;
		addl $32, %edi;

		paddw %mm4, %mm0;
		paddw %mm5, %mm1;

		//movq %mm0,  16(%eax);
		//movq %mm1,  24(%eax);
		movq %mm0,  %es:-16(%edi);
		movq %mm1,  %es:-8(%edi);
		
		addl $32, %eax;
		decl %ecx;
		jnz mem_test_16_sse_loop;

	popl %ecx;
	jmp mem_test_16_sse_rest_of_line;

	movl B_DEST, %edx;
	movl B_DEST_Y, %eax;
	WRITE_BANK();

	movl B_DEST_X, %edi;
	leal (%eax, %edi, 2), %edi;

	leal (buffer), %eax;
	
	_align_;
	mem_test_16_sse_copy_back_loop:

		movq (%eax),   %mm0;
		movq 8(%eax),  %mm1;
		movq 16(%eax), %mm2;
		movq 24(%eax), %mm3;
		addl $32, %eax;
		movq %mm0, %es:(%edi);
		movq %mm1, %es:8(%edi);
		movq %mm2, %es:16(%edi);
		movq %mm3, %es:24(%edi);
		addl $32, %edi;
		decl %ecx;
		jnz mem_test_16_sse_copy_back_loop;
		
	_align_;
	mem_test_16_sse_rest_of_line:
	)

/*
	BITMAP_BLEND_LOOP(mem_test_16_sse, 2, 2,

		shrl $4, %ecx;
		jz mem_test_16_sse_rest_of_line;

	_align_;
	mem_test_16_sse_loop:
		prefetcht0 %fs:32(%esi);

		movq %fs:(%esi),   %mm0;
		movq %fs:8(%esi),  %mm1;

		movq %es:(%edi),   %mm4;
		movq %es:8(%edi),  %mm5;
		
		paddw %mm4, %mm0;
		paddw %mm5, %mm1;
		
		movq %mm0,   (%eax);
		movq %mm1,  8(%eax);
		
		prefetcht0 %es:32(%edi);

		movq %fs:16(%esi), %mm0;
		movq %fs:24(%esi), %mm1;
		addl $32, %esi;

		movq %es:16(%edi), %mm4;
		movq %es:24(%edi), %mm5;
		addl $32, %edi;

		paddw %mm4, %mm0;
		paddw %mm5, %mm1;

		movq %mm0,  16(%eax);
		movq %mm1,  24(%eax);
		
		addl $32, %eax;
		decl %ecx;
		jnz mem_test_16_sse_loop;
	
	mem_test_16_sse_rest_of_line:
	
	);
*/
	emms

	restore_context(B_DEST);
	ret

#endif

#ifdef FBLEND_MMX

FUNC(fblend_mem_test_mmx_16)

	save_context();
   
	movl B_DEST, %edx
	//movl B_SOURCE, %ebx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */
	//load_bitmap_segment(%edx, %es);
	//load_bitmap_segment(%ebx, %fs);


	BLIT_LOOP(mem_test_16_mmx, 2, 2,
	
		leal (buffer), %eax;

		shrl $4, %ecx;
		jz mem_test_16_mmx_rest_of_line;
		pushl %ecx;

	_align_;
	mem_test_16_mmx_loop:

		movq (%esi),   %mm0;
		movq 8(%esi),  %mm1;

		movq %es:(%edi),   %mm4;
		movq %es:8(%edi),  %mm5;
		
		paddw %mm4, %mm0;
		paddw %mm5, %mm1;
		
		movq %mm0,   (%eax);
		movq %mm1,  8(%eax);
		
		movq 16(%esi), %mm0;
		movq 24(%esi), %mm1;
		addl $32, %esi;

		movq %es:16(%edi), %mm4;
		movq %es:24(%edi), %mm5;
		addl $32, %edi;

		paddw %mm4, %mm0;
		paddw %mm5, %mm1;

		movq %mm0,  16(%eax);
		movq %mm1,  24(%eax);
		
		addl $32, %eax;
		decl %ecx;
		jnz mem_test_16_mmx_loop;

	popl %ecx;

	movl B_DEST, %edx;
	movl B_DEST_Y, %eax;
	WRITE_BANK();

	movl B_DEST_X, %edi;
	leal (%eax, %edi, 2), %edi;

	leal (buffer), %eax;
	
	_align_;
	mem_test_16_mmx_copy_back_loop:

		movq (%eax),   %mm0;
		movq 8(%eax),  %mm1;
		movq 16(%eax), %mm2;
		movq 24(%eax), %mm3;
		addl $32, %eax;
		movq %mm0, %es:(%edi);
		movq %mm1, %es:8(%edi);
		movq %mm2, %es:16(%edi);
		movq %mm3, %es:24(%edi);
		addl $32, %edi;
		decl %ecx;
		jnz mem_test_16_mmx_copy_back_loop;
		
	_align_;
	mem_test_16_mmx_rest_of_line:
	)
	
	emms

	restore_context(B_DEST);
	ret

#endif
