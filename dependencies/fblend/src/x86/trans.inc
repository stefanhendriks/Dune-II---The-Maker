#ifndef TRANS_INC
#define TRANS_INC


#define SET_ON_EQUAL_16_8_MMX(source1, source2, mask, dest1, dest2, scratch1, scratch2, scratch3, scratch4) \
		movq source1, scratch1;                                          \
		movq source2, scratch2;                                          \
                                                                         \
		pcmpeqw mask, source1;                                           \
		movq dest1, scratch3;                                            \
		pcmpeqw mask, source2;                                           \
		movq dest2, scratch4;                                            \
                                                                         \
	    pand source1, scratch3;                                          \
	    pand source2, scratch4;                                          \
	    pandn scratch1, source1;   /* Set to 0 what is eq to mask */     \
	    pandn scratch2, source2;                                         \
	    por scratch3, source1;     /* Set src to dest if equal to mask */\
	    por scratch4, source2;                                           \



#define SET_ON_EQUAL_16_4_MMX(source1, mask, dest1, scratch1, scratch2)  \
		movq source1, scratch1;                                          \
                                                                         \
		pcmpeqw mask, source1;                                           \
		movq dest1, scratch2;                                            \
                                                                         \
	    pand source1, scratch2;                                          \
	    pandn scratch1, source1;   /* Set to 0 what is eq to mask */     \
	    por scratch2, source1;     /* Set src to dest if equal to mask */\



#define TRANSLUCENT_BLEND_16_4_MMX(source1, dest1, fact, r_mask, g_mask, b_mask, scratch1, scratch2, scratch3, scratch4) \
		movq source1, scratch1;                                          \
		movq dest1,   scratch2;                                          \
		movq r_mask,  scratch3;                                          \
		                                                                 \
		pand scratch3, source1;    /* Mask Red */                        \
		pand scratch3, dest1;                                            \
		                                                                 \
		psrlw $5, source1;         /* Multiply by the factor */          \
		psrlw $5, dest1;                                                 \
		psubw dest1, source1;                                            \
		pmullw fact, source1;                                            \
		psllw $5, dest1;                                                 \
		                                                                 \
		paddw dest1, source1;      /* Offset and clean up Red */         \
		                                                                 \
		movq scratch1, scratch4;                                         \
		pand scratch3, source1;                                          \
		movq g_mask, scratch3;                                           \
		movq scratch2, dest1;                                            \
		                                                                 \
		pand scratch3, scratch1;   /* Mask Green */                      \
		pand scratch3, dest1;                                            \
		                                                                 \
		psubw dest1, scratch1;     /* Multiply by the factor */          \
		pmullw fact, scratch1;                                           \
		psrlw $5, scratch1;                                              \
		                                                                 \
		paddw dest1, scratch1;     /* Offset and clean up Green */       \
		pand scratch3, scratch1;                                         \
		                                                                 \
		movq b_mask, scratch3;                                           \
		por  scratch1, source1;    /* Combine Red and Green */           \
		                                                                 \
		pand scratch3, scratch4;   /* Mask Blue */                       \
		pand scratch3, scratch2;                                         \
		                                                                 \
		psubw scratch2, scratch4;  /* Multiply by the factor */          \
		pmullw fact, scratch4;                                           \
		psrlw $5, scratch4;                                              \
		                                                                 \
		paddw scratch2, scratch4;  /* Offset and clean up Blue */        \
		pand scratch3, scratch4;                                         \
		                                                                 \
		por scratch4, source1;     /* Combine RGB */                     \



#define TRANSLUCENT_HALF_16_8_MMX(source1, source2, dest1, dest2, high_mask, lsb_mask, scratch1, scratch2, scratch3) \
		movq source1, scratch1;                                          \
		movq dest1,   scratch2;                                          \
		                                                                 \
		pand high_mask, scratch1;                                        \
		pand high_mask, scratch2;                                        \
		                                                                 \
		psrlq $1, scratch1;                                              \
		pand lsb_mask, source1;                                          \
		psrlq $1, scratch2;                                              \
		pand source1, dest1;                                             \
		                                                                 \
		paddw scratch1, scratch2;                                        \
		movq source2, scratch1;                                          \
		movq dest2,   scratch3;                                          \
		paddw scratch2, dest1;                                           \
		                                                                 \
		pand high_mask, scratch1;                                        \
		pand high_mask, scratch3;                                        \
		                                                                 \
		psrlq $1, scratch1;                                              \
		pand lsb_mask, source2;                                          \
		psrlq $1, scratch3;                                              \
		pand source2, dest2;                                             \
		                                                                 \
		paddw scratch1, scratch3;                                        \
		paddw scratch3, dest2;                                           \



#define TRANSLUCENT_HALF_16_4_MMX(source1, dest1, high_mask, lsb_mask, scratch1, scratch2) \
		movq source1, scratch1;                                          \
		movq dest1,   scratch2;                                          \
		                                                                 \
		pand high_mask, scratch1;                                        \
		pand high_mask, scratch2;                                        \
		                                                                 \
		psrlq $1, scratch1;                                              \
		pand lsb_mask, source1;                                          \
		psrlq $1, scratch2;                                              \
		pand source1, dest1;                                             \
		                                                                 \
		paddw scratch1, scratch2;                                        \
		paddw scratch2, dest1;                                           \


#endif