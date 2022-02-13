# Alfont has the FreeType library internally. These are the freetype sources,
# in the same order as in Makefile. Not sure if it matters, but this should replicate the original
# build the best.
set(FREETYPE_SOURCES
    "freetype/src/base/ftsystem.c"
    "freetype/src/base/ftdebug.c"
    "freetype/src/base/ftinit.c"
    "freetype/src/base/ftbase.c"
    "freetype/src/base/ftbitmap.c"
    "freetype/src/base/ftglyph.c"
    "freetype/src/base/ftmm.c"
    "freetype/src/cache/ftcache.c"
    "freetype/src/cff/cff.c"
    "freetype/src/cid/type1cid.c"
    "freetype/src/otvalid/otvalid.c"
    "freetype/src/pcf/pcf.c"
    "freetype/src/psaux/psaux.c"
    "freetype/src/pshinter/pshinter.c"
    "freetype/src/psnames/psmodule.c"
    "freetype/src/raster/raster.c"
    "freetype/src/sfnt/sfnt.c"
    "freetype/src/smooth/smooth.c"
    "freetype/src/truetype/truetype.c"
    "freetype/src/type1/type1.c"
    "freetype/src/winfonts/winfnt.c"
    "freetype/src/type42/type42.c"
    "freetype/src/gzip/ftgzip.c"
    "freetype/src/lzw/ftlzw.c"
    "freetype/src/pfr/pfr.c"
    "freetype/src/bdf/bdf.c"
    "freetype/src/autofit/autofit.c"
)

add_library(alfont STATIC
    "src/alfont.c"
    ${FREETYPE_SOURCES}
)

target_include_directories(alfont
  PRIVATE
    "freetype/include"
  PUBLIC
    "include"
)

target_link_libraries(alfont
  PUBLIC
    allegro
)

# Alfont has DOS and Linux specific fixes. Building for DOS is not supported for d2tm, but for
# Linux, we enable these fixes here.
if(UNIX)
  set(ALFONT_PLATFORM "ALFONT_LINUX")
else() # Default to "Windows" (no fixes)
  set(ALFONT_PLATFORM "ALFONT_WINDOWS")
endif()

# Compile & link options are taken from the Makefile. 
target_compile_options(alfont
  PRIVATE
    "-D${ALFONT_PLATFORM}"
    "-O2"
    "-march=pentium"
    "-fomit-frame-pointer"
    "-finline-functions"
    "-ffast-math"
)
