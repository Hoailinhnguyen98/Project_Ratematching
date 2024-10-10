# -----------------------------------------------------------------------------
# Title:    : 
# Project   : 
# Filename  : Makefile
# ----------------------------------------------------------------------------
# Author    : Nguyen Canh Trung
# Email     : nguyencanhtrung 'at' me 'dot' com
# Date      : 2024-08-12 14:38:26
# Last Modified : 2024-10-10 18:15:52
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

PROJECT = ratematching

# Include all source files
#SRCS    = $(wildcard *.cc)

# SRCS = sys_tb.cc
# SRCS += ldpc_enc.cc
# SRCS += sc_main.cc

SRCS = $(wildcard *.cpp)

OBJS    = $(SRCS:.cpp=.o)
PTHREADS =

## Enable to print project name 
FLAG_BATCH ?= 1
FLAGS_STRICT = -Wno-unused-variable

EXTRA_LIBS = 
EXTRA_CLEAN =  Ratematching.o 


include ../scripts/library.mk
