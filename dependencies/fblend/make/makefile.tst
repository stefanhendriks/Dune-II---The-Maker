

#-------------------------------#
#-- FBlend compiler config -----#

.PHONY: ssetest mmxtest

ssetest:
ifdef UNIX_TOOLS
	- @echo "#define FBLEND_NO_SSE_SUPPORT" > sse.h
else
	- @echo #define FBLEND_NO_SSE_SUPORT > sse.h
endif
	- @echo .text > ssetest.s
	- @echo prefetcht0 (%esi) >> ssetest.s
	@gcc -c ssetest.s -o $(OBJ_DIR_U)/ssetest.o
ifdef UNIX_TOOLS
	- @echo "#define FBLEND_SSE" > sse.h
else
	- @echo #define FBLEND_SSE > sse.h
endif


include/sse.h:
	@echo Testing for SSE assembler support...
	@- $(MAKE) ssetest --quiet
ifdef UNIX_TOOLS
	@- mv sse.h include
	@- rm -f ssetest.s
else
	@- move sse.h include
	@- del ssetest.s
endif
	@- echo .


mmxtest:
ifdef UNIX_TOOLS
	- @echo "#define FBLEND_NO_MMX_SUPPORT" > mmx.h
else
	- @echo #define FBLEND_NO_MMX_SUPORT > mmx.h
endif
	- @echo .text > mmxtest.s
	- @echo prefetcht0 (%esi) >> mmxtest.s
	@gcc -c mmxtest.s -o $(OBJ_DIR_U)/mmxtest.o
ifdef UNIX_TOOLS
	- @echo "#define FBLEND_MMX" > mmx.h
else
	- @echo #define FBLEND_MMX > mmx.h
endif


include/mmx.h:
	@echo Testing for MMX assembler support...
	@- $(MAKE) mmxtest --quiet
ifdef UNIX_TOOLS
	@- mv mmx.h include
	@- rm -f mmxtest.s
else
	@- move mmx.h include
	@- del mmxtest.s
endif
	@- echo .


obj/$(COMPILER)/asmdef.inc: obj/$(COMPILER)/asmdef.exe
	@obj/$(COMPILER)/asmdef.exe obj/$(COMPILER)/asmdef.inc

obj/$(COMPILER)/asmdef.exe: $(SRC_DIR_U)/asmdef.c $(MAIN_INCLUDE_PATH)
	gcc $(SRC_DIR_U)/asmdef.c -o $@ -I. -I./include
