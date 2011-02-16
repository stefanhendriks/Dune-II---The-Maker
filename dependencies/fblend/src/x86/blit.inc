/* Extracts structure info from Allegro for the assembly routines 
 * The code in this file is under the Giftware license, as it was
 * copyed off Allegro. Please see the copyright notice that comes
 * with Allegro for details.
 * Allegro is available at http://alleg.sf.net/
 */

#ifndef BLIT_INC
#define BLIT_INC



/* generic framework for constructing blitting routines, shared 
 * between the 8, 16, 24, and 32 bit versions of the code...
 */


#define B_SOURCE        ARG1 
#define B_DEST          ARG2
#define B_SOURCE_X      ARG3
#define B_SOURCE_Y      ARG4
#define B_DEST_X        ARG5
#define B_DEST_Y        ARG6
#define B_WIDTH         ARG7
#define B_HEIGHT        ARG8
#define B_FACT          ARG9


#define BLIT_LOOP(name, s_bpp, d_bpp, code...)                           \
blit_loop_##name:                                                        \
	movl B_DEST, %edx;            /* destination bitmap */               \
	movl B_DEST_Y, %eax;          /* line number */                      \
	                                                                     \
	movl BMP_ID(%edx), %eax;      /* Load line address */                \
	andl $BMP_ID_MASK, %eax;                                             \
	orl %eax, %eax;                                                      \
	movl B_DEST_Y, %eax;                                                 \
	jnz blit_loop_video_##name;                                          \
	                                                                     \
	movl BMP_LINE(%edx, %eax, 4), %eax;                                  \
	jmp blit_loop_continue_##name;                                       \
	                                                                     \
_align_;                                                                 \
blit_loop_video_##name:                                                  \
	WRITE_BANK();                                                        \
	                                                                     \
_align_;                                                                 \
blit_loop_continue_##name:                                               \
	movl B_DEST_X, %edi;          /* x offset */                         \
	leal (%eax, %edi, d_bpp), %edi;                                      \
	                                                                     \
	movl B_SOURCE, %edx;         /* source bitmap */                     \
	                                                                     \
	movl BMP_ID(%edx), %eax;      /* Load line address */                \
	andl $BMP_ID_MASK, %eax;                                             \
	orl %eax, %eax;                                                      \
	movl B_SOURCE_Y, %eax;       /* line number */                       \
	jnz blit_loop_video2_##name;                                         \
	                                                                     \
	movl BMP_LINE(%edx, %eax, 4), %eax;                                  \
	jmp blit_loop_continue2_##name;                                      \
	                                                                     \
_align_;                                                                 \
blit_loop_video2_##name:                                                 \
	READ_BANK();                                                         \
	                                                                     \
_align_;                                                                 \
blit_loop_continue2_##name:                                              \
	movl B_SOURCE_X, %esi;       /* x offset */                          \
	leal (%eax, %esi, s_bpp), %esi;                                      \
	                                                                     \
   movl B_WIDTH, %ecx            /* x loop counter */                      ; \
   movw BMP_SEG(%edx), %ds       /* load data segment */                   ; \
   code                          /* do the transfer */                     ; \
									   ; \
   movw %bx, %ds                 /* restore data segment */                ; \
   incl B_SOURCE_Y                                                         ; \
   incl B_DEST_Y                                                           ; \
   decl B_HEIGHT                                                           ; \
   jg blit_loop_##name           /* and loop */



#endif
