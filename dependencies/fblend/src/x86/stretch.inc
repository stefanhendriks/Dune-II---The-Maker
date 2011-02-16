
#ifndef STRETCH_INC
#define STRETCH_INC



#define S_SOURCE        ARG1 
#define S_DEST          ARG2
#define S_SOURCE_X      ARG3
#define S_SOURCE_Y      ARG4
#define S_DEST_X        ARG5
#define S_DEST_Y        ARG6
#define S_SOURCE_WIDTH  ARG7
#define S_SOURCE_HEIGHT ARG8
#define S_DEST_WIDTH    ARG9
#define S_DEST_HEIGHT   ARG10


#define STRETCH_LOOP(name, bpp, code...)                                     \
stretch_loop_##name:                                                       ; \
   movl S_DEST, %edx             /* destination bitmap */                  ; \
   movl S_DEST_Y, %eax           /* line number */                         ; \
   WRITE_BANK()                  /* select bank */                         ; \
   movl S_DEST_X, %edi           /* x offset */                            ; \
   leal (%eax, %edi, bpp), %edi                                            ; \
                                                                           ; \
   movl S_SOURCE, %edx           /* source bitmap */                       ; \
   movl S_SOURCE_Y, %eax         /* line number */                         ; \
   READ_BANK()                   /* select bank */                         ; \
   movl S_SOURCE_X, %esi         /* x offset */                            ; \
   leal (%eax, %esi, bpp), %esi                                            ; \
                                                                           ; \
   movl S_SOURCE_WIDTH, %ecx     /* x loop counter */                      ; \
   movw BMP_SEG(%edx), %ds       /* load data segment */                   ; \
   code                          /* do the transfer */                     ; \
                                                                           ; \
   movw %bx, %ds                 /* restore data segment */                ; \
                                                                           ; \
   popl %eax                     /* Incr source y only half the time */    ; \
   addl %eax, S_SOURCE_Y                                                   ; \
   xorl $1, %eax                                                           ; \
   pushl %eax                                                              ; \
                                                                           ; \
   incl S_DEST_Y                                                           ; \
   decl S_DEST_HEIGHT                                                      ; \
   jnz  stretch_loop_##name


#endif
