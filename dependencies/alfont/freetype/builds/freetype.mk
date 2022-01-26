#
# FreeType 2 library sub-Makefile
#


# Copyright 1996-2000, 2001, 2002, 2003, 2004 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


# DO NOT INVOKE THIS MAKEFILE DIRECTLY!  IT IS MEANT TO BE INCLUDED BY
# OTHER MAKEFILES.


# The following variables (set by other Makefile components, in the
# environment, or on the command line) are used:
#
#   BUILD_DIR      The architecture dependent directory,
#                  e.g. `$(TOP_DIR)/builds/unix'.
#
#   OBJ_DIR        The directory in which object files are created.
#
#   LIB_DIR        The directory in which the library is created.
#
#   DOC_DIR        The directory in which the API reference is created.
#
#   INCLUDES       A list of directories to be included additionally.
#                  Usually empty.
#
#   CFLAGS         Compilation flags.  This overrides the default settings
#                  in the platform-specific configuration files.
#
#   FTSYS_SRC      If set, its value is used as the name of a replacement
#                  file for `src/base/ftsystem.c'.
#
#   FTDEBUG_SRC    If set, its value is used as the name of a replacement
#                  file for `src/base/ftdebug.c'.  [For a normal build, this
#                  file does nothing.]
#
#   FT_MODULE_LIST The file which contains the list of modules for the
#                  current build.  Usually, this is automatically created by
#                  `modules.mk'.
#
#   BASE_OBJ_S
#   BASE_OBJ_M     A list of base objects (for single object and multiple
#                  object builds, respectively).  Set up in
#                  `src/base/rules.mk'.
#
#   BASE_EXT_OBJ   A list of base extension objects.  Set up in
#                  `src/base/rules.mk'.
#
#   DRV_OBJ_S
#   DRV_OBJ_M      A list of driver objects (for single object and multiple
#                  object builds, respectively).  Set up cumulatively in
#                  `src/<driver>/rules.mk'.
#
#   CLEAN
#   DISTCLEAN      The sub-makefiles can append additional stuff to these two
#                  variables which is to be removed for the `clean' resp.
#                  `distclean' target.
#
#   TOP_DIR, SEP,
#   COMPILER_SEP,
#   LIBRARY, CC,
#   A, I, O, T     Check `config.mk' for details.


# The targets `objects' and `library' are defined at the end of this
# Makefile after all other rules have been included.
#
.PHONY: single multi objects library refdoc

# default target -- build single objects and library
#
single: objects library

# `multi' target -- build multiple objects and library
#
multi: objects library


# The FreeType source directory, usually `./src'.
#
SRC_DIR := $(TOP_DIR)/src

# The directory where the base layer components are placed, usually
# `./src/base'.
#
BASE_DIR := $(SRC_DIR)/base

# Other derived directories.
#
PUBLIC_DIR   := $(TOP_DIR)/include/freetype
INTERNAL_DIR := $(PUBLIC_DIR)/internal
SERVICES_DIR := $(INTERNAL_DIR)/services
CONFIG_DIR   := $(PUBLIC_DIR)/config
CACHE_DIR    := $(PUBLIC_DIR)/cache

# The documentation directory.
#
ifndef DOC_DIR
  DOC_DIR := $(TOP_DIR)/docs/reference
endif

# The final name of the library file.
#
PROJECT_LIBRARY := $(LIB_DIR)/$(LIBRARY).$A


# include paths
#
# IMPORTANT NOTE: The architecture-dependent directory must ALWAYS be placed
#                 in front of the include list.  Porters are then able to
#                 put their own version of some of the FreeType components
#                 in the `freetype/builds/<system>' directory, as these
#                 files will override the default sources.
#
INCLUDES := $(subst /,$(COMPILER_SEP),$(OBJ_DIR) $(BUILD_DIR) \
                                      $(TOP_DIR)/include)

INCLUDE_FLAGS = $(INCLUDES:%=$I%)


# C flags used for the compilation of an object file.  This must include at
# least the paths for the `base' and `builds/<system>' directories;
# debug/optimization/warning flags + ansi compliance if needed.
#
# $(INCLUDE_FLAGS) should come before $(CFLAGS) to avoid problems with
# old FreeType versions.
#
FT_CFLAGS  = $(INCLUDE_FLAGS) $(CFLAGS)
FT_CC      = $(CC) $(FT_CFLAGS)
FT_COMPILE = $(CC) $(ANSIFLAGS) $(FT_CFLAGS)


# Include the `modules' rules file.
#
include $(TOP_DIR)/builds/modules.mk


# Initialize the list of objects.
#
OBJECTS_LIST :=


# Define $(PUBLIC_H) as the list of all public header files located in
# `$(TOP_DIR)/include/freetype'.  $(BASE_H), $(CACHE_H), and $(CONFIG_H) are
# defined similarly.
#
# This is used to simplify the dependency rules -- if one of these files
# changes, the whole library is recompiled.
#
PUBLIC_H   := $(wildcard $(PUBLIC_DIR)/*.h)
BASE_H     := $(wildcard $(INTERNAL_DIR)/*.h) \
              $(wildcard $(SERVICES_DIR)/*.h)
CONFIG_H   := $(wildcard $(CONFIG_DIR)/*.h) \
              $(wildcard $(BUILD_DIR)/freetype/config/*.h)
CACHE_H    := $(wildcard $(CACHE_DIR)/*.h)
DEVEL_H    := $(wildcard $(TOP_DIR)/devel/*.h)

FREETYPE_H := $(PUBLIC_H) $(BASE_H) $(CONFIG_H) $(CACHE_H) $(DEVEL_H)


# ftsystem component
#
ifndef FTSYS_SRC
  FTSYS_SRC = $(BASE_DIR)/ftsystem.c
endif

FTSYS_OBJ = $(OBJ_DIR)/ftsystem.$O

OBJECTS_LIST += $(FTSYS_OBJ)

$(FTSYS_OBJ): $(FTSYS_SRC) $(FREETYPE_H)
	$(FT_COMPILE) $T$(subst /,$(COMPILER_SEP),$@ $<)


# ftdebug component
#
ifndef FTDEBUG_SRC
  FTDEBUG_SRC = $(BASE_DIR)/ftdebug.c
endif

FTDEBUG_OBJ = $(OBJ_DIR)/ftdebug.$O

OBJECTS_LIST += $(FTDEBUG_OBJ)

$(FTDEBUG_OBJ): $(FTDEBUG_SRC) $(FREETYPE_H)
	$(FT_COMPILE) $T$(subst /,$(COMPILER_SEP),$@ $<)


# Include all rule files from FreeType components.
#
include $(wildcard $(SRC_DIR)/*/rules.mk)


# ftinit component
#
#   The C source `ftinit.c' contains the FreeType initialization routines.
#   It is able to automatically register one or more drivers when the API
#   function FT_Init_FreeType() is called.
#
#   The set of initial drivers is determined by the driver Makefiles
#   includes above.  Each driver Makefile updates the FTINIT_xxx lists
#   which contain additional include paths and macros used to compile the
#   single `ftinit.c' source.
#
FTINIT_SRC := $(BASE_DIR)/ftinit.c
FTINIT_OBJ := $(OBJ_DIR)/ftinit.$O

OBJECTS_LIST += $(FTINIT_OBJ)

$(FTINIT_OBJ): $(FTINIT_SRC) $(FREETYPE_H) $(FT_MODULE_LIST)
	$(FT_COMPILE) $T$(subst /,$(COMPILER_SEP),$@ $<)


# All FreeType library objects
#
#   By default, we include the base layer extensions.  These could be
#   omitted on builds which do not want them.
#
OBJ_M = $(BASE_OBJ_M) $(BASE_EXT_OBJ) $(DRV_OBJS_M)
OBJ_S = $(BASE_OBJ_S) $(BASE_EXT_OBJ) $(DRV_OBJS_S)


# The target `multi' on the Make command line indicates that we want to
# compile each source file independently.
#
# Otherwise, each module/driver is compiled in a single object file through
# source file inclusion (see `src/base/ftbase.c' or
# `src/truetype/truetype.c' for examples).
#
BASE_OBJECTS := $(OBJECTS_LIST)

ifneq ($(findstring multi,$(MAKECMDGOALS)),)
  OBJECTS_LIST += $(OBJ_M)
else
  OBJECTS_LIST += $(OBJ_S)
endif

objects: $(OBJECTS_LIST)

library: $(PROJECT_LIBRARY)

.c.$O:
	$(FT_COMPILE) $T$(subst /,$(COMPILER_SEP),$@ $<)


refdoc:
	python $(SRC_DIR)/tools/docmaker/docmaker.py \
               --prefix=ft2                          \
               --title=FreeType-2.1.10               \
               --output=$(DOC_DIR)                   \
               $(PUBLIC_DIR)/*.h                     \
               $(PUBLIC_DIR)/config/*.h              \
               $(PUBLIC_DIR)/cache/*.h


.PHONY: clean_project_std distclean_project_std

# Standard cleaning and distclean rules.  These are not accepted
# on all systems though.
#
clean_project_std:
	-$(DELETE) $(BASE_OBJECTS) $(OBJ_M) $(OBJ_S) $(CLEAN)

distclean_project_std: clean_project_std
	-$(DELETE) $(PROJECT_LIBRARY)
	-$(DELETE) *.orig *~ core *.core $(DISTCLEAN)


.PHONY: clean_project_dos distclean_project_dos

# The Dos command shell does not support very long list of arguments, so
# we are stuck with wildcards.
#
# Don't break the command lines with \; this prevents the "del" command from
# working correctly on Win9x.
#
clean_project_dos:
	-$(DELETE) $(subst /,\,$(OBJ)/*.$O $(CLEAN) $(NO_OUTPUT))

distclean_project_dos: clean_project_dos
	-$(DELETE) $(subst /,\,$(PROJECT_LIBRARY) $(DISTCLEAN) $(NO_OUTPUT))


.PHONY: remove_config_mk

# Remove configuration file (used for distclean).
#
remove_config_mk:
	-$(DELETE) $(subst /,$(SEP),$(CONFIG_MK) $(NO_OUTPUT))


.PHONY: clean distclean

# The `config.mk' file must define `clean_freetype' and
# `distclean_freetype'.  Implementations may use to relay these to either
# the `std' or `dos' versions from above, or simply provide their own
# implementation.
#
clean: clean_project
distclean: distclean_project remove_config_mk
	-$(DELETE) $(subst /,$(SEP),$(DOC_DIR)/*.html $(NO_OUTPUT))


# EOF
