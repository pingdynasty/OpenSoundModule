export INCLUDE_DIRS=`tr '\n' ' '< paths`
export MODULAR_FIRMWARE=n 
export MODULAR=n
export PLATFORM=photon
export GCC_PREFIX=~/devel/OwlWare.sysex/Tools/gcc-arm-none-eabi-4_9-2015q2/bin/arm-none-eabi- 
export APP=opensound
make $*

