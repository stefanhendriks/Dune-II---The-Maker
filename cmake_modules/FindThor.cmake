#################################################################################
##
## Thor C++ Library
## Copyright (c) 2011-2012 Jan Haller
## 
## This software is provided 'as-is', without any express or implied
## warranty. In no event will the authors be held liable for any damages
## arising from the use of this software.
## 
## Permission is granted to anyone to use this software for any purpose,
## including commercial applications, and to alter it and redistribute it
## freely, subject to the following restrictions:
## 
## 1. The origin of this software must not be misrepresented; you must not
##    claim that you wrote the original software. If you use this software
##    in a product, an acknowledgment in the product documentation would be
##    appreciated but is not required.
## 
## 2. Altered source versions must be plainly marked as such, and must not be
##    misrepresented as being the original software.
## 
## 3. This notice may not be removed or altered from any source distribution.
##
#################################################################################

# This script locates the Thor C++ Library
# Based on the FindSFML.cmake module
# ========================================
#
# Usage
# -----
#
# Thor's libraries use the same configuration (static/dynamic linking) as SFML's.
# If the variable SFML_STATIC_LIBRARIES is set to TRUE before calling find_package(Thor),
# the static libraries of Thor are found. Otherwise, the dynamic ones are found.
#
# If Thor is not installed in a standard path, you can use the THOR_ROOT CMake (or environment) variable
# to tell CMake where Thor is.
#
# Output
# ------
#
# This script defines the following variables:
# - THOR_LIBRARY_DEBUG:   Name of the debug library (set to THOR_LIBRARY_RELEASE if no debug version is found)
# - THOR_LIBRARY_RELEASE: Name of the release library (set to THOR_LIBRARY_DEBUG if no release version is found)
# - THOR_LIBRARY:		  Name of the library to link to (includes both debug and release names if necessary)
# - THOR_INCLUDE_DIR:	  The path where Thor headers are located (the directory containing the Thor/Config.hpp file)
# - THOR_FOUND:		      Is TRUE if at least one of the debug or release library is found
#
# Usage example:
# 
#   find_package(Thor 2 REQUIRED)
#   include_directories(${THOR_INCLUDE_DIR})
#   target_link_libraries(MyProject ${THOR_LIBRARY})


# Deduce the libraries suffix from the options
set(FINDTHOR_LIB_SUFFIX "")
if(SFML_STATIC_LIBRARIES)
	set(FINDTHOR_LIB_SUFFIX "-s")
endif()

# Find the SFML include directory
find_path(THOR_INCLUDE_DIR Thor/Config.hpp
		  PATH_SUFFIXES include
		  PATHS
		  ${THOR_ROOT}
		  $ENV{THOR_ROOT}
		  /usr/local/
		  /usr/
		  /sw          # Fink
		  /opt/local/  # DarwinPorts
		  /opt/csw/    # Blastwave
		  /opt/)

# Find the requested modules
set(THOR_FOUND TRUE) # will be set to false if one of the required modules is not found
set(FINDTHOR_LIB_PATHS
	${THOR_ROOT}
	$ENV{THOR_ROOT}
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt)

# Debug library
find_library(THOR_LIBRARY_DEBUG
			 NAMES thor${FINDTHOR_LIB_SUFFIX}-d
			 PATH_SUFFIXES lib64 lib
			 PATHS ${FINDTHOR_LIB_PATHS})

# Release library
find_library(THOR_LIBRARY_RELEASE
			 NAMES thor${FINDTHOR_LIB_SUFFIX}
			 PATH_SUFFIXES lib64 lib
			 PATHS ${FINDTHOR_LIB_PATHS})

if(THOR_LIBRARY_DEBUG OR THOR_LIBRARY_RELEASE)
	# Library found
	set(THOR_FOUND TRUE)

	# If both are found, set THOR_LIBRARY to contain both
	if(THOR_LIBRARY_DEBUG AND THOR_LIBRARY_RELEASE)
		set(THOR_LIBRARY debug	 ${THOR_LIBRARY_DEBUG}
						 optimized ${THOR_LIBRARY_RELEASE})
	endif()

	# If only one debug/release variant is found, set the other to be equal to the found one
	if(THOR_LIBRARY_DEBUG AND NOT THOR_LIBRARY_RELEASE)
		# debug and not release
		set(THOR_LIBRARY_RELEASE ${THOR_LIBRARY_DEBUG})
		set(THOR_LIBRARY		 ${THOR_LIBRARY_DEBUG})
	endif()
	if(THOR_LIBRARY_RELEASE AND NOT THOR_LIBRARY_DEBUG)
		# release and not debug
		set(THOR_LIBRARY_DEBUG ${THOR_LIBRARY_RELEASE})
		set(THOR_LIBRARY	   ${THOR_LIBRARY_RELEASE})
	endif()
else()
	# Library not found
	set(THOR_FOUND FALSE)
	set(THOR_LIBRARY "")
	set(FINDTHOR_MISSING "${FINDTHOR_MISSING} THOR_LIBRARY")
endif()

# Mark variables as advanced
mark_as_advanced(THOR_INCLUDE_DIR
				 THOR_LIBRARY
				 THOR_LIBRARY_RELEASE
				 THOR_LIBRARY_DEBUG)

# Result
if(THOR_FOUND)
	# Success
	message("Found Thor: ${THOR_INCLUDE_DIR}")
else()
	# include directory or library not found
	set(FINDTHOR_ERROR "Could NOT find Thor (missing: ${FINDTHOR_MISSING})")

	if(THOR_FIND_REQUIRED)
		# Fatal error
		message(FATAL_ERROR ${FINDTHOR_ERROR})
	elseif(NOT THOR_FIND_QUIETLY)
		# Error, but continue
		message("${FINDTHOR_ERROR}")
	endif()
endif()
