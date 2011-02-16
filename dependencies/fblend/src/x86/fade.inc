#ifndef FADE_INC
#define FADE_INC


/* generic framework for constructing blitting routines, shared 
 * between the 8, 16, 24, and 32 bit versions of the code...
 */


#define F_SOURCE        ARG1 
#define F_DEST          ARG2
#define F_SOURCE_X      ARG3
#define F_SOURCE_Y      ARG4
#define F_DEST_X        ARG5
#define F_DEST_Y        ARG6
#define F_WIDTH         ARG7
#define F_HEIGHT        ARG8
#define F_COLOR         ARG9
#define F_FACT         ARG10

/* Macro compatibility */
#define B_DEST          ARG2



#define FADE_LOOP(name, s_bpp, d_bpp, code...)                               \
fade_loop_##name:                                                          ; \
   movl F_DEST, %edx             /* destination bitmap */                  ; \
   movl F_DEST_Y, %eax           /* line number */                         ; \
   WRITE_BANK()                  /* select bank */                         ; \
   movl F_DEST_X, %edi           /* x offset */                            ; \
   leal (%eax, %edi, d_bpp), %edi                                          ; \
									   ; \
   movl F_SOURCE, %edx           /* source bitmap */                       ; \
   movl F_SOURCE_Y, %eax         /* line number */                         ; \
   READ_BANK()                   /* select bank */                         ; \
   movl F_SOURCE_X, %esi         /* x offset */                            ; \
   leal (%eax, %esi, s_bpp), %esi                                          ; \
									   ; \
   movl F_WIDTH, %ecx            /* x loop counter */                      ; \
   movw BMP_SEG(%edx), %ds       /* load data segment */                   ; \
   code                          /* do the transfer */                     ; \
									   ; \
   movw %bx, %ds                 /* restore data segment */                ; \
   incl F_SOURCE_Y                                                         ; \
   incl F_DEST_Y                                                           ; \
   decl F_HEIGHT                                                           ; \
   jg fade_loop_##name           /* and loop */



#define FADE_16_8_SSE2(source, rbg, grb, fact, color, scratch1, scratch2, scratch3)  \
	movq source, scratch1;         /* Make a copy of the source */       \
	pand rbg, source;              /* Mask | R.B | .G. | R.B | .G. | */  \
	pand grb, scratch1;            /* Mask | .G. | R.B | .G. | R.B | */  \
	                                                                     \
	movq source,   scratch2;       /* Store copies of the masked source */ \
	movq scratch1, scratch3;                                             \
	                                                                     \
	psrlw $5, source;              /* Multiply lower masks by factor */  \
	                                                                     \
	pmuludq fact, scratch1;                                              \
	pmuludq fact, source;                                                \
	                                                                     \
	psrlq $37, scratch2;           /* Shift down upper masks for multiply */ \
	psrlq $32, scratch3;                                                 \
	                                                                     \
	pmuludq fact, scratch2;         /* Multiply upper masks by factor */ \
	pmuludq fact, scratch3;                                              \
	                                                                     \
	psrlw $5, scratch1;                                                  \
	pand rbg, source;              /* Clean up lower masks */            \
	                                                                     \
	pand grb, scratch1;                                                  \
	paddw color, source;           /* Add the color */                   \
	                                                                     \
	paddw color, scratch1;                                               \
	psllq $32, scratch2;           /* Place back the upper masks */      \
	                                                                     \
	pand rbg, source;              /* Clean up lower masks */            \
	psllq $27, scratch3;                                                 \
	                                                                     \
	pand grb, scratch1;                                                  \
	                                                                     \
	por scratch1, source;          /* Combine lower masks together */    \
	pand rbg, scratch2;            /* Clean up upper masks */            \
	                                                                     \
	pand grb, scratch3;                                                  \
	paddw color, scratch2;         /* Add the color */                   \
	                                                                     \
	pand rbg, scratch2;            /* Clean up upper masks */            \
	paddw color, scratch3;                                               \
	                                                                     \
	pand grb, scratch3;                                                  \
	por scratch2, source;          /* Recombine everything */            \
	                                                                     \
	por scratch3, source;                                                \
/* Done! - 20 cycles for 8 pixels */


#define FADE_16_4_MMX(source, rmask, gmask, bmask, fact, color, scratch1, scratch2, scratch3, scratch4) \
	movq source, scratch1;                                               \
	movq source, scratch2;                                               \
	                                                                     \
	pand rmask, scratch1;        /* Mask both Red and Green */           \
	pand gmask, scratch2;                                                \
	                                                                     \
	psrlw $5, scratch1;          /* Shift and multiply by the factor */  \
	movq color, scratch3;                                                \
	                                                                     \
	psrlw $5, scratch2;                                                  \
	                                                                     \
	pmullw fact, scratch1;                                               \
	pmullw fact, scratch2;                                               \
	                                                                     \
	movq scratch3, scratch4;                                             \
	pand rmask, scratch3;        /* Prepare to add color offset */       \
	pand gmask, scratch4;                                                \
	                                                                     \
	paddw scratch3, scratch1;    /* Add color offset */                  \
	paddw scratch4, scratch2;                                            \
	                                                                     \
	movq bmask, scratch3;        /* Prepare blue mask */                 \
	pand rmask, scratch1;        /* Clean up Red and Green */            \
	pand gmask, scratch2;                                                \
	                                                                     \
	por scratch2, scratch1;      /* Recombine Red and Green */           \
	pand scratch3, source;                                               \
	                                                                     \
	pmullw fact, source;         /* Multiply-Shift Blue */               \
	movq color, scratch4;                                                \
	                                                                     \
	pand scratch3, scratch4;     /* Prepare blue offset */               \
	psrlw $5, source;                                                    \
	                                                                     \
	paddw scratch4, source;      /* Add offset */                        \
	                                                                     \
	pand scratch3, source;       /* Clean up blue */                     \
	                                                                     \
	por scratch1, source;        /* Recombine all components */          \
/* Done! - 23 cycles for 4 pixels 5.75 cycles/pixel */


#define FADE_32_4_MMX(source1, source2, rb_mask, g_mask, fact, color, scratch1, scratch2) \
	movq source1, scratch1;       /* Split up source components */       \
	movq source2, scratch2;                                              \
	pand g_mask, source1;                                                \
	pand rb_mask, scratch1;                                              \
	pand g_mask, source2;                                                \
	pand rb_mask, scratch2;                                              \
	                                                                     \
	psrlw $8, source1;            /* Multiply by factor */               \
	pmullw fact, scratch1;                                               \
	psrlw $8, source2;                                                   \
	pmullw fact, scratch2;                                               \
	pmullw fact, source1;                                                \
	pmullw fact, source2;                                                \
	                                                                     \
	psrlw $8, scratch1;                                                  \
	psrlw $8, scratch2;                                                  \
	                                                                     \
	pand g_mask, source1;                                                \
	pand g_mask, source2;                                                \
	                                                                     \
	por scratch1, source1;        /* Recombine components */             \
	por scratch2, source2;                                               \
	                                                                     \
	paddb color, source1;        /* Add offset */                        \
	paddb color, source2;                                                \
/* Done! - 15 cycles for 4 pixels, 3.75 cycles/pixel */


#endif
