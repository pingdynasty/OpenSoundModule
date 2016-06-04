#!/bin/bash
DIR=/tmp
echo -e -n "\x09OpenSound" > $DIR/ssid_prefix
SERIAL=`apg -a 1 -M nc -n 1 -m 4 -x 4 -E GHIJKLMNOPQRSTUVWXYZ`
echo "Generated device serial: $SERIAL"
echo -e -n "\x04$SERIAL" > $DIR/device_serial
echo -e -n "\x03osm\x00" > $DIR/dns_redirect
echo -e -n "\x03osm\x05local\x00" > $DIR/dns_redirect
echo -e -n "\x03www\x09rebeltech\x03org\x00" >> $DIR/dns_redirect
echo -e -n "\x00" >> $DIR/dns_redirect
sudo dfu-util -d 2b04:d006 -a 1 -s 1826 -D $DIR/ssid_prefix
sudo dfu-util -d 2b04:d006 -a 1 -s 1852:4 -D $DIR/device_serial
#sudo dfu-util -d 2b04:d006 -a 1 -s 1890 -D $DIR/dns_redirect
