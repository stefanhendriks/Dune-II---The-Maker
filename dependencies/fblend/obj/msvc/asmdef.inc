/* automatically generated structure offsets for use by asm code */

#ifndef ALLEGRO_WINDOWS
#define ALLEGRO_WINDOWS
#endif

#ifndef ALLEGRO_COLOR8
#define ALLEGRO_COLOR8
#endif

#ifndef ALLEGRO_COLOR16
#define ALLEGRO_COLOR16
#endif

#ifndef ALLEGRO_COLOR24
#define ALLEGRO_COLOR24
#endif

#ifndef ALLEGRO_COLOR32
#define ALLEGRO_COLOR32
#endif

#define BMP_W 0
#define BMP_H 4
#define BMP_CLIP 8
#define BMP_CL 12
#define BMP_CR 16
#define BMP_CT 20
#define BMP_CB 24
#define BMP_VTABLE 28
#define BMP_WBANK 32
#define BMP_RBANK 36
#define BMP_DAT 40
#define BMP_ID 44
#define BMP_EXTRA 48
#define BMP_XOFFSET 52
#define BMP_YOFFSET 56
#define BMP_SEG 60
#define BMP_LINE 64
#define NEWLINE 0
#ifndef BMP_ID_VIDEO
#define BMP_ID_VIDEO -2147483648
#define BMP_ID_SYSTEM 1073741824
#define BMP_ID_SUB 536870912
#define BMP_ID_PLANAR 268435456
#define BMP_ID_NOBLIT 134217728
#define BMP_ID_LOCKED 67108864
#define BMP_ID_AUTOLOCK 33554432
#define BMP_ID_MASK 33554431
#endif
#define NEWLINE 0
#define CMP_PLANAR 0
#define CMP_COLOR_DEPTH 2
#define CMP_DRAW 8
#define NEWLINE 0
#define VTABLE_COLOR_DEPTH 0
#define VTABLE_MASK_COLOR 4
#define VTABLE_UNBANK 8
#define NEWLINE 0
#define RLE_W 0
#define RLE_H 4
#define RLE_DAT 16
#define NEWLINE 0
#define GFXRECT_WIDTH 0
#define GFXRECT_HEIGHT 4
#define GFXRECT_PITCH 8
#define GFXRECT_DATA 12
#define NEWLINE 0
#define DRAW_SOLID 0
#define DRAW_XOR 1
#define DRAW_COPY_PATTERN 2
#define DRAW_SOLID_PATTERN 3
#define DRAW_MASKED_PATTERN 4
#define DRAW_TRANS 5
#define NEWLINE 0
#ifndef MASK_COLOR_8
#define MASK_COLOR_8 0
#define MASK_COLOR_15 31775
#define MASK_COLOR_16 63519
#define MASK_COLOR_24 16711935
#define MASK_COLOR_32 16711935
#endif
#define NEWLINE 0
#define POLYSEG_U 0
#define POLYSEG_V 4
#define POLYSEG_DU 8
#define POLYSEG_DV 12
#define POLYSEG_C 16
#define POLYSEG_DC 20
#define POLYSEG_R 24
#define POLYSEG_G 28
#define POLYSEG_B 32
#define POLYSEG_DR 36
#define POLYSEG_DG 40
#define POLYSEG_DB 44
#define POLYSEG_Z 48
#define POLYSEG_DZ 52
#define POLYSEG_FU 56
#define POLYSEG_FV 60
#define POLYSEG_DFU 64
#define POLYSEG_DFV 68
#define POLYSEG_TEXTURE 72
#define POLYSEG_UMASK 76
#define POLYSEG_VMASK 80
#define POLYSEG_VSHIFT 84
#define POLYSEG_SEG   88
#define POLYSEG_ZBADDR 92
#define POLYSEG_RADDR 96
#define NEWLINE 0
#define ERANGE 34
#define NEWLINE 0
#define M_V00 0
#define M_V01 4
#define M_V02 8
#define M_V10 12
#define M_V11 16
#define M_V12 20
#define M_V20 24
#define M_V21 28
#define M_V22 32
#define NEWLINE 0
#define M_T0 36
#define M_T1 40
#define M_T2 44
#define NEWLINE 0
#define FSEG
#define NEWLINE 0
#define FUNC(name)            .globl _##name ; _align_ ; _##name:
#define GLOBL(name)           _##name
#define NEWLINE 0
