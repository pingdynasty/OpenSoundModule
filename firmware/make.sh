#!/bin/bash
export INCLUDE_DIRS=`cat modules/paths|tr '\n' ' '`
export MODULAR_FIRMWARE=n
export MODULAR=n
# export USE_SWD=y
# export DEBUG_BUILD=y
export PLATFORM=photon
export SPARK_CLOUD=n
export GCC_PREFIX=~/devel/OwlWare/Tools/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi- 
# export GCC_PREFIX=arm-none-eabi- 
export APP=opensound
export WARNINGS_AS_ERRORS=n
make $*

