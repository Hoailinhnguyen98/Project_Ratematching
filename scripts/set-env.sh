#!/bin/sh
export SYSTEMC_HOME=$(pwd)/lib/systemc-2.3.3
export SYSTEMC_INCLUDE=$SYSTEMC_HOME/include
export SYSTEMC_LIBDIR=$SYSTEMC_HOME/lib-linux64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SYSTEMC_HOME/lib-linux64
echo "Environment variables set for SystemC"
echo "SYSTEMC_HOME: $SYSTEMC_HOME"
echo "SYSTEMC_INCLUDE: $SYSTEMC_INCLUDE"
echo "SYSTEMC_LIBDIR: $SYSTEMC_LIBDIR"