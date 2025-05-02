#ifndef _SPLASHPALETTE_H_
#define _SPLASHPALETTE_H_

u16 splashpalette[16] = {0x0000,0x0015,0x0C20,0x0A31,
			 0x0655,0x027A,0x0182,0x0D51,
			 0x0293,0x038C,0x0888,0x0D83,
			 0x04BF,0x0EA2,0x0FD2,0x0FFF};

#define SPLASH_BASE 0x80
#define SPLASH_PAL_OFFSET 1

/******************************************************************************
 Copy the palette of the splash image to VRAM
******************************************************************************/
void setsplashpal() {
	u16 vaddr;
	u8 haddr, cnt;

	// Save VERA address
	vaddr = VERA_ADDR;
	haddr = VERA_ADDR_H;

	// Set VERA address to 4bpp palette 1
	VERA_ADDR_H = 0x11;
	VERA_ADDR = 0xFA00+(16*2);
	// Copy the palette to VRAM
	for (cnt=0; cnt<16; cnt++) {
		VERA_DATA0 = splashpalette[cnt]&0x00FF;
		VERA_DATA0 = (splashpalette[cnt]>>8)&0x00FF;
	}

	// Restore the VERA address
	VERA_ADDR = vaddr;
	VERA_ADDR_H = haddr;
}

#endif
