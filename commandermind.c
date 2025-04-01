#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "commandermind.h"

void seethroughtile() {
	int cnt;

	*(u8*)VERA_ADDR_H = 0x10;
	*(u8*)VERA_ADDR_M = 0xA0;
	*(u8*)VERA_ADDR_L = 0xC0;
	for (cnt=0; cnt<32; cnt++) *(u8*)VERA_DATA0 = 0x00;
}

void gotoxy(u8 x, u8 y) {
	*(u8*)VERA_ADDR_H = *(u8*)VERA_ADDR_H|0x01;
	*(u8*)VERA_ADDR_M = 0xB0+y;
	*(u8*)VERA_ADDR_L = x*2;
}

int main() {
	int x, y;

	*(u8*)VERA_DC_HSCALE = 0x40;
	*(u8*)VERA_DC_VSCALE = 0x40;

	vload("bgimg.bin", 0x0000, 0);
	vload("tiles.bin", 0x9800, 0);
	seethroughtile();

	*(u8*)VERA_DC_VIDEO = *(u8*)VERA_DC_VIDEO|0x10; //Enable Layer 0
	*(u8*)VERA_L0_CONFIG = 0x06; //bitmap mode, 4bpp color
	*(u8*)VERA_L0_TILEBASE = 0x00; //start address = 0

	// map height = 0 = 32 tiles
	// map width = 2 = 128 tiles
	// color depth = 2 = 4 bpp
	*(u8*)VERA_L1_CONFIG = 0b00100010;

	// tile width = 0 = 8 pixels
	// tile height = 0 = 8 pixels
	// tile_base = address of tile data = 0x9600
	*(u8*)VERA_L1_TILEBASE = 0x4C;

	*(u8*)VERA_ADDR_H = 0x11;
	*(u8*)VERA_ADDR_M = 0xB0;
	*(u8*)VERA_ADDR_L = 0x00;


	for (y=0;y<30;y++) {
		for(x=0;x<40;x++) {
			*(u8*)VERA_DATA0 = 70;
			*(u8*)VERA_DATA0 = 0;
		}
		*(u8*)VERA_ADDR_L = 0x00;
		*(u8*)VERA_ADDR_M = *(u8*)VERA_ADDR_M+1;
	}

	*(u8*)VERA_ADDR_H = 0x21;

	gotoxy(1, 1);
	*(u8*)VERA_DATA0 = 2;
	*(u8*)VERA_DATA0 = 3;
	gotoxy(1, 2);
	*(u8*)VERA_DATA0 = 4;
	*(u8*)VERA_DATA0 = 5;


	while(1);

	return (0);
}
