cmake_minimum_required(VERSION 3.1...3.18)

# Fallback for using newer policies on CMake <3.12.
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Determine if fmt is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED FMT_MASTER_PROJECT)
  set(FMT_MASTER_PROJECT OFF)
  if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(FMT_MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
  endif ()
endif ()

# Joins arguments and places the results in ${result_var}.
function(join result_var)
  set(result "")
  foreach (arg ${ARGN})
    set(result "${result}${arg}")
  endforeach ()
  set(${result_var} "${result}" PARENT_SCOPE)
endfunction()

function(enable_module target)
  if (MSVC)
    set(BMI ${CMAKE_CURRENT_BINARY_DIR}/${target}.ifc)
    target_compile_options(${target}
      PRIVATE /interface /ifcOutput ${BMI}
      INTERFACE /reference fmt=${BMI})
  endif ()
  set_target_properties(${target} PROPERTIES ADDITIONAL_CLEAN_FILES ${BMI})
  set_source_files_properties(${BMI} PROPERTIES GENERATED ON)
endfunction()

include(CMakeParseArguments)

# Sets a cache variable with a docstring joined from multiple arguments:
#   set(<variable> <value>... CACHE <type> <docstring>...)
# This allows splitting a long docstring for readability.
function(set_verbose)
  # cmake_parse_arguments is broken in CMake 3.4 (cannot parse CACHE) so use
  # list instead.
  list(GET ARGN 0 var)
  list(REMOVE_AT ARGN 0)
  list(GET ARGN 0 val)
  list(REMOVE_AT ARGN 0)
  list(REMOVE_AT ARGN 0)
  list(GET ARGN 0 type)
  list(REMOVE_AT ARGN 0)
  join(doc ${ARGN})
  set(${var} ${val} CACHE ${type} ${doc})
endfunction()

# Set the default CMAKE_BUILD_TYPE to Release.
# This should be done before the project command since the latter can set
# CMAKE_BUILD_TYPE itself (it does so for nmake).
if (FMT_MASTER_PROJECT AND NOT CMAKE_BUILD_TYPE)
  set_verbose(CMAKE_BUILD_TYPE Release CACHE STRING
              "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or "
              "CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif ()

project(FMT CXX)
include(GNUInstallDirs)
set_verbose(FMT_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING
            "Installation directory for include files, a relative path that "
            "will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute path.")

option(FMT_PEDANTIC "Enable extra warnings and expensive tests." OFF)
option(FMT_WERROR "Halt the compilation with an error on compiler warnings."
       OFF)

# Options that control generation of various targets.
option(FMT_DOC "Generate the doc target." ${FMT_MASTER_PROJECT})
option(FMT_INSTALL "Generate the install target." ${FMT_MASTER_PROJECT})
option(FMT_TEST "Generate the test target." ${FMT_MASTER_PROJECT})
option(FMT_FUZZ "Generate the fuzz target." OFF)
option(FMT_CUDA_TEST "Generate the cuda-test target." OFF)
option(FMT_OS "Include core requiring OS (Windows/Posix) " ON)
option(FMT_MODULE "Build a module instead of a traditional library." OFF)

set(FMT_CAN_MODULE OFF)
if (CMAKE_CXX_STANDARD GREATER 17 AND
    # msvc 16.10-pre4
    MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 19.29.30035)
  set(FMT_CAN_MODULE ON)
endif ()
if (NOT FMT_CAN_MODULE)
  set(FMT_MODULE OFF)
  message(STATUS "Module support is disabled.")
endif ()
if (FMT_TEST AND FMT_MODULE)
  # The tests require {fmt} to be compiled as traditional library
  message(STATUS "Testing is incompatible with build mode 'module'.")
endif ()

# Get version from core.h
file(READ include/fmt/core.h core_h)
if (NOT core_h MATCHES "FMT_VERSION ([0-9]+)([0-9][0-9])([0-9][0-9])")
  message(FATAL_ERROR "Cannot get FMT_VERSION from core.h.")
endif ()
# Use math to skip leading zeros if any.
math(EXPR CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1})
math(EXPR CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2})
math(EXPR CPACK_PACKAGE_VERSION_PATCH ${CMAKE_MATCH_3})
join(FMT_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.
                 ${CPACK_PACKAGE_VERSION_PATCH})
message(STATUS "Version: ${FMT_VERSION}")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/support/cmake")

include(cxx14)
include(CheckCXXCompilerFlag)
include(JoinPaths)

list(FIND CMAKE_CXX_COMPILE_FEATURES "cxx_variadic_templates" index)
if (${index} GREATER -1)
  # Use cxx_variadic_templates instead of more appropriate cxx_std_11 for
  # compatibility with older CMake versions.
  set(FMT_REQUIRED_FEATURES cxx_variadic_templates)
endif ()
message(STATUS "Required features: ${FMT_REQUIRED_FEATURES}")

if (FMT_MASTER_PROJECT AND NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET)
  set_verbose(CMAKE_CXX_VISIBILITY_PRESET hidden CACHE STRING
              "Preset for the export of private symbols")
  set_property(CACHE CMAKE_CXX_VISIBILITY_PRESET PROPERTY STRINGS
               hidden default)
endif ()

if (FMT_MASTER_PROJECT AND NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set_verbose(CMAKE_VISIBILITY_INLINES_HIDDEN ON CACHE BOOL
              "Whether to add a compile flag to hide symbols of inline functions")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(PEDANTIC_COMPILE_FLAGS -pedantic-errors -Wall -Wextra -pedantic
      -Wold-style-cast -Wundef
      -Wredundant-decls -Wwrite-strings -Wpointer-arith
      -Wcast-qual -Wformat=2 -Wmissing-include-dirs
      -Wcast-align
      -Wctor-dtor-privacy -Wdisabled-optimization
      -Winvalid-pch -Woverloaded-virtual
      -Wconversion -Wswitch-enum -Wundef
      -Wno-ctor-dtor-privacy -Wno-format-nonliteral)
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.6)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS}
         -Wno-dangling-else -Wno-unused-local-typedefs)
  endif ()
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS} -Wdouble-promotion
          -Wtrampolines -Wzero-as-null-pointer-constant -Wuseless-cast
          -Wvector-operation-performance -Wsized-deallocation -Wshadow)
  endif ()
  if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
      set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS} -Wshift-overflow=2
          -Wnull-dereference -Wduplicated-cond)
  endif ()
  set(WERROR_FLAG -Werror)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(PEDANTIC_COMPILE_FLAGS -Wall -Wextra -pedantic -Wconversion -Wundef
      -Wdeprecated -Wweak-vtables -Wshadow
      -Wno-gnu-zero-variadic-macro-arguments)
  check_cxx_compiler_flag(-Wzero-as-null-pointer-constant HAS_NULLPTR_WARNING)
  if (HAS_NULLPTR_WARNING)
    set(PEDANTIC_COMPILE_FLAGS ${PEDANTIC_COMPILE_FLAGS}
        -Wzero-as-null-pointer-constant)
  endif ()
  set(WERROR_FLAG -Werror)
endif ()

if (MSVC)
  set(PEDANTIC_COMPILE_FLAGS /W3)
  set(WERROR_FLAG /WX)
endif ()

if (FMT_MASTER_PROJECT AND CMAKE_GENERATOR MATCHES "Visual Studio")
  # If Microsoft SDK is installed create script run-msbuild.bat that
  # calls SetEnv.cmd to set up build environment and runs msbuild.
  # It is useful when building Visual Studio projects with the SDK
  # toolchain rather than Visual Studio.
  include(FindSetEnv)
  if (WINSDK_SETENV)
    set(MSBUILD_SETUP "call \"${WINSDK_SETENV}\"")
  endif ()
  # Set FrameworkPathOverride to get rid of MSB3644 warnings.
  join(netfxpath
       "C:\\Program Files\\Reference Assemblies\\Microsoft\\Framework\\"
       ".NETFramework\\v4.0")
  file(WRITE run-msbuild.bat "
    ${MSBUILD_SETUP}
    ${CMAKE_MAKE_PROGRAM} -p:FrameworkPathOverride=\"${netfxpath}\" %*")
endif ()

set(strtod_l_headers stdlib.h)
if (APPLE)
  set(strtod_l_headers ${strtod_l_headers} xlocale.h)
endif ()

include(CheckSymbolExists)
if (WIN32)
  check_symbol_exists(_strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
else ()
  check_symbol_exists(strtod_l "${strtod_l_headers}" HAVE_STRTOD_L)
endif ()

function(add_headers VAR)
  set(headers ${${VAR}})
  foreach (header ${ARGN})
    set(headers ${headers} include/fmt/${header})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

# Define the fmt library, its includes and the needed defines.
add_headers(FMT_HEADERS args.h chrono.h color.h compile.h core.h format.h
                        format-inl.h locale.h os.h ostream.h printf.h ranges.h
                        xchar.h)
if (FMT_MODULE)
  set(FMT_SOURCES src/fmt.cc)
elseif (FMT_OS)
  set(FMT_SOURCES src/format.cc src/os.cc)
else()
  set(FMT_SOURCES src/format.cc)
endif ()

add_library(fmt ${FMT_SOURCES} ${FMT_HEADERS} README.rst ChangeLog.rst)
add_library(fmt::fmt ALIAS fmt)

if (HAVE_STRTOD_L)
  target_compile_definitions(fmt PUBLIC FMT_LOCALE)
endif ()

if (MINGW)
  check_cxx_compiler_flag("Wa,-mbig-obj" FMT_HAS_MBIG_OBJ)
  if (${FMT_HAS_MBIG_OBJ})
    target_compile_options(fmt PUBLIC "-Wa,-mbig-obj")
  endif()
endif ()

if (FMT_WERROR)
  target_compile_options(fmt PRIVATE ${WERROR_FLAG})
endif ()
if (FMT_PEDANTIC)
  target_compile_options(fmt PRIVATE ${PEDANTIC_COMPILE_FLAGS})
endif ()
if (FMT_MODULE)
  enable_module(fmt)
endif ()

target_compile_features(fmt INTERFACE ${FMT_REQUIRED_FEATURES})

target_include_directories(fmt PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${FMT_INC_DIR}>)

set(FMT_DEBUG_POSTFIX d CACHE STRING "Debug library postfix.")

set_target_properties(fmt PROPERTIES
  VERSION ${FMT_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR}
  DEBUG_POSTFIX "${FMT_DEBUG_POSTFIX}")

# Set FMT_LIB_NAME for pkg-config fmt.pc. We cannot use the OUTPUT_NAME target
# property because it's not set by default.
set(FMT_LIB_NAME fmt)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(FMT_LIB_NAME ${FMT_LIB_NAME}${FMT_DEBUG_POSTFIX})
endif ()

if (BUILD_SHARED_LIBS)
  if (UNIX AND NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "SunOS" AND
      NOT EMSCRIPTEN)
    # Fix rpmlint warning:
    # unused-direct-shlib-dependency /usr/lib/libformat.so.1.1.0 /lib/libm.so.6.
    target_link_libraries(fmt -Wl,--as-needed)
  endif ()
  target_compile_definitions(fmt PRIVATE FMT_EXPORT INTERFACE FMT_SHARED)
endif ()
if (FMT_SAFE_DURATION_CAST)
  target_compile_definitions(fmt PUBLIC FMT_SAFE_DURATION_CAST)
endif()

add_library(fmt-header-only INTERFACE)
add_library(fmt::fmt-header-only ALIAS fmt-header-only)

target_compile_definitions(fmt-header-only INTERFACE FMT_HEADER_ONLY=1)
target_compile_features(fmt-header-only INTERFACE ${FMT_REQUIRED_FEATURES})

target_include_directories(fmt-header-only INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${FMT_INC_DIR}>)

# Install targets.
if (FMT_INSTALL)
  include(CMakePackageConfigHelpers)
  set_verbose(FMT_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/fmt CACHE STRING
              "Installation directory for cmake files, a relative path that "
              "will be joined with ${CMAKE_INSTALL_PREFIX} or an absolute "
              "path.")
  set(version_config ${PROJECT_BINARY_DIR}/fmt-config-version.cmake)
  set(project_config ${PROJECT_BINARY_DIR}/fmt-config.cmake)
  set(pkgconfig ${PROJECT_BINARY_DIR}/fmt.pc)
  set(targets_export_name fmt-targets)

  set_verbose(FMT_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING
              "Installation directory for libraries, a relative path that "
              "will be joined to ${CMAKE_INSTALL_PREFIX} or an absolute path.")

  set_verbose(FMT_PKGCONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE PATH
              "Installation directory for pkgconfig (.pc) files, a relative "
              "path that will be joined with ${CMAKE_INSTALL_PREFIX} or an "
              "absolute path.")

  # Generate the version, config and target files into the build directory.
  write_basic_package_version_file(
    ${version_config}
    VERSION ${FMT_VERSION}
    COMPATIBILITY AnyNewerVersion)

  join_paths(libdir_for_pc_file "\${exec_prefix}" "${FMT_LIB_DIR}")
  join_paths(includedir_for_pc_file "\${prefix}" "${FMT_INC_DIR}")

  configure_file(
    "${PROJECT_SOURCE_DIR}/support/cmake/fmt.pc.in"
    "${pkgconfig}"
    @ONLY)
  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/support/cmake/fmt-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${FMT_CMAKE_DIR})

  set(INSTALL_TARGETS fmt fmt-header-only)

  # Install the library and headers.
  install(TARGETS ${INSTALL_TARGETS} EXPORT ${targets_export_name}
          LIBRARY DESTINATION ${FMT_LIB_DIR}
          ARCHIVE DESTINATION ${FMT_LIB_DIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  # Use a namespace because CMake provides better diagnostics for namespaced
  # imported targets.
  export(TARGETS ${INSTALL_TARGETS} NAMESPACE fmt::
         FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)

  # Install version, config and target files.
  install(
    FILES ${project_config} ${version_config}
    DESTINATION ${FMT_CMAKE_DIR})
  install(EXPORT ${targets_export_name} DESTINATION ${FMT_CMAKE_DIR}
          NAMESPACE fmt::)

  install(FILES $<TARGET_PDB_FILE:${INSTALL_TARGETS}>
          DESTINATION ${FMT_LIB_DIR} OPTIONAL)
  install(FILES ${FMT_HEADERS} DESTINATION "${FMT_INC_DIR}/fmt")
  install(FILES "${pkgconfig}" DESTINATION "${FMT_PKGCONFIG_DIR}")
endif ()

if (FMT_DOC)
  add_subdirectory(doc)
endif ()

if (FMT_TEST)
  enable_testing()
  add_subdirectory(test)
endif ()

# Control fuzzing independent of the unit tests.
if (FMT_FUZZ)
  add_subdirectory(test/fuzzing)

  # The FMT_FUZZ macro is used to prevent resource exhaustion in fuzzing
  # mode and make fuzzing practically possible. It is similar to
  # FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION but uses a different name to
  # avoid interfering with fuzzing of projects that use {fmt}.
  # See also https://llvm.org/docs/LibFuzzer.html#fuzzer-friendly-build-mode.
  target_compile_definitions(fmt PUBLIC FMT_FUZZ)
endif ()

set(gitignore ${PROJECT_SOURCE_DIR}/.gitignore)
if (FMT_MASTER_PROJECT AND EXISTS ${gitignore})
  # Get the list of ignored files from .gitignore.
  file (STRINGS ${gitignore} lines)
  list(REMOVE_ITEM lines /resources/doc/html)
  foreach (line ${lines})
    string(REPLACE "." "[.]" line "${line}")
    string(REPLACE "*" ".*" line "${line}")
    set(ignored_files ${ignored_files} "${line}$" "${line}/")
  endforeach ()
  set(ignored_files ${ignored_files}
    /.git /breathe /format-benchmark sphinx/ .buildinfo .doctrees)

  set(CPACK_SOURCE_GENERATOR ZIP)
  set(CPACK_SOURCE_IGNORE_FILES ${ignored_files})
  set(CPACK_SOURCE_PACKAGE_FILE_NAME fmt-${FMT_VERSION})
  set(CPACK_PACKAGE_NAME fmt)
  set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/README.rst)
  include(CPack)
endif ()
