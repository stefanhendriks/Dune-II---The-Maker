
#ifndef FBLEND_INC
#define FBLEND_INC


/* Macros to help save and restore context for the blenders */



/* Pushes the current context on the stack */
#define save_context()    \
	pushl %ebp;           \
	movl %esp, %ebp;      \
	pushl %edi;           \
	pushl %esi;           \
	pushl %ebx;           \
	pushw %es;            \
	pushw %fs;            \



/* Pops the context from the stack.
 * 
 * \param dest   Pointer to the destination bitmap to unlock (in)
 */
#define restore_context(dest) \
	popw %fs;             \
	popw %es;             \
	                      \
	movl dest, %edx;      \
	UNWRITE_BANK();       \
	                      \
	popl %ebx;            \
	popl %esi;            \
	popl %edi;            \
	movl %ebp, %esp;      \
	popl %ebp;            \



/* Loads the bitmap's segment into a register
 *
 * \param bitmap   The bitmap to get the segment from (in)
 * \param segment  The register to place the segment into (out)
 */
#define load_bitmap_segment(bitmap, segment) \
	movw BMP_SEG(bitmap), segment;



/* Loads the bitmap's line read address. The bitmap is locked if needed.
 *
 * \param name     Macro name to generate (in)
 * \param bitmap   Bitmap pointer (in)
 * \param x_offs   X offset in pixels (in)
 * \param shift    Bytes per pixel shift (1 << bpp) (in)
 * \param line     Line number to read from (in)
 * \param addr     Address to read from (out)
 *
 * Scratch regs: %eax
 */
#define load_bitmap_read_address(name, bitmap, x_offs, shift, line, addr) \
	movl BMP_ID(bitmap), %eax;      /* Load line address */              \
	andl $BMP_ID_MASK, %eax;                                             \
	orl %eax, %eax;                                                      \
	movl line, %eax;                                                     \
	jnz load_bitmap_read_address_video_##name;                           \
	                                                                     \
	movl BMP_LINE(bitmap, %eax, 4), %eax;                                \
	jmp load_bitmap_read_address_end_##name;                             \
	                                                                     \
_align_;                                                                 \
load_bitmap_read_address_video_##name:                                   \
	READ_BANK();                                                         \
	                                                                     \
_align_;                                                                 \
load_bitmap_read_address_end_##name:                                     \
	movl x_offs, addr;                                                   \
	leal (%eax, addr, shift), addr;                                      \



/* Loads the bitmap's line write address. The bitmap is locked if needed.
 *
 * \param name     Macro name to generate (in)
 * \param bitmap   Bitmap pointer (in)
 * \param x_offs   X offset in pixels (in)
 * \param shift    Bytes per pixel shift (1 << bpp) (in)
 * \param line     Line number to write to (in)
 * \param addr     Address to write to (out)
 *
 * Scratch regs: %eax
 */
#define load_bitmap_write_address(name, bitmap, x_offs, shift, line, addr) \
	movl BMP_ID(bitmap), %eax;      /* Load line address */              \
	andl $BMP_ID_MASK, %eax;                                             \
	orl %eax, %eax;                                                      \
	movl line, %eax;                                                     \
	jnz load_bitmap_write_address_video_##name;                          \
	                                                                     \
	movl BMP_LINE(bitmap, %eax, 4), %eax;                                \
	jmp load_bitmap_write_address_end_##name;                            \
	                                                                     \
_align_;                                                                 \
load_bitmap_write_address_video_##name:                                  \
	WRITE_BANK();                                                        \
	                                                                     \
_align_;                                                                 \
load_bitmap_write_address_end_##name:                                    \
	movl x_offs, addr;                                                   \
	leal (%eax, addr, shift), addr;                                      \


#define B_SOURCE        ARG1 
#define B_DEST          ARG2
#define B_SOURCE_X      ARG3
#define B_SOURCE_Y      ARG4
#define B_DEST_X        ARG5
#define B_DEST_Y        ARG6
#define B_WIDTH         ARG7
#define B_HEIGHT        ARG8
#define B_FACT          ARG9



/* Main bitmap blender loop. Takes care of loading addresses and
 * necessary context. Calls the user's code to do the actual blending
 * work.
 *
 * \param name   Macro to generate names for (in)
 * \param s_bpp  Source bits-per-pixels (in)
 * \param d_bpp  Destination bits-per-pixel (in)
 * \param code   The code that will do the blending. (in)
 *
 * When the user code is entered, the following resgiters hold:
 *  %ds:%eax - Destination buffer for blend data
 *  %fs:%esi - Source bitmap pointer
 *  %es:%edi - Destination bitmap pointer
 *      %ecx - Number of pixels to blend
 *      %edx - Source bitmap pointer
 */
#define BITMAP_BLEND_LOOP(name, s_bpp, d_bpp, code...)                   \
bitmap_blend_loop_##name:                                                \
	movl B_DEST, %edx;            /* destination bitmap */               \
	                                                                     \
	load_bitmap_read_address(blend_loop_1_##name, %edx, B_DEST_X, s_bpp, B_DEST_Y, %edi); \
                                                                         \
	movl B_DEST_X, %edi;          /* x offset */                         \
	leal (%eax, %edi, d_bpp), %edi;                                      \
	                                                                     \
	movl B_SOURCE, %edx;          /* source bitmap */                    \
	                                                                     \
	load_bitmap_read_address(blend_loop_2_##name, %edx, B_SOURCE_X, d_bpp, B_SOURCE_Y, %esi); \
	                                                                     \
	movl B_WIDTH, %ecx;           /* x loop counter */                   \
	                                                                     \
	init_buffer();                                                       \
	                                                                     \
	code;                         /* Blend */                            \
	                                                                     \
	movl B_DEST, %edx;            /* destination bitmap */               \
	movl B_WIDTH, %ecx;                                                  \
	copy_back_buffer(blend_copy_back_##name, %edx, B_DEST_X, d_bpp, B_DEST_Y, %ecx); \
	                                                                     \
	incl B_SOURCE_Y;                                                     \
	incl B_DEST_Y;                                                       \
	decl B_HEIGHT;                                                       \
	jg bitmap_blend_loop_##name;  /* and loop */



#define R_DEST          ARG1
#define R_X             ARG2
#define R_Y             ARG3
#define R_WIDTH         ARG4
#define R_HEIGHT        ARG5
#define R_COLOR         ARG6
#define R_FACT          ARG7



/* Main rectangular blender loop. Takes care of loading addresses and
 * necessary context. Calls the user's code to do the actual blending
 * work.
 *
 * \param name   Macro to generate names for (in)
 * \param bpp    Dest bits-per-pixels (in)
 * \param code   The code that will do the blending. (in)
 */
#define RECT_BLEND_LOOP(name, bpp, code...)                              \
rect_blend_loop_##name:                                                  \
	movl R_DEST, %edx             /* destination bitmap */               \
	                                                                     \
	load_bitmap_read_address(rect_loop_1_##name, %edx, R_X, bpp, R_Y, %edi); \
	                                                                     \
	movl R_WIDTH, %ecx;           /* x loop counter */                   \
	                                                                     \
	init_buffer();                                                       \
	                                                                     \
	code;                         /* Blend */                            \
	                                                                     \
	movl R_DEST, %edx             /* destination bitmap */               \
	movl R_WIDTH, %ecx;                                                  \
	copy_back_buffer(rect_copy_back_##name, %edx, R_X, bpp, R_Y, %ecx);  \
	                                                                     \
	incl R_Y                                                             \
	decl R_HEIGHT                                                        \
	jg rect_blend_loop_##name     /* and loop */



#define init_buffer()                                                    \
	leal (_fblend_static_buffer), %eax;                                  \



/* Copies the scratch buffer back to the destination bitmap.
 *
 * \param name   Macro to generate name for (in)
 * \param dest   Pointer to the destination bitmap (in)
 * \param x_offs X offset in pixels (in)
 * \param bpp    Bytes Per Pixel (bpp) (in)
 * \param line   Bitmap Line to write back to (in)
 * \param length Number of pixels to write back (in)
 *
 * Assumes: %es contains the destination bitmap segment.
 *
 * Scratch Regs: %edi, %esi, %eax
 */
#define copy_back_buffer(name, dest, x_offs, bpp, line, length)        \
	load_bitmap_write_address(copy_back_##name, dest, x_offs, bpp, line, %edi); \
	                                                                     \
	imull $bpp, length;                                                  \
	leal (_fblend_static_buffer), %esi;                                  \
	pushl length;                                                        \
	shrl $5, length;                                                     \
	                                                                     \
	orl length, length;                                                  \
	jz copy_back_##name##_rest_of_line;                                  \
	                                                                     \
	_align_;                                                             \
	copy_back_##name##_loop:                                             \
		movq (%esi),   %mm0;                                             \
		movq 8(%esi),  %mm1;                                             \
		movq 16(%esi), %mm2;                                             \
		movq 24(%esi), %mm3;                                             \
		addl $32, %esi;                                                  \
		movq %mm0, %es:(%edi);                                           \
		movq %mm1, %es:8(%edi);                                          \
		movq %mm2, %es:16(%edi);                                         \
		movq %mm3, %es:24(%edi);                                         \
		addl $32, %edi;                                                  \
		decl %ecx;                                                       \
		jnz copy_back_##name##_loop;                                     \
	                                                                     \
	_align_;                                                             \
	copy_back_##name##_rest_of_line:                                     \
		popl length;                                                     \
		andl $31, length;                                                \
	                                                                     \
		jz copy_back_##name##_end_line;                                  \
	                                                                     \
	_align_;                                                             \
	copy_back_##name##_sixteen_bytes:                                    \
		testl $16, length;                                               \
		jz copy_back_##name##_eight_bytes;                               \
		                                                                 \
		movq (%esi), %mm0;                                               \
		movq 8(%esi), %mm1;                                              \
		addl $16, %esi;                                                  \
		addl $16, %edi;                                                  \
		movq %mm0, %es:-16(%edi);                                        \
		movq %mm1, %es:-8(%edi);                                         \
		                                                                 \
	_align_;                                                             \
	copy_back_##name##_eight_bytes:                                      \
		testl $8, length;                                                \
		jz copy_back_##name##_four_bytes;                                \
		                                                                 \
		movq (%esi), %mm0;                                               \
		addl $8, %esi;                                                   \
		addl $8, %edi;                                                   \
		movq %mm0, %es:-8(%edi);                                         \
		                                                                 \
	_align_;                                                             \
	copy_back_##name##_four_bytes:                                       \
		testl $4, length;                                                \
		jz copy_back_##name##_two_bytes;                                 \
		                                                                 \
		movl (%esi), %eax;                                               \
		addl $4, %esi;                                                   \
		addl $4, %edi;                                                   \
		movl %eax, %es:-4(%edi);                                         \
		                                                                 \
	copy_back_##name##_two_bytes:                                        \
		testl $2, length;                                                \
		jz copy_back_##name##_one_byte;                                  \
		                                                                 \
		movzwl (%esi), %eax;                                             \
		addl $2, %esi;                                                   \
		addl $2, %edi;                                                   \
		movw %ax, %es:-2(%edi);                                          \
		                                                                 \
	copy_back_##name##_one_byte:                                         \
		testl $1, length;                                                \
		jz copy_back_##name##_end_line;                                  \
	                                                                     \
		movb (%esi), %al;                                                \
		incl %esi;                                                       \
		incl %edi;                                                       \
		movb %al, %es:-1(%edi);                                          \
	                                                                     \
		                                                                 \
	_align_;                                                             \
	copy_back_##name##_end_line:                                         \


#endif
