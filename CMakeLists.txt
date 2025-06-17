cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

project(d2tm)

SET(OpenGL_GL_PREFERENCE "GLVND")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
link_libraries(SDL2 SDL2_ttf SDL2_mixer SDL2_image)

add_subdirectory(dependencies)
add_subdirectory(src)


add_executable(${PROJECT_NAME}
  ${D2TM_SRC}
  Script.rc
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

if (UNIX)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      alleg
      alfont
      fmt::fmt
  )
else()
  target_link_libraries(${PROJECT_NAME}
  PRIVATE
    allegro
    alfont
    fmt::fmt
    -static-libgcc
    -static-libstdc++
  )
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(D2TM_OPTMIZATION "-O0")
else()
  set(D2TM_OPTMIZATION "-O3")
  set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${D2TM_OPTMIZATION}
    "-Wall"
    "-Wextra"
    "-Wno-dangling-reference"
    "-Wno-deprecated"
    "-Wpedantic"
    "-Wnon-virtual-dtor"
    "-Wuseless-cast"
    "-Wredundant-tags"
    "-Wzero-as-null-pointer-constant"
    "-Wextra-semi"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (NOT UNIX)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
  )
endif()