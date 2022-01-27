# - Find DirectSound
# Find the DirectSound includes and libraries
#
#  DSound_INCLUDE_DIR - where to find dsound.h
#  DSound_LIBRARIES   - List of libraries when using dsound.
#  DSound_FOUND       - True if dsound found.

if(DSound_INCLUDE_DIR)
    # Already in cache, be silent
    set(DSOUND_FIND_QUIETLY TRUE)
endif(DSound_INCLUDE_DIR)

# Makes my life easier.
if(MSVC)
    set(HINT_INCLUDE "C:/Program Files/Microsoft DirectX SDK (August 2008)/Include")
    set(HINT_LIB "C:/Program Files/Microsoft DirectX SDK (August 2008)/Lib")
endif(MSVC)

find_path(DSound_INCLUDE_DIR dsound.h
    PATHS ${HINT_INCLUDE})

find_library(DSOUND_LIBRARY NAMES dsound
    PATHS ${HINT_LIB}
    PATH_SUFFIXES x86 x64
    )

# Handle the QUIETLY and REQUIRED arguments and set DSound_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DSound DEFAULT_MSG
    DSound_INCLUDE_DIR DSOUND_LIBRARY)

if(DSound_FOUND)
    set(DSound_LIBRARIES ${DSOUND_LIBRARY})
else(DSound_FOUND)
    set(DSound_LIBRARIES)
endif(DSound_FOUND)

mark_as_advanced(DSound_INCLUDE_DIR DSOUND_LIBRARY)
