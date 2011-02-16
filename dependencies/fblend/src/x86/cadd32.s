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

.data
.balign 32, 0x90
rb_mask32:  .long 0x00FF00FF, 0x00FF00FF
g_mask32:   .long 0x0000FF00, 0x0000FF00
fact:       .long 0, 0

.text

#ifdef FBLEND_SSE

FUNC(fblend_add_sse_32)

	save_context();	
   
	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $255, %ecx                /* Special case: factor == 255 */
	je draw_color_add_sse_32_full
	

	BLIT_LOOP(draw_color_add_sse_32, 4, 4,
	
		shrl $2, %ecx;
		jz color_add_sse_32_rest_of_line;

	_align_;
	color_add_sse_32_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | */
		movq 8(%esi), %mm4;       /*src2| rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		movq %mm4, %mm5;

		pcmpeqd %mm6, %mm0;
		pcmpeqd %mm6, %mm4;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		pandn %mm5, %mm4;
		
		movq %mm0, %mm1;
		movq %mm4, %mm5;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		
		pand (g_mask32), %mm5;    /*src2| 0g01 | 0g02 | */
		psrlq $8, %mm1;

		movq %mm7, %mm6;
		psrlq $8, %mm5;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		addl $16, %esi;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
		movq %mm7, %mm3;
		movq %mm7, %mm6;

		pmullw %mm4, %mm3;
		pmullw %mm5, %mm6;
		addl $16, %edi;

		psrlq $8, %mm3;
		pand (rb_mask32), %mm3;
		pand (g_mask32), %mm6;
		por %mm6, %mm3;
		
	    movq %es:-16(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi), %mm5;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */
		paddusb %mm5, %mm3;

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_sse_32_loop;

	color_add_sse_32_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $3, %ecx;
		jz color_add_sse_32_end;

		shrl $1, %ecx;
		jnc color_add_sse_32_two_pixels;
	
	color_add_sse_32_one_pixel:

		movd (%esi), %mm0;        /*src | rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */

		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
		movq %mm0, %mm1;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		
		psrlq $8, %mm1;
		movq %mm7, %mm6;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		
		addl $4, %esi;
		addl $4, %edi;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
	    movd %es:-4(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */

		movd %mm2, %es:-4(%edi);  /* Write the results */

	color_add_sse_32_two_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_32_end;
		
		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */

		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
		movq %mm0, %mm1;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		
		psrlq $8, %mm1;
		movq %mm7, %mm6;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
	    movq %es:(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */

		movq %mm2, %es:(%edi);  /* Write the results */

		_align_;
		color_add_sse_32_end:
	)
	
	jmp draw_color_add_sse_32_end
	
	
	/***********************
	 * Start of special case
	 */
	draw_color_add_sse_32_full:

	BLIT_LOOP(draw_color_add_sse_32_full, 4, 4,

		shrl $2, %ecx;
		jz color_add_sse_32_full_rest_of_line;

	_align_;
	color_add_sse_32_full_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm4;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		movq %mm4, %mm5;

		pcmpeqd %mm6, %mm0;
		pcmpeqd %mm6, %mm4;

		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		pandn %mm5, %mm4;		
		
	    movq %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm5;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $16, %esi;
	    addl $16, %edi;
	    
	    paddusb %mm1, %mm0;
	    paddusb %mm5, %mm4;

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm4, %es:-8(%edi);
		decl %ecx;

		jnz color_add_sse_32_full_loop;

	color_add_sse_32_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $3, %ecx;
		jz color_add_sse_32_full_end;

		shrl $1, %ecx;
		jnc color_add_sse_32_full_two_pixels;
	
	color_add_sse_32_full_one_pixel:

		movd (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
	    movd %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $4, %esi;
	    addl $4, %edi;
	    
	    paddusb %mm1, %mm0;

		movd %mm0, %es:-4(%edi);  /* Write the results */

	color_add_sse_32_full_two_pixels:
		shrl $1, %ecx;
		jnc color_add_sse_32_end;
		
		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
	    movd %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    paddusb %mm1, %mm0;

		movq %mm0, %es:(%edi);  /* Write the results */

		_align_;
		color_add_sse_32_full_end:
	)	

	draw_color_add_sse_32_end:
	
	emms

	restore_context(B_DEST);
	ret

#endif

#ifdef FBLEND_MMX

FUNC(fblend_add_mmx_32)

	save_context();
   
	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact)

	cmpl $255, %ecx                /* Special case: factor == 255 */
	je draw_color_add_mmx_32_full
	

	BLIT_LOOP(draw_color_add_mmx_32, 4, 4,
	
		shrl $2, %ecx;
		jz color_add_mmx_32_rest_of_line;

	_align_;
	color_add_mmx_32_loop:

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | */
		movq 8(%esi), %mm4;       /*src2| rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		movq %mm4, %mm5;

		pcmpeqd %mm6, %mm0;
		pcmpeqd %mm6, %mm4;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		pandn %mm5, %mm4;
		
		movq %mm0, %mm1;
		movq %mm4, %mm5;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		
		pand (g_mask32), %mm5;    /*src2| 0g01 | 0g02 | */
		psrlq $8, %mm1;

		movq %mm7, %mm6;
		psrlq $8, %mm5;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		addl $16, %esi;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
		movq %mm7, %mm3;
		movq %mm7, %mm6;

		pmullw %mm4, %mm3;
		pmullw %mm5, %mm6;
		addl $16, %edi;

		psrlq $8, %mm3;
		pand (rb_mask32), %mm3;
		pand (g_mask32), %mm6;
		por %mm6, %mm3;
		
	    movq %es:-16(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:-8(%edi), %mm5;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */
		paddusb %mm5, %mm3;

		movq %mm2, %es:-16(%edi);  /* Write the results */
		movq %mm3, %es:-8(%edi);
		decl %ecx;

		jnz color_add_mmx_32_loop;

	color_add_mmx_32_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $3, %ecx;
		jz color_add_mmx_32_end;

		shrl $1, %ecx;
		jnc color_add_mmx_32_two_pixels;
	
	color_add_mmx_32_one_pixel:

		movd (%esi), %mm0;        /*src | rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */

		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
		movq %mm0, %mm1;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		
		psrlq $8, %mm1;
		movq %mm7, %mm6;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		
		addl $4, %esi;
		addl $4, %edi;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
	    movd %es:-4(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */

		movd %mm2, %es:-4(%edi);  /* Write the results */

	color_add_mmx_32_two_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_32_end;
		
		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */

		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
		movq %mm0, %mm1;

		pand %mm6, %mm0;          /*src | r0b1 | r0b2 | */
		movq %mm7, %mm2;

		pand %mm6, %mm4;          /*src2| r0b1 | r0b2 | */
		pand (g_mask32), %mm1;    /*src1| 0g01 | 0g02 | */
		
		psrlq $8, %mm1;
		movq %mm7, %mm6;

		pmullw %mm0, %mm2;        /* Multiply by factor */
		pmullw %mm1, %mm6;
		
		psrlq $8, %mm2;
		pand (rb_mask32), %mm2;
		pand (g_mask32), %mm6;
		por %mm6, %mm2;
		
	    movq %es:(%edi), %mm1;  /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		paddusb %mm1, %mm2;  /* Add with satuartion */

		movq %mm2, %es:(%edi);  /* Write the results */

		_align_;
		color_add_mmx_32_end:
	)
	
	jmp draw_color_add_mmx_32_end
	
	
	/***********************
	 * Start of special case
	 */
	draw_color_add_mmx_32_full:

	BLIT_LOOP(draw_color_add_mmx_32_full, 4, 4,

		shrl $2, %ecx;
		jz color_add_mmx_32_full_rest_of_line;

	_align_;
	color_add_mmx_32_full_loop:

		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm4;       /*src2| rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		movq %mm4, %mm5;

		pcmpeqd %mm6, %mm0;
		pcmpeqd %mm6, %mm4;

		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		pandn %mm5, %mm4;		
		
	    movq %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm5;   /*dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $16, %esi;
	    addl $16, %edi;
	    
	    paddusb %mm1, %mm0;
	    paddusb %mm5, %mm4;

		movq %mm0, %es:-16(%edi);  /* Write the results */
		movq %mm4, %es:-8(%edi);
		decl %ecx;

		jnz color_add_mmx_32_full_loop;

	color_add_mmx_32_full_rest_of_line:		
		movl B_WIDTH, %ecx;     /* Check if there are pixels left */
		andl $3, %ecx;
		jz color_add_mmx_32_full_end;

		shrl $1, %ecx;
		jnc color_add_mmx_32_full_two_pixels;
	
	color_add_mmx_32_full_one_pixel:

		movd (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
	    movd %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    addl $4, %esi;
	    addl $4, %edi;
	    
	    paddusb %mm1, %mm0;

		movd %mm0, %es:-4(%edi);  /* Write the results */

	color_add_mmx_32_full_two_pixels:
		shrl $1, %ecx;
		jnc color_add_mmx_32_end;
		
		movq (%esi), %mm0;        /*src | rgb1 | rgb2 | rgb3 | rgb4 | */

		movq (rb_mask32), %mm6;
		movq %mm0, %mm1;          /* Compare to mask */
		pcmpeqd %mm6, %mm0;
		pandn %mm1, %mm0;         /* Set to 0 if eq to mask */
		
	    movd %es:(%edi), %mm1;    /*dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
	    paddusb %mm1, %mm0;

		movq %mm0, %es:(%edi);  /* Write the results */

		_align_;
		color_add_mmx_32_full_end:
	)	

	draw_color_add_mmx_32_end:
	
	emms
	
	restore_context(B_DEST);
	ret

#endif
