# Open Sound Module Firmware

## Prerequisites
* arm gcc compiler [1]
* make and bash (to run the build scripts)
* dfu-util (to flash firmware to device) [2] 

## Build
* edit make.sh to set the location of your arm-gcc installation, e.g.
`export GCC_PREFIX=~/Tools/gcc-arm-none-eabi-5_3-2016q1/bin/arm-none-eabi-`
* run the make script from the `firmware` directory: `./make.sh`

## Flash
* connect a USB cable to the Open Sound Module and enter Photon DFU mode [3]
* run the flash script from the `firmware` directory: `./flash.sh`

## LICENCE
Unless otherwise stated, all files in the directory `user/applications/opensound` are licensed under a GPLv2 licence.
Unless otherwise stated, all other files are licensed under an LGPLv3 licence.

## References
[1] https://launchpad.net/gcc-arm-embedded
[2] http://dfu-util.sourceforge.net/
[3] https://docs.particle.io/guide/getting-started/modes/photon/
