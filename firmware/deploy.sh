#!/bin/bash
DFUUTIL=dfu-util
$DFUUTIL -d 2b04:d006 -a 0 -s 0x8020000:leave -D binaries/opensound-v0.9.bin
