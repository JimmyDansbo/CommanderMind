#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "commandermind.h"



int main() {
	vload("bgimg.bin", 0x0000, 0);

	*(u8*)VERA_DC_VIDEO = *(u8*)VERA_DC_VIDEO|0x10; //Enable Layer 0
	*(u8*)VERA_L0_CONFIG = 0x06; //bitmap mode, 4bpp color
	*(u8*)VERA_L0_TILEBASE = 0x00; //start address = 0

	return (0);
}
