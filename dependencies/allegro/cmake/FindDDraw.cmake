# - Find DirectDraw
# Find the DirectDraw includes and libraries
#
#  DDraw_INCLUDE_DIR - where to find ddraw.h
#  DDraw_LIBRARIES   - List of libraries when using ddraw.
#  DDraw_FOUND       - True if ddraw found.

if(DDraw_INCLUDE_DIR)
    # Already in cache, be silent
    set(DDRAW_FIND_QUIETLY TRUE)
endif(DDraw_INCLUDE_DIR)

# Makes my life easier.
if(MSVC)
    set(HINT_INCLUDE "C:/Program Files/Microsoft DirectX SDK (August 2008)/Include")
    set(HINT_LIB "C:/Program Files/Microsoft DirectX SDK (August 2008)/Lib")
endif(MSVC)

find_path(DDraw_INCLUDE_DIR ddraw.h
    PATH ${HINT_INCLUDE}
    )

find_library(DDRAW_LIBRARY
    NAMES ddraw
    PATHS ${HINT_LIB}
    PATH_SUFFIXES x86 x64
    )

# Handle the QUIETLY and REQUIRED arguments and set DDraw_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DDraw DEFAULT_MSG
    DDraw_INCLUDE_DIR DDRAW_LIBRARY)

if(DDraw_FOUND)
    set(DDraw_LIBRARIES ${DDRAW_LIBRARY})
else(DDraw_FOUND)
    set(DDraw_LIBRARIES)
endif(DDraw_FOUND)

mark_as_advanced(DDraw_INCLUDE_DIR DDRAW_LIBRARY)
