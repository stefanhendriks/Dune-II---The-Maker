# - Find DirectInput
# Find the DirectInput includes and libraries
#
#  DInput_INCLUDE_DIR - where to find dinput.h
#  DInput_LIBRARIES   - List of libraries when using dinput.
#  DInput_FOUND       - True if dinput found.

if(DInput_INCLUDE_DIR)
    # Already in cache, be silent
    set(DINPUT_FIND_QUIETLY TRUE)
endif(DInput_INCLUDE_DIR)

# Makes my life easier.
if(MSVC)
    set(HINT_INCLUDE "C:/Program Files/Microsoft DirectX SDK (August 2008)/Include")
    set(HINT_LIB "C:/Program Files/Microsoft DirectX SDK (August 2008)/Lib")
endif(MSVC)

find_path(DInput_INCLUDE_DIR dinput.h
    PATH ${HINT_INCLUDE}
    )

find_library(DINPUT_LIBRARY
    NAMES dinput dinput8
    PATHS ${HINT_LIB}
    PATH_SUFFIXES x86 x64
    )

# Handle the QUIETLY and REQUIRED arguments and set DInput_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DInput DEFAULT_MSG
    DInput_INCLUDE_DIR DINPUT_LIBRARY)

if(DInput_FOUND)
    set(DInput_LIBRARIES ${DINPUT_LIBRARY})
else(DInput_FOUND)
    set(DInput_LIBRARIES)
endif(DInput_FOUND)

mark_as_advanced(DInput_INCLUDE_DIR DINPUT_LIBRARY)
