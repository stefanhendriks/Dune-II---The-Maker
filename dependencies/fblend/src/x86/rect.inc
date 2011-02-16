/* Extracts structure info from Allegro for the assembly routines 
 * The code in this file is under the Giftware license, as it was
 * copyed off Allegro. Please see the copyright notice that comes
 * with Allegro for details.
 * Allegro is available at http://alleg.sf.net/
 */

#ifndef RECT_INC
#define RECT_INC



/* generic framework for constructing blitting routines, shared 
 * between the 8, 16, 24, and 32 bit versions of the code...
 */


#define R_DEST          ARG1
#define R_X             ARG2
#define R_Y             ARG3
#define R_WIDTH         ARG4
#define R_HEIGHT        ARG5
#define R_COLOR         ARG6
#define R_FACT          ARG7



#define RECT_LOOP(name, bpp, code...)                                        \
rect_loop_##name:                                                          ; \
   movl R_DEST, %edx             /* destination bitmap */                  ; \
   movl R_Y, %eax                /* line number */                         ; \
   WRITE_BANK()                  /* select bank */                         ; \
   movl R_X, %edi                /* x offset */                            ; \
   leal (%eax, %edi, bpp), %edi                                            ; \
                                                                           ; \
   movl R_WIDTH, %ecx            /* x loop counter */                      ; \
   code                          /* do the transfer */                     ; \
                                                                           ; \
   incl R_Y                                                                ; \
   decl R_HEIGHT                                                           ; \
   jg rect_loop_##name           /* and loop */

#endif


#define RECT_TRANS_16_4_MMX(source, r_mask, g_mask, b_mask, fact, offset, scratch1, scratch2) \
	    movq source, scratch1;                                       \
	    movq source, scratch2;                                       \
	    pand r_mask, source;      /* Isolate each component */       \
	    pand g_mask, scratch1;                                       \
	    pand b_mask, scratch2;                                       \
                                                                     \
	    psrlw $5,  source;        /* Shift to free up some bits */   \
	    psrlw $5,  scratch1;                                         \
                                                                     \
		pmullw fact, scratch2;    /* Multiply by the factor */       \
		pmullw fact, source;                                         \
		pmullw fact, scratch1;                                       \
                                                                     \
		psrlw $5, scratch2;                                          \
                                                                     \
		pand r_mask, source;      /* Clean up */                     \
		pand g_mask, scratch1;                                       \
		pand b_mask, scratch2;                                       \
                                                                     \
		paddw offset, source;     /* Add the offset for trans */     \
		paddw offset, scratch1;                                      \
		paddw offset, scratch2;                                      \
                                                                     \
		pand r_mask, source;      /* Final clean up */               \
		pand g_mask, scratch1;                                       \
		pand b_mask, scratch2;                                       \
                                                                     \
		por scratch1, source;     /* Combine everything */           \
		por scratch2, source;                                        \


#define RECT_TRANS_32_8_MMX(source1, source2, rb_mask, g_mask, fact, offset, scratch1, scratch2) \
                                                                     \
	    movq source1, scratch1;   /* Separate components s1 */       \
	    pand rb_mask, source1;                                       \
	    pand g_mask,  scratch1;                                      \
                                                                     \
	    psrlw $8, scratch1;       /* Multiply s1 by the factor */    \
                                                                     \
		pmullw fact, source1;                                        \
		pmullw fact, scratch1;                                       \
                                                                     \
	    movq source2, scratch2;   /* Separate components s2 */       \
	    pand rb_mask, source2;                                       \
	    pand g_mask,  scratch2;                                      \
                                                                     \
		psrlw $8, source1;                                           \
		pand g_mask, scratch1;    /* Clean up s1 */                  \
		por scratch1, source1;                                       \
                                                                     \
	    psrlw $8, scratch2;       /* Multiply s2 by the factor */    \
                                                                     \
		pmullw fact, source2;                                        \
		pmullw fact, scratch2;                                       \
                                                                     \
		paddb offset, source1;    /* Add offset */                   \
                                                                     \
		psrlw $8, source2;        /* Clean up s2 */                  \
		pand g_mask, scratch2;                                       \
		por scratch2, source2;                                       \
                                                                     \
		paddb offset, source2;    /* Add offset */                   \


#define RECT_TRANS_32_4_MMX(source, rb_mask, g_mask, fact, offset, scratch) \
                                                                     \
	    movq source, scratch;     /* Separate components */          \
	    pand rb_mask, source;                                        \
	    pand g_mask,  scratch;                                       \
                                                                     \
	    psrlw $8, scratch;        /* Multiply by the factor */       \
                                                                     \
		pmullw fact, source;                                         \
		pmullw fact, scratch;                                        \
                                                                     \
		psrlw $8, source;         /* Clean up */                     \
		pand g_mask, scratch;                                        \
		por scratch, source;                                         \
                                                                     \
		paddb %mm7, %mm0;         /* Add offset */                   \



#define RECT_ADD_16_8_MMX(source1, source2, low_mask, msb_mask, src_low, src_msb, scratch1, scratch2, scratch3, scratch4) \
		movq source1, scratch1;   /* Prepare masks for dest color */ \
		pand low_mask, source1;                                      \
		pand msb_mask, scratch1;                                     \
                                                                     \
		paddw src_low, source1;   /* Add lower bits together */      \
		movq scratch1, scratch2;                                     \
                                                                     \
		por  src_msb, scratch1;                                      \
		pand src_msb, scratch2;                                      \
                                                                     \
		movq source2, scratch3;   /* Start next pack */              \
		movq source1, scratch4;                                      \
                                                                     \
		pand low_mask, source2;                                      \
		pand scratch1, source1;                                      \
                                                                     \
		por scratch2,  source1;                                      \
		pand msb_mask, scratch3;                                     \
                                                                     \
		psrlq $4, source1;                                           \
		paddw src_low, source2;    /* Add lower bits together */     \
                                                                     \
		paddw low_mask, source1;   /* Finish mask creation */        \
		pxor  low_mask, source1;                                     \
                                                                     \
		por  scratch1, source1;                                      \
		por  scratch4, source1;                                      \
                                                                     \
		movq scratch3, scratch1;                                     \
		pand (source_msb), scratch3;                                 \
		por  (source_msb), scratch1;                                 \
                                                                     \
		movq source2, scratch4;                                      \
		pand scratch1, source2;                                      \
                                                                     \
		por scratch3, source2;                                       \
		psrlq $4, source2;                                           \
		paddw low_mask, source2;                                     \
		movq (source_msb), src_msb;                                  \
		pxor  low_mask, source2;                                     \
                                                                     \
		por  scratch1, source2;                                      \
		por  scratch4, source2;                                      \


#define RECT_ADD_16_4_MMX(source, low_mask, msb_mask, src_low, src_msb, scratch1, scratch2, scratch3) \
		movq source, scratch1;     /* Prepare masks for dest color */\
		pand low_mask, source;                                       \
		pand msb_mask, scratch1;                                     \
                                                                     \
		paddw src_low, source;     /* Add lower bits together */     \
		movq scratch1, scratch2;                                     \
                                                                     \
		por  src_msb, scratch1;                                      \
		pand src_msb, scratch2;                                      \
                                                                     \
		movq source, scratch3;                                       \
                                                                     \
		pand scratch1, source;                                       \
                                                                     \
		por scratch2, source;                                        \
                                                                     \
		psrlq $4, source;                                            \
		paddw low_mask, source;    /* Finish mask creation */        \
		pxor low_mask, source;                                       \
                                                                     \
		por  scratch1, source;                                       \
		por  scratch3, source;                                       \

