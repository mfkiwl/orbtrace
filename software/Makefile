# Build configuration
VERBOSE?=0
DEBUG=1
SCREEN_HANDLING=1

# Set your preferred screen colours here, or create a new palette by copying the file to a new one
SCREEN_PALETTE="uicolours_default.h"
#SCREEN_PALETTE="uicolours_mono.h"

CFLAGS=-DVERSION="\"1.00InProgress\""

CROSS_COMPILE=
# Output Files
ORBTRACELIB = orbtraceif
ORBTRACE  = orbtrace

##########################################################################
# Check Host OS
##########################################################################

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  CFLAGS += -DLINUX -D_GNU_SOURCE
  LINUX=1
endif
ifeq ($(UNAME_S),Darwin)
  CFLAGS += -DOSX
  OSX=1
endif

##########################################################################
# User configuration and firmware specific object files
##########################################################################

# Overall system defines for compilation
ifdef DEBUG
GCC_DEFINE= -DDEBUG
DEBUG_OPTS = -g3 -gdwarf-2 -ggdb3
OPT_LEVEL = -Og
else
GCC_DEFINE=
DEBUG_OPTS =
OPT_LEVEL = -O2
endif

ifeq ($(SCREEN_HANDLING),1)
CFLAGS += -DSCREEN_HANDLING
endif

# Directories for sources
App_DIR=Src
Inc_DIR=Inc
EXT=$(App_DIR)/external
EXTINC=$(Inc_DIR)/external
INCLUDE_PATHS = -I$(Inc_DIR) -I$(EXTINC) -I$(OLOC)

GCC_DEFINE+= -std=gnu99

CFILES =
SFILES =
OLOC = ofiles

LDLIBS += -L$(OLOC) -l$(ORBTRACELIB)

ifdef OSX
INCLUDE_PATHS += -I/usr/local/include/libusb-1.0 -I/usr/local/opt/binutils/include
LDLIBS += -L. -lpthread -L/usr/local/lib -lusb-1.0 -ldl -lintl -L/usr/local/opt/binutils/lib
else
INCLUDE_PATHS += -I/usr/local/include/libusb-1.0
LDLIBS += -L. -L/usr/local/lib -lusb-1.0 -ldl
endif


##########################################################################
# Generic multi-project files
##########################################################################

##########################################################################
# Project-specific files
##########################################################################

# Main Files
# ==========

ORBTRACELIB_CFILES = $(App_DIR)/orbtraceIf.c

ORBTRACE_CFILES = $(App_DIR)/$(ORBTRACE).c

##########################################################################
# GNU GCC compiler prefix and location
##########################################################################

ASTYLE = astyle
AS = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
GET_GIT_HASH = Tools/git_hash_to_c/git_hash_to_c.sh
MAKE = make

##########################################################################
# Quietening
##########################################################################

ifeq ($(VERBOSE),1)
cmd = $1
Q :=
else
cmd = @$(if $(value 2),echo "$2";)$1
Q := @
endif

HOST=-lc -lusb

##########################################################################
# Compiler settings, parameters and flags
##########################################################################
# filename for embedded git revision
GIT_HASH_FILENAME=git_version_info.h

CFLAGS +=  $(ARCH_FLAGS) $(STARTUP_DEFS) $(OPT_LEVEL) $(DEBUG_OPTS) \
           -ffunction-sections -fdata-sections -Wall -Wno-unused-result $(INCLUDE_PATHS) \
           -include $(SCREEN_PALETTE) $(GCC_DEFINE)
ASFLAGS += -c $(DEBUG_OPTS) $(INCLUDE_PATHS) $(ARCH_FLAGS) $(GCC_DEFINE) \
           -x assembler-with-cpp
LDFLAGS += $(CFLAGS)

OCFLAGS += --strip-unneeded

# Generic Stuff
OBJS =  $(patsubst %.c,%.o,$(CFILES)) $(patsubst %.s,%.o,$(SFILES))
POBJS = $(patsubst %,$(OLOC)/%,$(OBJS))
PDEPS = $(POBJS:.o=.d)

# Per Target Stuff
ORBTRACELIB_OBJS =  $(OBJS) $(patsubst %.c,%.o,$(ORBTRACELIB_CFILES))
ORBTRACELIB_POBJS = $(POJBS) $(patsubst %,$(OLOC)/%,$(ORBTRACELIB_OBJS))
ORBTRACELIB_PDEPS = $(PDEPS) $(ORBTRACELIB_POBJS:.o=.d)

ORBTRACE_OBJS =  $(OBJS) $(patsubst %.c,%.o,$(ORBTRACE_CFILES))
ORBTRACE_POBJS = $(POJBS) $(patsubst %,$(OLOC)/%,$(ORBTRACE_OBJS))
ORBTRACE_PDEPS = $(PDEPS) $(ORBTRACE_POBJS:.o=.d)

CFILES += $(App_DIR)/generics.c

##########################################################################
##########################################################################
##########################################################################

all : build

get_version:
	$(Q)mkdir -p $(OLOC)
	$(Q)$(GET_GIT_HASH) > $(OLOC)/$(GIT_HASH_FILENAME)

$(OLOC)/%.o : %.c
	$(Q)mkdir -p $(basename $@)
	$(call cmd, \$(CC) -c $(CFLAGS) -MMD -o $@ $< ,\
	Compiling $<)

build: $(ORBTRACE)

$(ORBTRACELIB) : get_version $(ORBTRACELIB_POBJS)
	$(Q)$(AR) rcs $(OLOC)/lib$(ORBTRACELIB).a  $(ORBTRACELIB_POBJS)
	-@echo "Completed build of" $(ORBTRACELIB)

$(ORBTRACE) : get_version $(ORBTRACELIB) $(ORBTRACE_POBJS)
	$(Q)$(LD) $(LDFLAGS) -o $(OLOC)/$(ORBTRACE) $(MAP) $(ORBTRACE_POBJS) $(LDLIBS)
	-@echo "Completed build of" $(ORBTRACE)

tags:
	-@etags $(CFILES) 2> /dev/null

clean:
	-$(call cmd, \rm -f $(POBJS) $(LD_TEMP) $(ORBTRACE) $(OUTFILE).map $(EXPORT) ,\
	Cleaning )
	$(Q)-rm -rf SourceDoc/*
	$(Q)-rm -rf *~ core
	$(Q)-rm -rf $(OLOC)/*
	$(Q)-rm -rf config/*~
	$(Q)-rm -rf TAGS

$(generated_dir)/git_head_revision.c:
	mkdir -p $(dir $@)
	../Tools/git_hash_to_c.sh > $@

doc:
	doxygen $(DOXCONFIG)

print-%:
	@echo $* is $($*)

pretty:
	$(Q)-$(ASTYLE) --options=config/astyle.conf "Inc/*.h" "Src/*.c"

-include $(PDEPS)
