add_library(alfontdll SHARED IMPORTED GLOBAL)
set_target_properties(alfontdll PROPERTIES
  IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}/alfont.dll
  IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lib/${PLATFORM_DIR}/libalfontdll.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include/
  INTERFACE_LINK_LIBRARIES alleg
)