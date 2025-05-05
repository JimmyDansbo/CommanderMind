#ifndef _commandermind_h_
#define _commandermind_h_

typedef unsigned char u8;
typedef char s8;
typedef unsigned u16;
typedef int s16;
typedef unsigned long u32;
typedef long s32;

extern void __fastcall__ dbg8(char var);
extern void __fastcall__ dbg16(u16 var);
extern void __fastcall__ breakpoint();
extern char __fastcall__ vload(char *str, unsigned int addr, char bank);
extern void __fastcall__ enamouse();
extern u16  __fastcall__ getmouse(char zp);
extern char __fastcall__ rndcircle();
extern void __fastcall__ returntobasic();
extern char __fastcall__ bload(char *str, unsigned int addr, char bank);
extern void __fastcall__ initzsm();
extern void __fastcall__ zsmplay(u8 priority);
extern void __fastcall__ zsmstop(u8 priority);
extern void __fastcall__ zsm_setbank(u8 slot, u8 bank);
extern void __fastcall__ zsm_setmem(u8 slot, u16 addr);
extern void __fastcall__ zsm_rewind(u8 priority);
extern void __fastcall__ zsm_setloop(u8 priority, u8 loop);

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

#define VERA_ADDR	(*(u16*)0x9F20)
#define VERA_ADDR_L	(*(u8*)0x9F20)
#define VERA_ADDR_M	(*(u8*)0x9F21)
#define VERA_ADDR_H	(*(u8*)0x9F22)
#define VERA_DATA0	(*(u8*)0x9F23)
#define VERA_DATA1	(*(u8*)0x9F24)
#define VERA_CTRL	(*(u8*)0x9F25)
#define VERA_IEN	(*(u8*)0x9F26)
#define VERA_ISR	(*(u8*)0x9F27)
#define VERA_IRQLINE_L	(*(u8*)0x9F28)
#define VERA_SCANLINE_L	(*(u8*)0x9F28)

#define VERA_DC_VIDEO	(*(u8*)0x9F29)	// DCSEL=0
#define VERA_DC_HSCALE	(*(u8*)0x9F2A)
#define VERA_DC_VSCALE	(*(u8*)0x9F2B)
#define VERA_DC_BORDER	(*(u8*)0x9F2C)

#define VERA_DC_HSTART	(*(u8*)0x9F29)	// DCSEL=1
#define VERA_DC_HSTOP	(*(u8*)0x9F2A)
#define VERA_DC_VSTART	(*(u8*)0x9F2B)
#define VERA_DC_VSTOP	(*(u8*)0x9F2C)

#define VERA_FX_CTRL	 (*(u8*)0x9F29)	// DCSEL=2
#define VERA_FX_TILEBASE (*(u8*)0x9F2A)
#define VERA_FX_MAPBASE  (*(u8*)0x9F2B)
#define VERA_FX_MULT	 (*(u8*)0x9F2C)

#define VERA_FX_X_INCR_L (*(u8*)0x9F29)	// DCSEL=3
#define VERA_FX_X_INCR_H (*(u8*)0x9F2A)
#define VERA_FX_Y_INCR_L (*(u8*)0x9F2B)
#define VERA_FX_Y_INCR_H (*(u8*)0x9F2C)

#define VERA_FX_X_POS_L	 (*(u8*)0x9F29)	// DCSEL=4
#define VERA_FX_X_POS_H	 (*(u8*)0x9F2A)
#define VERA_FX_Y_POS_L	 (*(u8*)0x9F2B)
#define VERA_FX_Y_POS_H	 (*(u8*)0x9F2C)

//......

#define VERA_DC_VER0	(*(u8*)0x9F29)	// DCSEL=63
#define VERA_DC_VER1	(*(u8*)0x9F2A)
#define VERA_DC_VER2	(*(u8*)0x9F2B)
#define VERA_DC_VER3	(*(u8*)0x9F2C)

#define VERA_L0_CONFIG	  (*(u8*)0x9F2D)
#define VERA_L0_MAPBASE	  (*(u8*)0x9F2E)
#define VERA_L0_TILEBASE  (*(u8*)0x9F2F)
#define VERA_L0_HSCROLL_L (*(u8*)0x9F30)
#define VERA_L0_HSCROLL_H (*(u8*)0x9F31)
#define VERA_L0_VSCROLL_L (*(u8*)0x9F32)
#define VERA_L0_VSCROLL_H (*(u8*)0x9F33)

#define VERA_L1_CONFIG	  (*(u8*)0x9F34)
#define VERA_L1_MAPBASE	  (*(u8*)0x9F35)
#define VERA_L1_TILEBASE  (*(u8*)0x9F36)
#define VERA_L1_HSCROLL_L (*(u8*)0x9F37)
#define VERA_L1_HSCROLL_H (*(u8*)0x9F38)
#define VERA_L1_VSCROLL_L (*(u8*)0x9F39)
#define VERA_L1_VSCROLL_H (*(u8*)0x9F3A)

#define VERA_AUDIO_CTRL	(*(u8*)0x9F3B)
#define VERA_AUDIO_RATE	(*(u8*)0x9F3C)
#define VERA_AUDIO_DATA	(*(u8*)0x9F3D)
#define VERA_SPI_DATA	(*(u8*)0x9F3E)
#define VERA_SPI_CTRL	(*(u8*)0x9F3F)

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

#define TMP0		0x0030
#define	TMP1		0x0031
#define TMP2		0x0032
#define TMP3		0x0033
#define TMP4		0x0034
#define TMP5		0x0035
#define TMP6		0x0036
#define TMP7		0x0037
#define TMP8		0x0038
#define TMP9		0x0039
#define TMPa		0x003A
#define TMPb		0x003B
#define TMPc		0x003C
#define TMPd		0x003D
#define TMPe		0x003E
#define TMPf		0x003F
#define TMP_PTR0	TMP0
#define TMP_PTR1	TMP2
#define TMP_PTR2	TMP4
#define TMP_PTR3	TMP6
#define TMP_PTR4	TMP8
#define TMP_PTR5	TMPa
#define TMP_PTR6	TMPc
#define TMP_PTR7	TMPe


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
	unsigned char paletteoffset;
	unsigned char width;
	unsigned char height;
};

struct _lineinfo {
	unsigned char isDone;
	unsigned char tiley;
	unsigned char miny;
	unsigned char maxy;
	unsigned char fieldcolor[4];
};

#endif
