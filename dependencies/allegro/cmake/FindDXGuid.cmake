# - Find dxguid
# Find the dxguid libraries
#
#  DXGuid_LIBRARIES   - List of libraries
#  DXGuid_FOUND       - True if dxguid found.

if(MSVC)
    find_library(DXGUID_LIBRARY NAMES dxguid
        PATHS "C:/Program Files/Microsoft DirectX SDK (August 2008)/Lib"
        PATH_SUFFIXES x86 x64
        )
else()
    find_library(DXGUID_LIBRARY
        NAMES dxguid
        PATHS ${HINT_LIB}
    )
endif()

# Handle the QUIETLY and REQUIRED arguments and set DXGuid_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DXGuid DEFAULT_MSG
    DXGUID_LIBRARY)

if(DXGuid_FOUND)
    set(DXGuid_LIBRARIES ${DXGUID_LIBRARY})
else(DXGuid_FOUND)
    set(DXGuid_LIBRARIES)
endif(DXGuid_FOUND)

mark_as_advanced(DXGuid_INCLUDE_DIR DXGUID_LIBRARY)
