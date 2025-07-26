cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

project(d2tm)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#SET(OpenGL_GL_PREFERENCE "GLVND")
if(DEFINED ENV{ENV_D2TM_VERSION})
    SET(D2TM_VERSION "$ENV{ENV_D2TM_VERSION}")
elseif(DEFINED ENV{GITHUB_SHA})
    SET(D2TM_VERSION "$ENV{GITHUB_SHA}")
else()
    SET(D2TM_VERSION "unset")
endif()

message(STATUS "D2TM-Version determined to be: ${D2TM_VERSION}")

CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h"
)


if(APPLE)
    message(STATUS "Using pkg-config to locate SDL2 libraries on macOS")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    include_directories(
      ${SDL2_INCLUDE_DIRS}
      ${SDL2_IMAGE_INCLUDE_DIRS}
      ${SDL2_TTF_INCLUDE_DIRS}
      ${SDL2_MIXER_INCLUDE_DIRS}
    )

    link_directories(
      ${SDL2_LIBRARY_DIRS}
      ${SDL2_IMAGE_LIBRARY_DIRS}
      ${SDL2_TTF_LIBRARY_DIRS}
      ${SDL2_MIXER_LIBRARY_DIRS}
    )

else()
    message(STATUS "Using CMake's find_package to locate SDL2 libraries")

    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)

    link_libraries(SDL2 SDL2_ttf SDL2_mixer SDL2_image)
endif()

add_subdirectory(dependencies)
add_subdirectory(src)


add_executable(${PROJECT_NAME} ${D2TM_SRC} Script.rc)

target_include_directories(${PROJECT_NAME}
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h
    	${CMAKE_CURRENT_SOURCE_DIR}/src
    	${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

if (APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            fmt::fmt
            ${SDL2_LIBRARIES}
            ${SDL2_IMAGE_LIBRARIES}
            ${SDL2_TTF_LIBRARIES}
            ${SDL2_MIXER_LIBRARIES}
)
elseif (UNIX)
  	target_link_libraries(${PROJECT_NAME}
    PRIVATE
      	fmt::fmt
)
else()
  	target_link_libraries(${PROJECT_NAME}
  	PRIVATE
    fmt::fmt
    -static-libgcc
    -static-libstdc++
    -lmingw32
    -lSDL2
    -lSDL2main
    -mwindows
  )
endif()

# if(CMAKE_BUILD_TYPE MATCHES Debug)
#   set(D2TM_OPTMIZATION "-O0")
# else()
#   set(D2TM_OPTMIZATION "-O3")
#   set_target_properties(${PROJECT_NAME} PROPERTIES
#     INTERPROCEDURAL_OPTIMIZATION TRUE
#   )
# endif()

SET(CMAKE_CXX_FLAGS "-g")

target_compile_options(${PROJECT_NAME}
  PRIVATE
    ${D2TM_OPTMIZATION}
    "-Wall"
    "-Wextra"
    "-Wno-dangling-reference"
    # "-Wno-deprecated"
    "-Wpedantic"
    #"-Wnon-virtual-dtor"
    "-Wuseless-cast"
    "-Wredundant-tags"
    "-Wzero-as-null-pointer-constant"
    "-Wextra-semi"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})