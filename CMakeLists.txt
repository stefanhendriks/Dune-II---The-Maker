cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

project(d2tm)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(DEFINED ENV{ENV_D2TM_VERSION})
    SET(D2TM_VERSION "$ENV{ENV_D2TM_VERSION}")
elseif(DEFINED ENV{GITHUB_SHA})
    SET(D2TM_VERSION "$ENV{GITHUB_SHA}")
else()
    SET(D2TM_VERSION "unset")
endif()

message(STATUS "D2TM-Version determined to be: ${D2TM_VERSION}")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

    if(EXISTS "/opt/homebrew/bin/brew")
        set(HOMEBREW_PREFIX "/opt/homebrew")
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()

    set(CMAKE_PREFIX_PATH
            "${HOMEBREW_PREFIX}"
            # These libraries are keg-only and not loaded into
            # the root prefix by default (to avoid clashes).
            "${HOMEBREW_PREFIX}/opt/lapack"
            "${HOMEBREW_PREFIX}/opt/openblas"
    )

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

#enable_testing()
#add_subdirectory(tests)

add_executable(${PROJECT_NAME} ${D2TM_SRC} Script.rc)

target_include_directories(${PROJECT_NAME}
	PRIVATE
    	${CMAKE_CURRENT_SOURCE_DIR}/src
    	${CMAKE_CURRENT_SOURCE_DIR}/src/include
        /usr/local/include/SDL2
        /usr/local/include
)

if (APPLE)

    target_compile_definitions(${PROJECT_NAME} PRIVATE D2TM_CLANG=1)

    set(ALL_SDL_LIBS ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
    list(REMOVE_DUPLICATES ALL_SDL_LIBS)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            ${ALL_SDL_LIBS}
    )
    target_compile_options(${PROJECT_NAME}
      PRIVATE
        ${D2TM_OPTMIZATION}
        "-Wall"
        "-Wextra"
        "-Wpedantic"
        "-Wzero-as-null-pointer-constant"
        "-Wextra-semi"
        "-Werror"
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
        "-Werror"
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
        "-Werror"
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})