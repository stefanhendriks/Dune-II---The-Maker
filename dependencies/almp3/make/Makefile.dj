# Makefile for AllegroMP3 using mpglib part of mpg123


# select (uncomment) one target and comment DJGPP if you are not aiming
# for that platform
TARGET=DJGPP_STATIC
#TARGET=MINGW32_STATIC
#TARGET=MINGW32_DLL
#TARGET=LINUX_STATIC


CC=gcc
LFLAGS=
LIBIMP=



# DJGPP_STATIC
ifeq ($(TARGET),DJGPP_STATIC)
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OBJDIR=obj/djgpp/static
LIBDEST=lib/djgpp/libalmp3.a
endif



# MINGW32_STATIC
ifeq ($(TARGET),MINGW32_STATIC)
LFLAGS=-mwindows
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OBJDIR=obj/mingw32/static
LIBDEST=lib/mingw32/libalmp3.a
endif


# MINGW32_DLL
ifeq ($(TARGET),MINGW32_DLL)
LFLAGS=-mwindows -shared
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OBJDIR=obj/mingw32/dll
LIBIMP=lib/mingw32/libalmp3dll.a
LIBDEST=lib/mingw32/almp3.dll
ALMP3_DLL=1
ALMP3_DLL_EXPORTS=1
endif



# LINUX_STATIC
ifeq ($(TARGET),LINUX_STATIC)
TARGETFLAGS=-Wall -O2 -march=pentium -fomit-frame-pointer -finline-functions -ffast-math
OBJDIR=obj/linux/static
LIBDEST=lib/linux/libalmp3.a
endif



# setting object files, paths and compiler flags
vpath %.c decoder

vpath %.c src
vpath %.o $(OBJDIR)

CFLAGS=$(TARGETFLAGS) -Idecoder -Iinclude
OBJECTS=layer2.o layer3.o interface.o decode_i386.o dct64_i386.o common.o tabinit.o almp3.o
OBJECTS2=$(addprefix $(OBJDIR)/,$(OBJECTS))



# making of the library

# MINGW32_DLL
ifeq ($(TARGET),MINGW32_DLL)
$(LIBDEST): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS2) $(LFLAGS) $(CFLAGS) -Wl,--out-implib=$(LIBIMP) -lalleg

# others
else
$(LIBDEST): $(OBJECTS)
	ar rs $(LIBDEST) $(OBJECTS2)
endif


# compiling of the library
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $(OBJDIR)/$@


clean:
	rm -f $(OBJECTS2) $(LIBDEST) $(LIBIMP)

