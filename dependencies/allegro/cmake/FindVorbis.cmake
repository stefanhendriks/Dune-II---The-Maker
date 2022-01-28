# - Find vorbis
# Find the native vorbis includes and libraries
#
#  Vorbis_INCLUDE_DIR - where to find vorbis.h, etc.
#  Vorbis_LIBRARIES   - List of libraries when using vorbis(file).
#  Vorbis_FOUND       - True if vorbis found.

if(NOT GP2XWIZ)
    if(Vorbis_INCLUDE_DIR)
        # Already in cache, be silent
        set(VORBIS_FIND_QUIETLY TRUE)
    endif(Vorbis_INCLUDE_DIR)
    find_path(OGG_INCLUDE_DIR ogg/ogg.h)
    find_path(Vorbis_INCLUDE_DIR vorbis/vorbisfile.h)
    find_library(OGG_LIBRARY NAMES ogg)
    find_library(VORBIS_LIBRARY NAMES vorbis)
    find_library(VORBISFILE_LIBRARY NAMES vorbisfile)
    # Handle the QUIETLY and REQUIRED arguments and set Vorbis_FOUND
    # to TRUE if all listed variables are TRUE.
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Vorbis DEFAULT_MSG
        OGG_INCLUDE_DIR Vorbis_INCLUDE_DIR
        OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY)
else(NOT GP2XWIZ)
    find_path(Vorbis_INCLUDE_DIR tremor/ivorbisfile.h)
    find_library(VORBIS_LIBRARY NAMES vorbis_dec)
    find_package_handle_standard_args(VORBIS DEFAULT_MSG
        Vorbis_INCLUDE_DIR VORBIS_LIBRARY)
endif(NOT GP2XWIZ)

if(Vorbis_FOUND)
  if(NOT GP2XWIZ)
     set(Vorbis_LIBRARIES ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY}
           ${OGG_LIBRARY})
  else(NOT GP2XWIZ)
     set(Vorbis_LIBRARIES ${VORBIS_LIBRARY})
  endif(NOT GP2XWIZ)
else(Vorbis_FOUND)
  set(Vorbis_LIBRARIES)
endif(Vorbis_FOUND)

mark_as_advanced(OGG_INCLUDE_DIR Vorbis_INCLUDE_DIR)
mark_as_advanced(OGG_LIBRARY VORBIS_LIBRARY VORBISFILE_LIBRARY)

