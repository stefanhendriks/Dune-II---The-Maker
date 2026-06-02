cmake_minimum_required(VERSION 3.21)

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

add_library(cpp_stacktrace INTERFACE)

if(APPLE)
    message(STATUS "Using pkg-config to locate SDL3 libraries on macOS")

    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL3 REQUIRED sdl3)
    pkg_check_modules(SDL3_IMAGE REQUIRED SDL3_image)
    pkg_check_modules(SDL3_TTF REQUIRED SDL3_ttf)
    pkg_check_modules(SDL3_MIXER REQUIRED SDL3_mixer)

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
      ${SDL3_INCLUDE_DIRS}
      ${SDL3_IMAGE_INCLUDE_DIRS}
      ${SDL3_TTF_INCLUDE_DIRS}
      ${SDL3_MIXER_INCLUDE_DIRS}
    )

    link_directories(
      ${SDL3_LIBRARY_DIRS}
      ${SDL3_IMAGE_LIBRARY_DIRS}
      ${SDL3_TTF_LIBRARY_DIRS}
      ${SDL3_MIXER_LIBRARY_DIRS}
    )

else()
    message(STATUS "Using CMake's find_package to locate SDL3 libraries")

    find_package(SDL3 REQUIRED)
    find_package(SDL3_ttf REQUIRED)
    find_package(SDL3_image REQUIRED)
    find_package(SDL3_mixer REQUIRED)

    link_libraries(SDL3 SDL3_ttf SDL3_mixer SDL3_image)
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

add_executable(${PROJECT_NAME} ${D2TM_SRC} Script.rc)

target_include_directories(${PROJECT_NAME}
	PRIVATE
    	${CMAKE_CURRENT_SOURCE_DIR}/src
    	${CMAKE_CURRENT_SOURCE_DIR}/src/include
        /usr/local/include
)

if (APPLE)

    target_compile_definitions(${PROJECT_NAME} PRIVATE D2TM_CLANG=1)

    set(ALL_SDL_LIBS ${SDL3_LIBRARIES} ${SDL3_IMAGE_LIBRARIES} ${SDL3_TTF_LIBRARIES} ${SDL3_MIXER_LIBRARIES})
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
            "-Wno-unused-private-field"
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
    target_link_libraries(cpp_stacktrace INTERFACE stdc++exp backtrace)

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
    -lSDL3
    -mwindows
  )
    target_link_options(cpp_stacktrace INTERFACE $<$<CONFIG:Debug>:/DEBUG>)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE cpp_stacktrace)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})