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
g_mask32:     .long 0x0000FF00, 0x0000FF00
rb_mask32:    .long 0x00FF00FF, 0x00FF00FF
high_mask_32: .long 0x00FEFEFE, 0x00FEFEFE
low_mask_32:  .long 0x00010101, 0x00010101
fact32:       .long 0, 0
temp:         .long 0, 0

.text

#ifdef FBLEND_SSE

FUNC(fblend_trans_sse_32)

	save_context();
   
	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact32)

	cmpl $128, %ecx
	je draw_trans_sse_32_half

	cmpl $127, %ecx
	je draw_trans_sse_32_half

	BLIT_LOOP(draw_trans_sse_32, 4, 4,

		shrl $2, %ecx;
		jz trans_sse_32_rest_of_line;

	_align_;
	trans_sse_32_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | */
	    
	    addl $16, %esi;
	    addl $16, %edi;
	    
		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    pcmpeqd (rb_mask32), %mm1;
	    movq %mm2, %mm6;
	    movq %mm3, %mm7;
	    pand %mm0, %mm6;
	    pand %mm1, %mm7;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    pandn %mm5, %mm1;
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */
	    por %mm7, %mm1;

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
		movq %mm2, (temp);
		movq %mm3, (temp + 8);

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm1;
		
		pand %mm6, %mm2;
		pand %mm6, %mm3;

		psubw %mm2, %mm0;            /* source -= dest */
		psubw %mm3, %mm1;
		
		movq (temp),     %mm2;
		movq (temp + 8), %mm3;

		pand %mm7, %mm5;             /* Split B */
		pand %mm7, %mm4;
		
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */
		pmullw (fact32), %mm1;

		psrlw $8, %mm4;
		psrlw $8, %mm5;

		pand %mm7, %mm2;
		pand %mm7, %mm3;

		psrlw $8, %mm2;
		psrlw $8, %mm3;

		psubw %mm2, %mm4;
		psubw %mm3, %mm5;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		pmullw (fact32), %mm5;
		
		movq (temp),   %mm2;
		movq (temp+8), %mm3;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		psrlw $8, %mm1;
		pand %mm7, %mm5;

		por  %mm4, %mm0;             /* Recombine components */
		por  %mm5, %mm1;

		paddb %mm2, %mm0;
		paddb %mm3, %mm1;

		movq %mm0, %es:-16(%edi);    /* Write results back */
		movq %mm1, %es:-8(%edi);

		decl %ecx;

		jnz trans_sse_32_loop;
		
	trans_sse_32_rest_of_line:
	
		movl B_WIDTH, %ecx;
		andl $3, %ecx;
		jz trans_sse_32_end;
		
		shrl $1, %ecx;
		jnc trans_sse_32_two_pixels;
		
	trans_sse_32_one_pixel:
		movd (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
	    movd %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */
	    
	    addl $4, %esi;
	    addl $4, %edi;
	    
		movq %mm0, %mm4;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm2, %mm3;

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm2;

		psubw %mm2, %mm0;            /* source -= dest */
		movq %mm3, %mm2;

		pand %mm7, %mm5;             /* Split B */
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */

		psrlw $8, %mm4;
		pand %mm7, %mm2;

		psrlw $8, %mm2;
		psubw %mm2, %mm4;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		movq %mm3, %mm2;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		por  %mm4, %mm0;             /* Recombine components */
		paddb %mm2, %mm0;

		movd %mm0, %es:-4(%edi);    /* Write results back */

	trans_sse_32_two_pixels:
		shrl $1, %ecx;
		jz trans_sse_32_end;
	
		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */


		movq %mm0, %mm4;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm2, %mm3;

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm2;

		psubw %mm2, %mm0;            /* source -= dest */
		movq %mm3, %mm2;

		pand %mm7, %mm5;             /* Split B */
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */

		psrlw $8, %mm4;
		pand %mm7, %mm2;

		psrlw $8, %mm2;
		psubw %mm2, %mm4;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		movq %mm3, %mm2;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		por  %mm4, %mm0;             /* Recombine components */
		paddb %mm2, %mm0;

		movq %mm0, %es:-8(%edi);    /* Write results back */

	_align_;
	trans_sse_32_end:
	)
	
	jmp draw_trans_32_sse_end

	/****************
	 * Special code 
	 */
	_align_
	draw_trans_sse_32_half:

	BLIT_LOOP(draw_trans_sse_32_half, 4, 4,

		shrl $2, %ecx;
		jz trans_sse_32_half_rest_of_line;

	_align_;
	trans_sse_32_half_loop:

		prefetchnta 32(%esi);
		prefetcht0  %es:32(%edi);

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		addl $16, %esi;
		addl $16, %edi;

		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    pcmpeqw (rb_mask32), %mm1;
	    movq %mm2, %mm6;
	    movq %mm3, %mm7;
	    pand %mm0, %mm6;
	    pand %mm1, %mm7;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    pandn %mm5, %mm1;
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    por %mm7, %mm1;
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		
		movq %mm3, %mm6;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		pand %mm7, %mm1;            /* color1 & high_mask */
		pand %mm7, %mm3;            /* color2 & high_mask */
		
		psrlq $1, %mm1;             /* color1 >> 1 & high_mask */
		pand (low_mask_32), %mm6;   /* color1 & low_mask */

		psrlq $1, %mm3;             /* color2 >> 1 & high_mask */
		pand %mm4, %mm6;            /* color2 & low_mask */
		
		paddb %mm1, %mm3;
		paddb %mm3, %mm6;           /* %mm6 = second 2 pixels blended */

		movq %mm5, %es:-16(%edi);   /* Write results back */
		movq %mm6, %es:-8(%edi);

		decl %ecx;

		jnz trans_sse_32_half_loop;
		
	trans_sse_32_half_rest_of_line:
		movl B_WIDTH, %ecx;
		andl $3, %ecx;
		jz trans_sse_32_half_end;
		
		shrl $1, %ecx;
		jnc trans_sse_32_half_two_pixels;
		
	trans_sse_32_half_one_pixel:
		movd (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movd %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		addl $4, %esi;
		addl $4, %edi;

		movq %mm0, %mm4;
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		movd %mm5, %es:-4(%edi);   /* Write results back */
		
	trans_sse_32_half_two_pixels:
		shrl $1, %ecx;
		jnc trans_sse_32_half_end;
		
		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		movq %mm0, %mm4;
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		movq %mm5, %es:-4(%edi);   /* Write results back */

	_align_;
	trans_sse_32_half_end:
	)	
	
	_align_
	draw_trans_32_sse_end:
	
	emms

	restore_context(B_DEST);
	ret

#endif


#ifdef FBLEND_MMX

FUNC(fblend_trans_mmx_32)

	save_context();
   
	movl B_DEST, %edx
	movw BMP_SEG(%edx), %es       /* load destination segment */
	movw %ds, %bx                 /* save data segment selector */

	movl B_FACT, %ecx
	movd %ecx, %mm7
	punpcklwd %mm7, %mm7
	punpcklwd %mm7, %mm7          /* mm7 = | fact | fact | fact | fact | */
	movq %mm7, (fact32)

	cmpl $128, %ecx
	je draw_trans_mmx_32_half

	cmpl $127, %ecx
	je draw_trans_mmx_32_half

	BLIT_LOOP(draw_trans_mmx_32, 4, 4,

		shrl $2, %ecx;
		jz trans_mmx_32_rest_of_line;

	_align_;
	trans_mmx_32_loop:

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | */
	    
	    addl $16, %esi;
	    addl $16, %edi;
	    
		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    pcmpeqd (rb_mask32), %mm1;
	    movq %mm2, %mm6;
	    movq %mm3, %mm7;
	    pand %mm0, %mm6;
	    pand %mm1, %mm7;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    pandn %mm5, %mm1;
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */
	    por %mm7, %mm1;

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
		movq %mm2, (temp);
		movq %mm3, (temp + 8);

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm1;
		
		pand %mm6, %mm2;
		pand %mm6, %mm3;

		psubw %mm2, %mm0;            /* source -= dest */
		psubw %mm3, %mm1;
		
		movq (temp),     %mm2;
		movq (temp + 8), %mm3;

		pand %mm7, %mm5;             /* Split B */
		pand %mm7, %mm4;
		
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */
		pmullw (fact32), %mm1;

		psrlw $8, %mm4;
		psrlw $8, %mm5;

		pand %mm7, %mm2;
		pand %mm7, %mm3;

		psrlw $8, %mm2;
		psrlw $8, %mm3;

		psubw %mm2, %mm4;
		psubw %mm3, %mm5;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		pmullw (fact32), %mm5;
		
		movq (temp),   %mm2;
		movq (temp+8), %mm3;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		psrlw $8, %mm1;
		pand %mm7, %mm5;

		por  %mm4, %mm0;             /* Recombine components */
		por  %mm5, %mm1;

		paddb %mm2, %mm0;
		paddb %mm3, %mm1;

		movq %mm0, %es:-16(%edi);    /* Write results back */
		movq %mm1, %es:-8(%edi);

		decl %ecx;

		jnz trans_mmx_32_loop;
		
	trans_mmx_32_rest_of_line:
	
		movl B_WIDTH, %ecx;
		andl $3, %ecx;
		jz trans_mmx_32_end;
		
		shrl $1, %ecx;
		jnc trans_mmx_32_two_pixels;
		
	trans_mmx_32_one_pixel:
		movd (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
	    movd %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */
	    
	    addl $4, %esi;
	    addl $4, %edi;
	    
		movq %mm0, %mm4;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm2, %mm3;

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm2;

		psubw %mm2, %mm0;            /* source -= dest */
		movq %mm3, %mm2;

		pand %mm7, %mm5;             /* Split B */
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */

		psrlw $8, %mm4;
		pand %mm7, %mm2;

		psrlw $8, %mm2;
		psubw %mm2, %mm4;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		movq %mm3, %mm2;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		por  %mm4, %mm0;             /* Recombine components */
		paddb %mm2, %mm0;

		movd %mm0, %es:-4(%edi);    /* Write results back */

	trans_mmx_32_two_pixels:
		shrl $1, %ecx;
		jz trans_mmx_32_end;
	
		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | */
	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | */


		movq %mm0, %mm4;
		
	    pcmpeqd (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;            /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;              /* Set src to dest if equal to mask */

		movq (rb_mask32), %mm6;
		movq (g_mask32), %mm7;

		movq %mm0, %mm4;
		movq %mm2, %mm3;

		pand %mm6, %mm0;             /* Split RB */
		pand %mm6, %mm2;

		psubw %mm2, %mm0;            /* source -= dest */
		movq %mm3, %mm2;

		pand %mm7, %mm5;             /* Split B */
		pmullw (fact32), %mm0;       /* source.rb * fact / 256 */

		psrlw $8, %mm4;
		pand %mm7, %mm2;

		psrlw $8, %mm2;
		psubw %mm2, %mm4;

		pmullw (fact32), %mm4;       /* source.g * fact / 256 */
		movq %mm3, %mm2;

		psrlw $8, %mm0;
		pand %mm7, %mm4;

		por  %mm4, %mm0;             /* Recombine components */
		paddb %mm2, %mm0;

		movq %mm0, %es:-8(%edi);    /* Write results back */

	_align_;
	trans_mmx_32_end:
	)
	
	jmp draw_trans_32_mmx_end

	/****************
	 * Special code 
	 */
	_align_
	draw_trans_mmx_32_half:

	BLIT_LOOP(draw_trans_mmx_32_half, 4, 4,

		shrl $2, %ecx;
		jz trans_mmx_32_half_rest_of_line;

	_align_;
	trans_mmx_32_half_loop:

		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
		movq 8(%esi), %mm1;       /* mm1 = src2| rgb1 | rgb2 | rgb3 | rgb4 | */

	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:8(%edi), %mm3;   /* mm3 = dst2| rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		addl $16, %esi;
		addl $16, %edi;

		movq %mm0, %mm4;
		movq %mm1, %mm5;
		
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    pcmpeqw (rb_mask32), %mm1;
	    movq %mm2, %mm6;
	    movq %mm3, %mm7;
	    pand %mm0, %mm6;
	    pand %mm1, %mm7;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    pandn %mm5, %mm1;
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    por %mm7, %mm1;
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		
		movq %mm3, %mm6;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		pand %mm7, %mm1;            /* color1 & high_mask */
		pand %mm7, %mm3;            /* color2 & high_mask */
		
		psrlq $1, %mm1;             /* color1 >> 1 & high_mask */
		pand (low_mask_32), %mm6;   /* color1 & low_mask */

		psrlq $1, %mm3;             /* color2 >> 1 & high_mask */
		pand %mm4, %mm6;            /* color2 & low_mask */
		
		paddb %mm1, %mm3;
		paddb %mm3, %mm6;           /* %mm6 = second 2 pixels blended */

		movq %mm5, %es:-16(%edi);   /* Write results back */
		movq %mm6, %es:-8(%edi);

		decl %ecx;

		jnz trans_mmx_32_half_loop;
		
	trans_mmx_32_half_rest_of_line:
		movl B_WIDTH, %ecx;
		andl $3, %ecx;
		jz trans_mmx_32_half_end;
		
		shrl $1, %ecx;
		jnc trans_mmx_32_half_two_pixels;
		
	trans_mmx_32_half_one_pixel:
		movd (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movd %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		addl $4, %esi;
		addl $4, %edi;

		movq %mm0, %mm4;
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		movd %mm5, %es:-4(%edi);   /* Write results back */
		
	trans_mmx_32_half_two_pixels:
		shrl $1, %ecx;
		jnc trans_mmx_32_half_end;
		
		movq (%esi), %mm0;        /* mm0 = src | rgb1 | rgb2 | rgb3 | rgb4 | */
	    movq %es:(%edi), %mm2;    /* mm2 = dst | rgb1 | rgb2 | rgb3 | rgb4 | */
	    
		movq %mm0, %mm4;
	    pcmpeqw (rb_mask32), %mm0; /* Compare to mask */
	    movq %mm2, %mm6;
	    pand %mm0, %mm6;
	    pandn %mm4, %mm0;          /* Set to 0 what is eq to mask */
	    por %mm6, %mm0;            /* Set src to dest if equal to mask */
	    
		movq (high_mask_32), %mm7;
		movq %mm0, %mm4;
		movq %mm2, %mm5;
		
		pand %mm7, %mm0;            /* color1 & high_mask */
		pand %mm7, %mm2;            /* color2 & high_mask */
		
		psrlq $1, %mm0;             /* color1 & high_mask >> 1 */
		pand (low_mask_32), %mm5;   /* color1 & low_mask */

		psrlq $1, %mm2;             /* color2 & high_mask >> 1 */
		pand %mm4, %mm5;            /* color1 & color2 & low_mask */
		
		paddb %mm0, %mm2;           /* color1 + color2 (high) */
		movq %mm1, %mm4;
		paddb %mm2, %mm5;           /* %mm5 = first 2 pixels blended */

		movq %mm5, %es:-4(%edi);   /* Write results back */

	_align_;
	trans_mmx_32_half_end:
	)	
	
	_align_
	draw_trans_32_mmx_end:
	
	emms

	restore_context(B_DEST);
	ret


#endif

