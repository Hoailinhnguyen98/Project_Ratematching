# -----------------------------------------------------------------------------
# Title:    : 
# Project   : 
# Filename  : Makefile
# ----------------------------------------------------------------------------
# Author    : Nguyen Canh Trung
# Email     : nguyencanhtrung 'at' me 'dot' com
# Date      : 2024-10-10 10:08:09
# Last Modified : 2024-10-10 16:48:29
# Modified By   : Nguyen Canh Trung
# ----------------------------------------------------------------------------
# Description: 
# 	1. Installing systemC library
# ----------------------------------------------------------------------------
# HISTORY:
# Date      	By	Comments
# ----------	---	---------------------------------------------------------
# 2024-10-10	NCT	File created!
# -----------------------------------------------------------------------------

UBUNTU_VERSION := $(shell lsb_release -rs)

VIVADO_VERSION := 2022.2

.PHONY: help

help: 						## Show help for each of the Makefile recipes
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' Makefile | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[1;36m%-30s\033[0m %s\n", $$1, $$2}'

# systemc library
workspace/downloads/systemc-2.3.3.tar.gz:
	mkdir -p ./workspace/downloads
	curl https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz -o $@.tmp
	mv $@.tmp $@

lib/systemc-2.3.3/objdir/timestamp.txt: workspace/downloads/systemc-2.3.3.tar.gz
	if [ ! -d "./lib/systemc-2.3.3" ]; then tar -xvzf ./workspace/downloads/systemc-2.3.3.tar.gz -C ./lib; fi
	cd ./lib/systemc-2.3.3 && mkdir -p objdir && cd objdir && CXXFLAGS=-std=c++11 ../configure --prefix=$(PWD)/lib/systemc-2.3.3 && make -j4 && make install
	date >$@

systemc: lib/systemc-2.3.3/objdir/timestamp.txt 	## Setup the environment variables for SystemC
	@echo "Run 'source ./scripts/setup-env.sh' to set the environment variables in your current terminal session."

