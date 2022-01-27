set(PLATFORM_DIR mingw32) # msvc not supported with CMake (yet?)

add_library(aldat STATIC IMPORTED GLOBAL)
set_target_properties(aldat PROPERTIES
  IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}/libaldat.a
)

add_library(alleg SHARED IMPORTED GLOBAL)
set_target_properties(alleg PROPERTIES
  IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}/alleg42.dll
  IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}/liballeg.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
