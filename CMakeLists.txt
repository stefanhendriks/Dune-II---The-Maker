cmake_minimum_required(VERSION 2.6)
project(d2tm)
file(GLOB_RECURSE sources src/*.cpp src/*.h)
include_directories("dependencies/mingw32/include")
add_definitions(-DALFONT_DLL_DECLSPEC=)
add_definitions(-DALMP3_DLL_DECLSPEC=)

#add_definitions(-DUNIX)
add_definitions(-g)

# enable warnings
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

# fixes the seedmap signed char
add_definitions(-Wno-narrowing)

add_executable(d2tm ${sources})
find_library(lib_almp3 libalmp3dll)
find_library(lib_alfont libalfontdll)
find_library(lib_fblend libfblend)
find_library(lib_allegro alleg)
#add_compile_options(-std=c++11)
#find_library(lib_x11 X11)
#find_library(lib_xext Xext)
#find_library(lib_xft Xft)
#find_library(lib_xpm Xpm)
#find_library(lib_xxf86vm Xxf86vm)
#find_library(lib_xcursor Xcursor)
#find_library(lib_pthread pthread)
target_link_libraries(d2tm ${lib_almp3} ${lib_alfont} ${lib_fblend} ${lib_allegro} ${lib_x11} ${lib_xext} ${lib_xft} ${lib_xpm} ${lib_xxf86vm} ${lib_xcursor} ${lib_pthread})
# message(status ${sources})

