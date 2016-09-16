#!/bin/bash
DFUUTIL=dfu-util
$DFUUTIL -d 2b04:d006 -a 0 -s 0x8020000:leave -D build/target/main/platform-6/opensound.bin
