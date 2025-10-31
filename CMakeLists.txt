cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

project(d2tm)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(DEFINED ENV{ENV_D2TM_VERSION})
    SET(D2TM_VERSION "$ENV{ENV_D2TM_VERSION}")
elseif(DEFINED ENV{GITHUB_SHA})
    SET(D2TM_VERSION "$ENV{GITHUB_SHA}")
else()
    SET(D2TM_VERSION "unset")
endif()

message(STATUS "D2TM-Version determined to be: ${D2TM_VERSION}")

CONFIGURE_FILE (
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include/config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include/config.h"
)


if(APPLE)
    message(STATUS "Using pkg-config to locate SDL2 libraries on macOS")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    set(HOMEBREW_PREFIX "/opt/homebrew")

    set(CMAKE_PREFIX_PATH
            "${HOMEBREW_PREFIX}"
            # These libraries are keg-only and not loaded into
            # the root prefix by default (to avoid clashes).
            "${HOMEBREW_PREFIX}/opt/lapack"
            "${HOMEBREW_PREFIX}/opt/openblas"
            "${HOMEBREW_PREFIX}/opt/gcc/lib/gcc/15"
    )

    set(CMAKE_C_COMPILER "${HOMEBREW_PREFIX}/bin/gcc-15")
    set(CMAKE_CXX_COMPILER "${HOMEBREW_PREFIX}/bin/g++-15")

    message(STATUS "Mac GCC C compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "Mac GCC C++ compiler: ${CMAKE_CXX_COMPILER}")

    add_compile_options(-stdlib=libstdc++)
    add_link_options(-stdlib=libstdc++)

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

add_subdirectory(src)

add_executable(${PROJECT_NAME} ${D2TM_SRC} Script.rc)

target_include_directories(${PROJECT_NAME}
	PRIVATE
    	${CMAKE_CURRENT_SOURCE_DIR}/src
    	${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

if (APPLE)

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${SDL2_LIBRARIES}
            ${SDL2_IMAGE_LIBRARIES}
            ${SDL2_TTF_LIBRARIES}
            ${SDL2_MIXER_LIBRARIES}
    )
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        ${D2TM_OPTMIZATION}
        "-Wall"
        "-Wextra"
        "-Wpedantic"
        "-Wzero-as-null-pointer-constant"
        "-Wextra-semi"
    )
elseif (UNIX)
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        ${D2TM_OPTMIZATION}
        "-Wall"
        "-Wextra"
        "-Wno-dangling-reference"
        "-Wpedantic"
        "-Wuseless-cast"
        "-Wredundant-tags"
        "-Wzero-as-null-pointer-constant"
        "-Wextra-semi"
        #"-Wsuggest-override"
        #"-Wmismatched-tags"
        #"-Wdeprecated-declarations"
    )
else()
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        ${D2TM_OPTMIZATION}
        "-Wall"
        "-Wextra"
        "-Wno-dangling-reference"
        "-Wpedantic"
        "-Wuseless-cast"
        "-Wredundant-tags"
        "-Wzero-as-null-pointer-constant"
        "-Wextra-semi"
    )
  	target_link_libraries(${PROJECT_NAME}
  	PRIVATE
    # fmt::fmt
    -static-libgcc
    -static-libstdc++
    -lmingw32
    -lSDL2main
    -lSDL2
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



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})