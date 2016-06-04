#!/bin/bash
export INCLUDE_DIRS=`cat modules/paths|tr '\n' ' '`
export MODULAR_FIRMWARE=n
export MODULAR=n
# export USE_SWD=y
# export DEBUG_BUILD=y
export PLATFORM=photon
export SPARK_CLOUD=n
export GCC_PREFIX=~/devel/OwlWare/Tools/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-
export GCC_PREFIX=~/devel/OwlWare/Tools/gcc-arm-none-eabi-5_2-2015q4/bin/arm-none-eabi-
export APP=opensound
make $*

