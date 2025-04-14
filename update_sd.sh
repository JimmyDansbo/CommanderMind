#!/bin/bash
if [ "$1" = "" ]; then SDCARD="sdcard.img"; else SDCARD="$1"; fi
echo "Removing and creating new sdcard image file"
rm $SDCARD
truncate -s 34M $SDCARD
echo "Creating partition and formatting to FAT32"
parted -s $SDCARD mklabel msdos mkpart primary fat32 2038s -- -1
mformat -i $SDCARD@@1M -F
echo "Copying files..."
mcopy -i $SDCARD@@1M -o -m COMMIND.PRG ::
mcopy -i $SDCARD@@1M -o -m BGIMG.BIN ::
mcopy -i $SDCARD@@1M -o -m SPLASH.BIN ::
mcopy -i $SDCARD@@1M -o -m TILES.BIN ::
mcopy -i $SDCARD@@1M -o -m MUSIC.ZSM ::
mcopy -i $SDCARD@@1M -o -m ZSMKIT-A000.BIN ::
