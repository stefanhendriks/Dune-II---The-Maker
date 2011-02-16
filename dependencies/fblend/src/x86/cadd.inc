
#ifndef CADD_INC
#define CADD_INC


/** Zeroes the input vector component if it matches the mask.
 *  Otherwise, sets the source to all 1s.
 *  Works on 2 MMX registers, packed 16-bit data.
 */
#define ZERO_ON_EQUAL_16_8_MMX(source1, source2, mask, scratch1, scratch2) \
		movq source1, scratch1;                                          \
		movq source2, scratch2;                                          \
                                                                         \
		pcmpeqw mask, source1;     /* Compare to mask */                 \
		pcmpeqw mask, source2;                                           \
                                                                         \
		pandn scratch1, source1;   /* Set to 0 if eq to mask */          \
		pandn scratch2, source2;		                                 \

/** Zeroes the input vector component if it matches the mask.
 *  Otherwise, sets the source to all 1s.
 *  Works on 1 MMX register, packed 16-bit data.
 */
#define ZERO_ON_EQUAL_16_4_MMX(source1, mask, scratch1)                  \
		movq source1, scratch1;                                          \
		pcmpeqw mask, source1;     /* Compare to mask */                 \
		pandn scratch1, source1;   /* Set to 0 if eq to mask */          \


/** Multiplies the input vector's RGB components by some constant
 *  factor vector and shifts right by 5. Pass the masks of each R,
 *  G and B components.
 *  Result is stored back into the source.
 *
 *  This operation is separated into two steps to allow interleaving of
 *  other ops.
 *
 *  Works on 2 MMX registers, packed 16-bit data.
 */
#define	MULTIPLY_BY_FACTOR_16_8_MMX_1(source1, source2, factor, rb_mask, g_mask, scratch1, scratch2, scratch3, scratch4, scratch5) \
		movq rb_mask, scratch3;                                          \
		movq source1, scratch1;    /* Save the old values */             \
		movq source2, scratch2;                                          \
		movq source1, scratch4;                                          \
		movq source2, scratch5;                                          \
                                                                         \
		pand scratch3, scratch1;   /* Mask red/blue components */        \
		pand scratch3, scratch2;                                         \
                                                                         \
		pmullw factor, scratch1;   /* Multiply by the factor */          \
		pmullw factor, scratch2;                                         \
                                                                         \
		pand scratch3, scratch4;                                         \
		pand scratch3, scratch5;                                         \
		psrlw $5, scratch4;                                              \
		psrlw $5, scratch5;                                              \
                                                                         \
		pmullw factor, scratch4;                                         \
		pmullw factor, scratch5;                                         \
                                                                         \
		pand g_mask, source1;    /* Mask green component */              \
		pand g_mask, source2;                                            \
                                                                         \
		psrlw $5, scratch1;                                              \
		psrlw $5, scratch2;                                              \
                                                                         \
		por scratch4, scratch1;                                          \
		por scratch5, scratch2;                                          \
                                                                         \
		pand scratch3, scratch1;   /* Mask red/blue components */        \
		pand scratch3, scratch2;                                         \
                                                                         \
		movq g_mask, scratch3;                                           \
                                                                         \
		psrlw $5, source1;         /* Multiply by the factor */          \
		psrlw $5, source2;                                               \
                                                                         \
		pmullw factor, source1;                                          \
		pmullw factor, source2;                                          \


#define	MULTIPLY_BY_FACTOR_16_8_MMX_2(source1, source2, factor, rb_mask, g_mask, scratch1, scratch2, scratch3) \
		pand scratch3, source1;    /* Mask green component */            \
		pand scratch3, source2;                                          \
                                                                         \
		por scratch1, source1;     /* Finish component recombination */  \
		por scratch2, source2;                                           \
/* Done! 24 cycles / 8 pixels */


/** Multiplies the input vector's RGB components by some constant
 *  factor vector and shifts right by 5. Pass the masks of each R,
 *  G and B components.
 *  Result is stored back into the source.
 *
 *  Works on 1 MMX register, packed 16-bit data.
 */
#define	MULTIPLY_BY_FACTOR_16_4_MMX(source1, fact, rb_mask, g_mask, scratch1, scratch2) \
		movq source1, scratch1;    /* Save the old values */             \
		movq source1, scratch2;                                          \
		pand rb_mask, scratch1;    /* Mask red and blue components */    \
		pand rb_mask, scratch2;                                          \
                                                                         \
		psrlw $5, scratch1;        /* Multiply by the factor */          \
		pmullw fact, scratch2;                                           \
		pmullw fact, scratch1;                                           \
                                                                         \
		pand g_mask, source1;      /* Mask green component */            \
                                                                         \
		psrlw $5, scratch2;                                              \
                                                                         \
		por scratch2, scratch1;                                          \
		pmullw fact, source1;      /* Multiply by factor */              \
                                                                         \
		pand rb_mask, scratch1;                                          \
		psrlw $5, source1;                                               \
                                                                         \
		pand g_mask, source1;                                            \
		por scratch1, source1;     /* Finish component recombination */  \



/** Adds two input vectors with packed saturation. The msb_mask
 *  indicates where to saturate. The algorithm works by
 *  manually computing the carries of the sum of the lower bits, then
 *  figuring out if overflow occured. If overflow occurs, then the
 *  corresponding component is set to all 1s.
 *  Only works on powers-of-2 for saturating.
 *
 *  The C code for the algorithm is outlined below:
 *
 *  Same as ADD_SATURATE_16_8_MMX, but deals with only one input vector
 *  of 4 pixels
 *
 *	int msb_x = x & msb;
 *	int msb_y = y & msb;
 *	
 *	int sum = (x ^ msb_x) + (y ^ msb_y);
 *	
 *	int p = msb_x | msb_y;
 *	int g = msb_x & msb_y;
 *	int c = p & sum;
 *	
 *	int overflow = (c | g) >> 4;
 *	
 *	int result = ((msb - overflow) ^ msb) | sum | p;
 */
#define ADD_SATURATE_16_8_MMX(source1, source2, dest1, dest2, msb_mask, scratch1, scratch2, scratch3) \
	movq source1, scratch1;                                              \
	movq dest1,   scratch2;                                              \
	pand msb_mask, scratch1;       /* msb_x = x & msb */                 \
	pand msb_mask, scratch2;       /* msb_y = y & msb */                 \
	pxor scratch1, source1;        /* x ^ msb_x */                       \
	pxor scratch2, dest1;          /* y ^ msb_y */                       \
	paddw dest1,  source1;         /* sum = (x ^ msb_x) + (y ^ msb_y) */ \
	                                                                     \
	movq scratch2, dest1;                                                \
	por  scratch1, scratch2;       /* p = msb_x | msb_y */               \
	pand dest1,    scratch1;       /* g = msb_x & msb_y */               \
	movq scratch2, dest1;                                                \
	pand source1, scratch2;        /* c = p & sum */                     \
	                                                                     \
	por scratch2, scratch1;        /* c | g */                           \
	movq msb_mask, scratch2;                                             \
	psrlq $4, scratch1;            /* overflow = (c | g) >> 4 */         \
	movq source2, scratch3;                                              \
	psubw scratch1, scratch2;      /* msb - overflow */                  \
	por   source1, dest1;          /* sum | p */                         \
	pxor msb_mask, scratch2;       /* (msb - overflow) ^ msb */          \
	movq dest2, scratch1;                                                \
	por scratch2, dest1;           /* result = ((msb - overflow ^ msb) | sum | p */ \
	                                                                     \
	pand msb_mask, scratch3;       /* msb_x = x & msb */                 \
	pand msb_mask, scratch1;       /* msb_y = y & msb */                 \
	pxor scratch3, source2;        /* x ^ msb_x */                       \
	pxor scratch1, dest2;          /* y ^ msb_y */                       \
	paddw dest2,   source2;        /* sum = (x ^ msb_x) + (y ^ msb_y) */ \
	                                                                     \
	movq scratch1, dest2;                                                \
	por  scratch3, scratch1;       /* p = msb_x | msb_y */               \
	pand dest2,    scratch3;       /* g = msb_x & msb_y */               \
	movq scratch1, dest2;                                                \
	pand source2,  scratch1;       /* c = p & sum */                     \
	                                                                     \
	por scratch1, scratch3;        /* c | g */                           \
	movq msb_mask, scratch1;                                             \
	psrlq $4, scratch3;            /* overflow = (c | g) >> 4 */         \
	psubw scratch3, scratch1;      /* msb - overflow */                  \
	por   source2, dest2;          /* sum | p */                         \
	pxor msb_mask, scratch1;       /* (msb - overflow) ^ msb */          \
	por scratch1, dest2;           /* result = ((msb - overflow ^ msb) | sum | p */ \
/* Done! - 20 cycles / 8 pixels, or 2.50 cycles/pixel */



/** Same as ADD_SATURATE_16_8_MMX, but deals with only one input vector
 *  of 4 pixels
 *
 *	int msb_x = x & msb;
 *	int msb_y = y & msb;
 *	
 *	int sum = (x ^ msb_x) + (y ^ msb_y);
 *	
 *	int p = msb_x | msb_y;
 *	int g = msb_x & msb_y;
 *	int c = p & sum;
 *	
 *	int overflow = (c | g) >> 4;
 *	
 *	int result = ((msb - overflow) ^ msb) | sum | p;
 *	
 */
#define ADD_SATURATE_16_4_MMX(source, dest, msb_mask, scratch1, scratch2) \
	movq source, scratch1;                                               \
	movq dest,   scratch2;                                               \
	pand msb_mask, scratch1;       /* msb_x = x & msb */                 \
	pand msb_mask, scratch2;       /* msb_y = y & msb */                 \
	pxor scratch1, source;         /* x ^ msb_x */                       \
	pxor scratch2, dest;           /* y ^ msb_y */                       \
	paddw dest,  source;           /* sum = (x ^ msb_x) + (y ^ msb_y) */ \
	                                                                     \
	movq scratch2, dest;                                                 \
	por  scratch1, scratch2;       /* p = msb_x | msb_y */               \
	pand dest,     scratch1;       /* g = msb_x & msb_y */               \
	movq scratch2, dest;                                                 \
	pand source, scratch2;         /* c = p & sum */                     \
	                                                                     \
	por scratch2, scratch1;        /* c | g */                           \
	movq msb_mask, scratch2;                                             \
	psrlq $4, scratch1;            /* overflow = (c | g) >> 4 */         \
	psubw scratch1, scratch2;      /* msb - overflow */                  \
	por   source, dest;            /* sum | p */                         \
	pxor msb_mask, scratch2;       /* (msb - overflow) ^ msb */          \
	por scratch2, dest;            /* result = ((msb - overflow ^ msb) | sum | p */ \
/* Done! - 11 cycles / 4 pixels, or 2.75 cycles/pixel */



/** Subtracts two input vectors from each other, with packed saturation.
 *  The msb_mask indicates how to saturate.
 *  The algorithm works by manually computing the carries of the
 *  difference of the lower bits, then figuring out if underflow
 *  occured. If underflow occurs, then the corresponding component
 *  is set to all 0s.
 *  Only works on powers-of-2 for saturating.
 *
 *  The C code for the algorithm is outlined below:
 *
 *	int msb_x = (x & msb) ^ msb;
 *	int msb_y =  y & msb;
 *	int diff = (x | msb) - (y ^ msb_y);
 *	
 *	int p = msb_x | msb_y;
 *	int g = msb_x & msb_y;
 *	int c = p & (diff ^ msb);
 *	
 *	int overflow = (c | g) >> 4;
 *	int mask = ((msb - overflow) ^ msb);
 *	
 *	int result = ~mask & (diff ^ p);
 */
#define SUB_SATURATE_16_4_MMX(source, dest, msb_mask, scratch1, scratch2) \
	movq source, scratch1;                                               \
	movq dest,   scratch2;                                               \
	pand msb_mask, scratch1;       /* x & msb */                         \
	pand msb_mask, scratch2;       /* msb_y = y & msb */                 \
	por  msb_mask, source;         /* x | msb */                         \
	pxor scratch2, dest;           /* y ^ msb_y */                       \
	pxor msb_mask, scratch1;       /* msb_x = (x & msb) ^ msb */         \
	psubw dest,    source;         /* diff = (x | msb) - (y ^ msb_y) */  \
	                                                                     \
	movq scratch1, dest;                                                 \
	por  scratch2, scratch1;       /* p = msb_x | msb_y */               \
	pand dest,     scratch2;       /* g = msb_x & msb_y */               \
	                                                                     \
	pxor msb_mask, source;         /* diff ^= msb */                     \
	movq scratch1, dest;                                                 \
	pand source, scratch1;         /* c = p & (diff ^ msb) */            \
	pxor msb_mask, source;         /* diff ^= msb - restore diff */      \
	                                                                     \
	por scratch2, scratch1;                                              \
	movq msb_mask, scratch2;                                             \
	psrlq $4,     scratch1;        /* overflow = (c | g) >> 4 */         \
	psubw scratch1, scratch2;      /* msb - overflow */                  \
	pcmpeqw scratch1, scratch1;                                          \
	pxor msb_mask, scratch2;       /* mask = ((msb - overflow) ^ msb) */ \
	pxor scratch1, scratch2;       /* ~mask */                           \
	pxor source, dest;             /* diff ^ p */                        \
	pand scratch2, dest;           /* result = ~mask & (diff ^ p) */     \
/* Done! - Total: 13 cycles / 4 pixels, or 3.25 cycles/pixel */



/** Same as SUB_SATURATE_16_4_MMX, but deals with two input vectors
 *  of 4 pixels each
 *
 *  The C code for the algorithm is outlined below:
 *
 *	int msb_x = (x & msb) ^ msb;
 *	int msb_y =  y & msb;
 *	int diff = (x | msb) - (y ^ msb_y);
 *	
 *	int p = msb_x | msb_y;
 *	int g = msb_x & msb_y;
 *	int c = p & (diff ^ msb);
 *	
 *	int overflow = (c | g) >> 4;
 *	int mask = ((msb - overflow) ^ msb);
 *	
 *	int result = ~mask & (diff ^ p);
 */
#define SUB_SATURATE_16_8_MMX(source1, source2, dest1, dest2, msb_mask, scratch1, scratch2, scratch3) \
	movq source1, scratch1;                                              \
	movq dest1,   scratch2;                                              \
	pand msb_mask, scratch1;                                             \
	pand msb_mask, scratch2;       /* msb_y = y & msb */                 \
	por  msb_mask, source1;        /* x | msb */                         \
	pxor scratch2, dest1;          /* y ^ msb_y */                       \
	pxor msb_mask, scratch1;       /* msb_x = (x & msb) ^ msb */         \
	psubw dest1,   source1;        /* diff = (x | msb) - (y ^ msb_y) */  \
	                                                                     \
	movq scratch1, dest1;                                                \
	por  scratch2, scratch1;       /* p = msb_x | msb_y */               \
	pand dest1,    scratch2;       /* g = msb_x & msb_y */               \
	                                                                     \
	pxor msb_mask, source1;        /* diff ^= msb */                     \
	movq scratch1, dest1;                                                \
	pand source1, scratch1;        /* c = p & (diff ^ msb) */            \
	pxor msb_mask, source1;        /* diff ^= msb - restore diff */      \
	                                                                     \
	por scratch2, scratch1;                                              \
	movq msb_mask, scratch2;                                             \
	psrlq $4,     scratch1;        /* overflow = (c | g) >> 4 */         \
	psubw scratch1, scratch2;      /* msb - overflow */                  \
	pcmpeqw scratch1, scratch1;                                          \
	pxor msb_mask, scratch2;       /* mask = ((msb - overflow) ^ msb) */ \
	movq source2, scratch3;                                              \
	pxor scratch1, scratch2;       /* ~mask */                           \
	pxor source1, dest1;           /* diff ^ p */                        \
	movq dest2,   scratch1;                                              \
	pand scratch2, dest1;          /* result = ~mask & (diff ^ p) */     \
	                                                                     \
	pand msb_mask, scratch3;                                             \
	pand msb_mask, scratch1;       /* msb_y = y & msb */                 \
	por  msb_mask, source2;        /* x | msb */                         \
	pxor scratch1, dest2;          /* y ^ msb_y */                       \
	pxor msb_mask, scratch3;       /* msb_x = (x & msb) ^ msb */         \
	psubw dest2,   source2;        /* diff = (x | msb) - (y ^ msb_y) */  \
	                                                                     \
	movq scratch3, dest2;                                                \
	por  scratch1, scratch3;       /* p = msb_x | msb_y */               \
	pand dest2,    scratch1;       /* g = msb_x & msb_y */               \
	                                                                     \
	pxor msb_mask, source2;        /* diff ^= msb */                     \
	movq scratch3, dest2;                                                \
	pand source2, scratch3;        /* c = p & (diff ^ msb) */            \
	pxor msb_mask, source2;        /* diff ^= msb - restore diff */      \
	                                                                     \
	por scratch1, scratch3;                                              \
	movq msb_mask, scratch1;                                             \
	psrlq $4,     scratch3;        /* overflow = (c | g) >> 4 */         \
	psubw scratch3, scratch1;      /* msb - overflow */                  \
	pcmpeqw scratch3, scratch3;                                          \
	pxor msb_mask, scratch1;       /* mask = ((msb - overflow) ^ msb) */ \
	pxor scratch3, scratch1;       /* ~mask */                           \
	pxor source2, dest2;           /* diff ^ p */                        \
	pand scratch1, dest2;          /* result = ~mask & (diff ^ p) */     \
/* Done! - Total: 25 cycles / 8 pixels, or 3.125 cycles/pixel */



#endif
