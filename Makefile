SDCARD	:= ./sdcard.img

all:
	ca65 -t cx16 -o comminda.o commandermind.asm
	cl65 -t cx16 -m commandermind.map -Ln commandermind.sym -o COMMANDERMIND.PRG commandermind.c comminda.o
clean:
	rm -rf *.PRG *.o *.map *.sym *.zip

sdcard:
	rm $(SDCARD)
	truncate -s 34M $(SDCARD)
	/sbin/parted -s $(SDCARD) mklabel msdos mkpart primary fat32 2048s -- -1
	mformat -i $(SDCARD)@@1M -F
	mcopy -v -i $(SDCARD)@@1M -o -m *.PRG *.BIN *.ZSM ::

zip:
	zip commandermind.zip *.PRG *.BIN *.ZSM

