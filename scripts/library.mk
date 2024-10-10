# -----------------------------------------------------------------------------
# Title:    : 
# Project   : 
# Filename  : library
# ----------------------------------------------------------------------------
# Author    : Nguyen Canh Trung
# Email     : nguyencanhtrung 'at' me 'dot' com
# Date      : 2024-08-15 17:51:58
# Last Modified : 2024-09-25 01:12:52
# Modified By   : Nguyen Canh Trung
# ----------------------------------------------------------------------------
# Description: 
# 
# ----------------------------------------------------------------------------
# HISTORY:
# Date      	By	Comments
# ----------	---	---------------------------------------------------------
# 2024-08-15	NCT	File created!
# -----------------------------------------------------------------------------

## ***** Basic configurations ***************************************
## Variable that points to SystemC installation path
PRJ_DIR ?= $(PWD)

SYSTEMC_HOME ?= $(PRJ_DIR)/lib/systemc-2.3.3

## Select the target architecture
TARGET_ARCH ?= linux64

# default compiler flags
FLAGS_COMMON = -g -Wall
FLAGS_STRICT += -pedantic -Wno-long-long
FLAGS_WERROR = -Werror

# Run with valgrind (Mem leak detection)
# VALGRIND=valgrind --log-file=valgrind.log

## default values for additional setup variables
ifneq (,$(strip $(TARGET_ARCH)))
ARCH_SUFFIX      ?= -$(TARGET_ARCH)
endif

## How to instruct the dynamic linker to locate the SystemC library
LDFLAG_RPATH     ?= -Wl,-rpath=

# Explicit location of the SystemC headers
SYSTEMC_INC_DIR  ?= $(SYSTEMC_HOME)/include

# Explicit location if the SystenC library
SYSTEMC_LIB_DIR  ?= $(SYSTEMC_HOME)/lib$(ARCH_SUFFIX)

# Additional preprocessor symbols
SYSTEMC_DEFINES  ?=

# combined flags
SYSTEMC_CXXFLAGS ?= $(FLAGS_COMMON) $(FLAGS_STRICT) $(FLAGS_WERROR)
SYSTEMC_LDFLAGS  ?= -L $(SYSTEMC_LIB_DIR) \
                    $(LDFLAG_RPATH)$(SYSTEMC_LIB_DIR)
SYSTEMC_LIBS     ?= -lsystemc -lm

## Add 'PTHREADS=1' to command line for a pthreads build
## (should not be needed in most cases)
ifdef PTHREADS
SYSTEMC_CXXFLAGS += -pthread
SYSTEMC_LIBS     += -lpthread
endif
## ***** End Basic configurations ***************************************

FILTER ?= cat

INCDIR   += -I. -I.. -I$(SYSTEMC_INC_DIR)
LIBDIR   += -L. -L..

CXXFLAGS  += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(INCDIR) $(SYSTEMC_DEFINES)
LDFLAGS   += $(CFLAGS) $(SYSTEMC_CXXFLAGS) $(LIBDIR) $(SYSTEMC_LDFLAGS)
LIBS      += $(SYSTEMC_LIBS) $(EXTRA_LIBS)

# "real" Makefile needs to set PROJECT
ifeq (,$(strip $(PROJECT)))
$(error PROJECT not set. Cannot build.)
endif

# basic check for SystemC directory
ifeq (,$(wildcard $(SYSTEMC_HOME)/.))
$(error SYSTEMC_HOME [$(SYSTEMC_HOME)] is not present. \
        Please update ./scripts/library.mk)
endif
ifeq (,$(wildcard $(SYSTEMC_INC_DIR)/systemc.h))
$(error systemc.h [$(SYSTEMC_INC_DIR)] not found. \
        Please update ./scripts/library.mk)
endif
ifeq (,$(wildcard $(SYSTEMC_LIB_DIR)/libsystemc*))
$(error SystemC library [$(SYSTEMC_LIB_DIR)] not found. \
        Please update ./scripts/library.mk)
endif

## ***************************************************************************
## build rules

SCRIPTS_PATH := $(subst scripts/library.mk,,$(lastword $(MAKEFILE_LIST)))

.SUFFIXES: .cc .cpp .o .x

GOLDEN?=$(firstword $(wildcard ../results/expected.log golden.log))
EXEEXT?=.x
EXE   := $(PROJECT)$(EXEEXT)

.PHONY: help

help: 						## Show help for each of the Makefile recipes
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(SCRIPTS_PATH)/scripts/library.mk | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[1;36m%-30s\033[0m %s\n", $$1, $$2}'

all: announce build 		## Compile project

announce:
	@if test x1 = x$(FLAG_BATCH) ; then \
		echo; echo "*** $(PROJECT):"; echo; \
	fi

check: announce all
	@if test -f "$(INPUT)" ; then INPUT="< $(INPUT)" ; fi ; \
		eval "$(VALGRIND) ./$(EXE) $(ARGS) $${INPUT} > run.log"
	@cat run.log | grep -v "stopped by user" | \
		$(FILTER) | awk '{if($$0!="") print $$0}' > run_trimmed.log
	@if test -f "$(GOLDEN)" ; then \
	  cat "$(GOLDEN)" | grep -v "stopped by user" | \
		awk '{if($$0!="") print $$0}' > ./expected_trimmed.log ; \
	  diff ./run_trimmed.log ./expected_trimmed.log > diff.log 2>&1 ; \
	  if [ -s diff.log ]; then \
	    echo "***ERROR:"; cat diff.log; \
	  else echo "OK"; fi \
	fi

run: announce all 			## Run executable
	@if test -f "$(INPUT)" ; then INPUT="< $(INPUT)" ; fi ; \
		eval "./$(EXE) $(ARGS) $${INPUT}"

build: announce $(EXE)

$(EXE): $(OBJS) $(SYSTEMC_LIB_DIR)/libsystemc.a
	@echo "Compiling..."
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) 2>&1 | tee compile.log | c++filt
	@test -x $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@


CLEAN_TARGETS := *.vcd
CLEAN_TARGETS += *.log
CLEAN_TARGETS += $(EXE)
CLEAN_TARGETS += $(OBJS)
CLEAN_TARGETS += *.wlf

.PHONY: clean

clean:: announce 			## Clean the project
	rm -f $(CLEAN_TARGETS) $(EXTRA_CLEAN) 



GTKWAVE_EXISTS := $(shell command -v gtkwave 2>/dev/null)
# GTKWAVE_EXISTS := $(shell which gtkwave 2>/dev/null)
# ifeq ($(GTKWAVE_EXISTS),)
#   $(error gtkwave is not found)
# endif

.PHONY: view

check_gtk:
	@if test -z "$(GTKWAVE_EXISTS)"; then echo "Error: Please install gtkwave"; exit 1; fi

tb.vcd: announce all					## View waveform with gtkwave

view: announce check_gtk tb.vcd			## View waveform with gtkwave
	gtkwave *.vcd &

questa: announce tb.vcd					## View waveform with QuestaSim
	vcd2wlf tb.vcd tb.wlf;
	vsim -view tb.wlf;



.PHONY: debug

debug: announce $(EXE)					## Debug with GDB
	gdb $(EXE)
