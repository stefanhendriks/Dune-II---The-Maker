/* Extracts structure info from Allegro for the assembly routines 
 * The code in this file is under the Giftware license, as it was
 * copyed off Allegro. Please see the copyright notice that comes
 * with Allegro for details.
 * Allegro is available at http://alleg.sf.net/
 */

#ifndef ASMDEFS_INC
#define ASMDEFS_INC


#include "../../obj/asmdef.inc"


/* helper macro in case we ever need to change this */
#define _align_   .balign 8, 0x90


/* readable way to access arguments passed from C code */
#define ARG1      8(%ebp)
#define ARG2      12(%ebp)
#define ARG3      16(%ebp)
#define ARG4      20(%ebp)
#define ARG5      24(%ebp)
#define ARG6      28(%ebp)
#define ARG7      32(%ebp)
#define ARG8      36(%ebp)
#define ARG9      40(%ebp)
#define ARG10     44(%ebp)


/* Bank switching macros. These should be called with a pointer to the
 * bitmap structure in %edx, and the line number you want to access in
 * %eax. Registers will be unchanged, except %eax will return a pointer 
 * to the start of the selected scanline.
 */
#define WRITE_BANK()    call *BMP_WBANK(%edx)
#define READ_BANK()     call *BMP_RBANK(%edx)

#define UNWRITE_BANK()  movl BMP_VTABLE(%edx), %eax ; call *VTABLE_UNBANK(%eax)
#define UNREAD_BANK()   movl BMP_VTABLE(%edx), %eax ; call *VTABLE_UNBANK(%eax)


#endif   /* ifndef ASMDEFS_INC */

