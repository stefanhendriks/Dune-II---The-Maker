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

add_library(alfont SHARED
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
    alleg
)

# Compile & link options are taken from the Makefile. 
target_compile_options(alfont
  PRIVATE
    "-O2"
    "-march=pentium"
    "-fomit-frame-pointer"
    "-finline-functions"
    "-ffast-math"
)
target_link_options(alfont
  PRIVATE
    "-mwindows"
)

target_compile_definitions(alfont
  PRIVATE
    ALFONT_DLL_EXPORTS=1
  PUBLIC
    ALFONT_DLL=1
)
