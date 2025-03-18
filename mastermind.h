#ifndef _x16maze_h_
#define _x16maze_h_

#define BLACK		0x00
#define WHITE		0x01
#define RED		0x02
#define CYAN		0x03
#define PURPLE		0x04
#define GREEN		0x05
#define BLUE		0x06
#define YELLOW		0x07
#define ORANGE		0x08
#define BROWN		0x09
#define PINK		0x0A
#define DARKGRAY	0x0B
#define MIDGRAY		0x0C
#define LIGHTGREEN	0x0D
#define LIGHTBLUE	0x0E
#define LIGHTGRAY	0x0F
#define WALLCOL		LIGHTGRAY

// Joystick button values
#define SNES_B		0x80
#define SNES_Y		0x40
#define SNES_SEL	0x20
#define SNES_STA	0x10
#define SNES_UP		0x08
#define SNES_DN		0x04
#define SNES_LT		0x02
#define SNES_RT		0x01
#define SNES_A		0x80
#define SNES_X		0x40
#define SNES_L		0x20
#define SNES_R		0x10

#define	RAM_BANK	0x0000
#define ROM_BANK	0x0001

#define VERA_ADDR	0x9F20
#define VERA_ADDR_HI	0x9F22
#define VERA_DATA0	0x9F23
#define VERA_DATA1	0x9F24
#define VERA_CTRL	0x9F25
#define VERA_IEN	0x9F26
#define VERA_ISR	0x9F27
#define VERA_IRQLINE_L	0x9F28
#define VERA_SCANLINE_L	0x9F28

#define VERA_DC_VIDEO	0x9F29	// DCSEL=0
#define VERA_DC_HSCALE	0x9F2A
#define VERA_DC_VSCALE	0x9F2B
#define VERA_DC_BORDER	0x9F2C

#define VERA_DC_HSTART	0x9F29	// DCSEL=1
#define VERA_DC_HSTOP	0x9F2A
#define VERA_DC_VSTART	0x9F2B
#define VERA_DC_VSTOP	0x9F2C

#define VERA_FX_CTRL	0x9F29	// DCSEL=2
#define VERA_FX_TILEBASE 0x9F2A
#define VERA_FX_MAPBASE 0x9F2B
#define VERA_FX_MULT	0x9F2C

#define VERA_FX_X_INCR_L 0x9F29	// DCSEL=3
#define VERA_FX_X_INCR_H 0x9F2A
#define VERA_FX_Y_INCR_L 0x9F2B
#define VERA_FX_Y_INCR_H 0x9F2C

#define VERA_FX_X_POS_L	0x9F29	// DCSEL=4
#define VERA_FX_X_POS_H	0x9F2A
#define VERA_FX_Y_POS_L	0x9F2B
#define VERA_FX_Y_POS_H	0x9F2C

//......

#define VERA_DC_VER0	0x9F29	// DCSEL=63
#define VERA_DC_VER1	0x9F2A
#define VERA_DC_VER2	0x9F2B
#define VERA_DC_VER3	0x9F2C

#define VERA_L0_CONFIG	0x9F2D
#define VERA_L0_MAPBASE	0x9F2E
#define VERA_L0_TILEBASE 0x9F2F
#define VERA_L0_HSCROLL_L 0x9F30
#define VERA_L0_HSCROLL_H 0x9F31
#define VERA_L0_VSCROLL_L 0x9F32
#define VERA_L0_VSCROLL_H 0x9F33

#define VERA_L1_CONFIG	0x9F34
#define VERA_L1_MAPBASE	0x9F35
#define VERA_L1_TILEBASE 0x9F36
#define VERA_L1_HSCROLL_L 0x9F37
#define VERA_L1_HSCROLL_H 0x9F38
#define VERA_L1_VSCROLL_L 0x9F39
#define VERA_L1_VSCROLL_H 0x9F3A

#define VERA_AUDIO_CTRL	0x9F3B
#define VERA_AUDIO_RATE	0x9F3C
#define VERA_AUDIO_DATA	0x9F3D
#define VERA_SPI_DATA	0x9F3E
#define VERA_SPI_CTRL	0x9F3F

extern void __fastcall__ dummy(char var);

#define SPRITE_MODE_4BPP	0
#define SPRITE_MODE_8BPP	1

#define SPRITE_WIDTH_8		0
#define SPRITE_WIDTH_16		1
#define SPRITE_WIDTH_32		2
#define SPRITE_WIDTH_64		3

#define SPRITE_HEIGHT_8		0
#define SPRITE_HEIGHT_16	1
#define SPRITE_HEIGHT_32	2
#define SPRITE_HEIGHT_64	3

struct _spriteattributes {
	unsigned int address;
	unsigned char address_hi;
	unsigned char mode;
	unsigned int x;
	unsigned int y;
	unsigned char vflip;
	unsigned char hflip;
	unsigned char zdepth;
	unsigned char collisionmask;
	unsigned char palletteoffset;
	unsigned char width;
	unsigned char height;
};

#endif
