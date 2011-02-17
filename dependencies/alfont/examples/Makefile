# Makefile for AllegroFont examples


# select (uncomment) one target and commet DJGPP again if you are not
# aiming for that platform
TARGET=DJGPP_STATIC
#TARGET=MINGW32_STATIC
#TARGET=LINUX_STATIC


CC=gcc
LFLAGS=-s



# DJGPP_STATIC
ifeq ($(TARGET),DJGPP_STATIC)
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OUT=djgpp/example1.exe
LIBS=../lib/djgpp/libalfont.a -lalleg
OBJDIR=../obj/djgpp/examples
endif



# MINGW32_STATIC
ifeq ($(TARGET),MINGW32_STATIC)
LFLAGS=-s -mwindows
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OUT=mingw32/example1.exe
LIBS=../lib/mingw32/libalfont.a -lalleg
OBJDIR=../obj/mingw32/examples
endif



# LINUX_STATIC
ifeq ($(TARGET),LINUX_STATIC)
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OUT=linux/example1
LIBS=../lib/linux/libalfont.a `allegro-config --libs`
OBJDIR=../obj/linux/examples
endif



# setting compiler flags
CFLAGS=$(TARGETFLAGS) -I../include



# making of the example
$(OUT): example1.o
	$(CC) $(LFLAGS) $(CFLAGS) $(OBJDIR)/$< -o $@ $(LIBS)



# compiling of the example
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $(OBJDIR)/$@



clean:
	rm -f $(OBJDIR)/example1.o $(OUT)

