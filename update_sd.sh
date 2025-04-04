#!/bin/bash
if [ "$1" = "" ]; then SDCARD="sdcard.img"; else SDCARD="$1"; fi
echo "Mounting sdcard image"
mkdir sd
sudo mount -o uid=1000,loop,offset=1048576 sdcard.img sd
echo "Deleting all files on card..."
rm -rf sd/*
echo "Copying files..."
cp -rv COMMIND.PRG BGIMG.BIN TILES.BIN sd/
echo "Unmounting sdcard image"
sudo umount sd
echo "Deleting device for sdcard image"
rm -r sd
