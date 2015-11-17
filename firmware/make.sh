#!/bin/bash
export INCLUDE_DIRS=`cat modules/paths|tr '\n' ' '`
export MODULAR_FIRMWARE=n
export MODULAR=n
# export USE_SWD=y
# export DEBUG_BUILD=y
export PLATFORM=photon
export SPARK_CLOUD=n
export GCC_PREFIX=~/devel/OwlWare.sysex/Tools/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi- 
export APP=opensound
make $*

