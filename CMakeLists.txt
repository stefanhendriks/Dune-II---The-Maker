project(d2tm)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

# Find SFML
find_package(SFML 2 REQUIRED graphics window system)

# Output an error if SFML wasn't found
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else()
	set(SFML_ROOT "" CACHE PATH "SFML top-level directory")
	message("\n-> SFML directory not found. Set SFML_ROOT to SFML's top-level path (containing \"include\" and \"lib\" directories).")
	message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
endif()

# Find Thor
find_package(Thor 2 REQUIRED)

# Output an error if Thor wasn't found
if(THOR_FOUND)
	include_directories(${THOR_INCLUDE_DIR})
else()
	set(THOR_ROOT "" CACHE PATH "Thor top-level directory")
	message("\n-> Thor directory not found. Set THOR_ROOT to Thor's top-level path (containing \"include\" and \"lib\" directories).")
	message("-> Make sure the Thor library with the same configuration (Release/Debug, Static/Dynamic) exists.\n")
endif()

# Find SFGUI
find_package(SFGUI REQUIRED)

# Output an error if SFGUI wasn't found
if(SFGUI_FOUND)
	include_directories(${SFGUI_INCLUDE_DIR})
else()
	set(SFGUI_ROOT "" CACHE PATH "SFGUI top-level directory")
	message("\n-> SFGUI directory not found. Set SFGUI_ROOT to SFGUI's top-level path (containing \"include\" and \"lib\" directories).")
	message("-> Make sure the SFGUI library with the same configuration (Release/Debug, Static/Dynamic) exists.\n")
endif()

# Find Boost
find_package(Boost REQUIRED)

# Output an error if boost wasn't found
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIR})
else()
	set(BOOST_ROOT "" CACHE PATH "Boost top-level directory")
	message("\n-> Boost directory not found. Set Boost_ROOT to Boost's top-level path (containing \"include\" and \"lib\" directories).")
	message("-> Make sure the Boost library with the same configuration (Release/Debug, Static/Dynamic) exists.\n")
endif()

# Add the source files
set(SRC_DIR "./")
set(INC_DIR "./")

set(d2_SRCS
    ${SRC_DIR}/ActionManager.cpp
    ${SRC_DIR}/Cell.cpp
    ${SRC_DIR}/Console.cpp
    ${SRC_DIR}/d2tm.cpp
    ${SRC_DIR}/Game.cpp
    ${SRC_DIR}/Houses.cpp
    ${SRC_DIR}/ini.cpp
    ${SRC_DIR}/INIReader.cpp
    ${SRC_DIR}/Map.cpp
    ${SRC_DIR}/MapLoader.cpp
    ${SRC_DIR}/Mouse.cpp
    ${SRC_DIR}/Player.cpp
    ${SRC_DIR}/SelectionBox.cpp
    ${SRC_DIR}/Unit.cpp
    ${SRC_DIR}/UnitRepository.cpp
		${INC_DIR}/ActionManager.hpp
		${INC_DIR}/Cell.hpp
		${INC_DIR}/Console.hpp
		${INC_DIR}/FpsCounter.hpp
		${INC_DIR}/Game.hpp
		${INC_DIR}/Houses.hpp
		${INC_DIR}/ini.h
		${INC_DIR}/INIReader.hpp
		${INC_DIR}/Map.hpp
		${INC_DIR}/MapLoader.hpp
		${INC_DIR}/Mouse.hpp
		${INC_DIR}/Player.hpp
		${INC_DIR}/SelectionBox.hpp
		${INC_DIR}/Unit.hpp
		${INC_DIR}/UnitRepository.hpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall -g")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
endif()

add_executable(${PROJECT_NAME} ${d2_SRCS})

# Link SFML
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${THOR_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${SFGUI_LIBRARY})
# Boost is currently header-only

# Install executable
install(TARGETS ${PROJECT_NAME} 
	RUNTIME DESTINATION .)

# Install resource directories
install(DIRECTORY graphics maps resources
	DESTINATION .)
		
# Install resource files
install(FILES DejaVuSans.ttf gui.theme
	DESTINATION .)
