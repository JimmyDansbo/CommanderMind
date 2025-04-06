#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "commandermind.h"

u16 mousex=0;
u16 mousey=0;
u8 sprite=0;
u8 btnpressed=0;
u8 btnenabled=0;
struct _lineinfo lineinfo[10];
u8 combination[4];

#define RED_CIRCLE	1
#define YELLOW_CIRCLE	5
#define BLUE_CIRCLE	9
#define GREEN_CIRCLE	13
#define PURPLE_CIRCLE	17
#define CYAN_CIRCLE	21

#define FIELDMINX0 8
#define FIELDMINX1 32
#define FIELDMINX2 56
#define FIELDMINX3 80
#define FIELDMAXX0 23
#define FIELDMAXX1 47
#define FIELDMAXX2 71
#define FIELDMAXX3 95

#define FIELDTX0 1
#define FIELDTX1 4
#define FIELDTX2 7
#define FIELDTX3 10

void preplines() {
	u8 cnt=0;
	u8 tiley=1;
	u8 miny = 8;
	u8 maxy = miny+15;

	for (cnt=0;cnt<10;cnt++) {
		lineinfo[cnt].fieldcolor[0]=0;
		lineinfo[cnt].fieldcolor[1]=0;
		lineinfo[cnt].fieldcolor[2]=0;
		lineinfo[cnt].fieldcolor[3]=0;
		lineinfo[cnt].isDone=0;
		lineinfo[cnt].tiley = tiley;
		lineinfo[cnt].miny = miny;
		lineinfo[cnt].maxy = maxy;
		tiley+=3;
		miny+=24;
		maxy=miny+15;
	}
}

void gotoxy(u8 x, u8 y) {
	*(u8*)VERA_ADDR_H = *(u8*)VERA_ADDR_H|0x01;
	*(u8*)VERA_ADDR_M = 0xB0+y;
	*(u8*)VERA_ADDR_L = x*2;
}

/******************************************************************************
 Set sprite attributes in VRAM
 The function ensure to set the correct VERA address and then parses the 
 information in the _spriteattributes struct to place it into the correct
 bitfields in VRAM
******************************************************************************/
static void configSprite(char spriteID, struct _spriteattributes *sa) {
	u16 oldaddr;
	u8 oldaddrhi;

	// Save VERA addresses
	oldaddr=*(u16*)VERA_ADDR;
	oldaddrhi=*(u8*)VERA_ADDR_H;

	// Set VERA address to start of spriteID
	*(u16*)VERA_ADDR=0xFC00+(spriteID*8);
	*(u8*)VERA_ADDR_H=0x11;

	*(u8*)VERA_DATA0=sa->address>>5;
	*(u8*)VERA_DATA0=(sa->mode<<7)|(sa->address_hi<<3)|(sa->address>>13);
	*(u8*)VERA_DATA0=sa->x;
	*(u8*)VERA_DATA0=sa->x>>8;
	*(u8*)VERA_DATA0=sa->y;
	*(u8*)VERA_DATA0=sa->y>>8;
	*(u8*)VERA_DATA0=(sa->collisionmask<<4)|(sa->zdepth<<2)|(sa->vflip<<1)|(sa->hflip&0x01);
	*(u8*)VERA_DATA0=(sa->height<<6)|(sa->width<<4)|(sa->palletteoffset&0x0F);

	// Restore VERA address
	*(u8*)VERA_ADDR_H=oldaddrhi;
	*(u16*)VERA_ADDR=oldaddr;
}

void set_sprite_coord(u8 spriteid, u16 x, u16 y){
	// Set VERA address to offset 2 of Sprite ID = X coordinate
	*(u16*)VERA_ADDR=(0xFC00+(spriteid*8))+2;
	*(u8*)VERA_ADDR_H=0x11;
	*(u8*)VERA_DATA0=x;
	*(u8*)VERA_DATA0=x>>8;
	*(u8*)VERA_DATA0=y;
	*(u8*)VERA_DATA0=y>>8;
}  

void configuresprites(){
	struct _spriteattributes sa;

	// Configure the sprite attributes
	sa.address_hi		= 0;
	sa.x			= 400;
	sa.y			= 195;
	sa.collisionmask	= 0;
	sa.height		= SPRITE_HEIGHT_8;
	sa.width		= SPRITE_WIDTH_8;
	sa.hflip		= 0;
	sa.vflip		= 0;
	sa.mode			= SPRITE_MODE_4BPP;
	sa.zdepth		= 3;
	sa.palletteoffset 	= 0;

	sa.address		= 0x9840;
	configSprite(1, &sa);
	sa.address		= 0x9860;
	configSprite(2, &sa);
	sa.address		= 0x9880;
	configSprite(3, &sa);
	sa.address		= 0x98A0;
	configSprite(4, &sa);

	sa.address		= 0x98C0;
	configSprite(5, &sa);
	sa.address		= 0x98E0;
	configSprite(6, &sa);
	sa.address		= 0x9900;
	configSprite(7, &sa);
	sa.address		= 0x9920;
	configSprite(8, &sa);
	
	sa.address		= 0x9940;
	configSprite(9, &sa);
	sa.address		= 0x9960;
	configSprite(10, &sa);
	sa.address		= 0x9980;
	configSprite(11, &sa);
	sa.address		= 0x99A0;
	configSprite(12, &sa);

	sa.address		= 0x99C0;
	configSprite(13, &sa);
	sa.address		= 0x99E0;
	configSprite(14, &sa);
	sa.address		= 0x9A00;
	configSprite(15, &sa);
	sa.address		= 0x9A20;
	configSprite(16, &sa);

	sa.address		= 0x9A40;
	configSprite(17, &sa);
	sa.address		= 0x9A60;
	configSprite(18, &sa);
	sa.address		= 0x9A80;
	configSprite(19, &sa);
	sa.address		= 0x9AA0;
	configSprite(20, &sa);

	sa.address		= 0x9AC0;
	configSprite(21, &sa);
	sa.address		= 0x9AE0;
	configSprite(22, &sa);
	sa.address		= 0x9B00;
	configSprite(23, &sa);
	sa.address		= 0x9B20;
	configSprite(24, &sa);
}

void showcircle(u8 circle) {
	switch (circle) {
		case RED_CIRCLE:	circle=2;  break;
		case YELLOW_CIRCLE:	circle=6;  break;
		case BLUE_CIRCLE:	circle=10; break;
		case GREEN_CIRCLE:	circle=14; break;
		case PURPLE_CIRCLE:	circle=18; break;
		case CYAN_CIRCLE:	circle=22; break;
	}
	*(u8*)VERA_DATA0 = circle;
	*(u8*)VERA_DATA0 = ++circle;
	*(u8*)VERA_ADDR_M = *(u8*)VERA_ADDR_M + 1;
	*(u8*)VERA_ADDR_L = *(u8*)VERA_ADDR_L - 4;
	*(u8*)VERA_DATA0 = ++circle;
	*(u8*)VERA_DATA0 = ++circle;
}

void enablebutton() {
	u8 cnt=0;

	btnenabled=1;
	*(u8*)VERA_ADDR_H = 0x21;
	gotoxy(20, 24);
	for (cnt=0; cnt<6; cnt++)
		*(u8*)VERA_DATA0 = 26;
	gotoxy(20, 25);
	for (cnt=0; cnt<14; cnt++)
		*(u8*)VERA_DATA0 = 26;
	gotoxy(20, 26);
	for (cnt=0; cnt<14; cnt++)
		*(u8*)VERA_DATA0 = 26;
	gotoxy(20, 27);
	for (cnt=0; cnt<16; cnt++)
		*(u8*)VERA_DATA0 = 26;
}

void grayoutbutton() {
	btnenabled=0;
	*(u8*)VERA_ADDR_H = 0x21;
	gotoxy(20, 24);
	*(u8*)VERA_DATA0 = 28;
	*(u8*)VERA_DATA0 = 26;
	*(u8*)VERA_DATA0 = 29;
	*(u8*)VERA_DATA0 = 30;
	*(u8*)VERA_DATA0 = 31;
	*(u8*)VERA_DATA0 = 32;
	gotoxy(20, 25);
	*(u8*)VERA_DATA0 = 33;
	*(u8*)VERA_DATA0 = 26;
	*(u8*)VERA_DATA0 = 34;
	*(u8*)VERA_DATA0 = 35;
	*(u8*)VERA_DATA0 = 36;
	*(u8*)VERA_DATA0 = 37;
	*(u8*)VERA_DATA0 = 38;
	*(u8*)VERA_DATA0 = 39;
	*(u8*)VERA_DATA0 = 40;
	*(u8*)VERA_DATA0 = 41;
	*(u8*)VERA_DATA0 = 42;
	*(u8*)VERA_DATA0 = 43;
	*(u8*)VERA_DATA0 = 44;
	*(u8*)VERA_DATA0 = 45;
	gotoxy(20, 26);
	*(u8*)VERA_DATA0 = 33;
	*(u8*)VERA_DATA0 = 26;
	*(u8*)VERA_DATA0 = 46;
	*(u8*)VERA_DATA0 = 47;
	*(u8*)VERA_DATA0 = 48;
	*(u8*)VERA_DATA0 = 49;
	*(u8*)VERA_DATA0 = 50;
	*(u8*)VERA_DATA0 = 51;
	*(u8*)VERA_DATA0 = 52;
	*(u8*)VERA_DATA0 = 53;
	*(u8*)VERA_DATA0 = 54;
	*(u8*)VERA_DATA0 = 55;
	*(u8*)VERA_DATA0 = 56;
	*(u8*)VERA_DATA0 = 57;
	gotoxy(20, 27);
	*(u8*)VERA_DATA0 = 58;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 60;
	*(u8*)VERA_DATA0 = 61;
	*(u8*)VERA_DATA0 = 62;
	*(u8*)VERA_DATA0 = 63;
	*(u8*)VERA_DATA0 = 64;
	*(u8*)VERA_DATA0 = 65;
	*(u8*)VERA_DATA0 = 66;
	*(u8*)VERA_DATA0 = 67;
	*(u8*)VERA_DATA0 = 68;
	*(u8*)VERA_DATA0 = 69;
	*(u8*)VERA_DATA0 = 70;
	*(u8*)VERA_DATA0 = 71;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 72;
}

void pressedbutton() {
	*(u8*)VERA_ADDR_H = 0x21;
	gotoxy(20, 24);
	*(u8*)VERA_DATA0 = 28;
	gotoxy(20, 25);
	*(u8*)VERA_DATA0 = 33;
	gotoxy(20, 26);
	*(u8*)VERA_DATA0 = 33;
	gotoxy(20, 27);
	*(u8*)VERA_DATA0 = 58;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 59;
	*(u8*)VERA_DATA0 = 72;
}

void considerguess() {
	u8 cnt;


}

int main() {
	u16 x, y;
	u8 cnt;

	*(u8*)VERA_DC_HSCALE = 0x40;
	*(u8*)VERA_DC_VSCALE = 0x40;

	vload("bgimg.bin", 0x0000, 0);
	vload("tiles.bin", 0x9800, 0);

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

	// Fill screen with see-through tiles = clear screen
	*(u8*)VERA_ADDR_H = 0x11;
	*(u8*)VERA_ADDR_M = 0xB0;
	*(u8*)VERA_ADDR_L = 0x00;
	for (y=0;y<30;y++) {
		for(x=0;x<40;x++) {
			*(u8*)VERA_DATA0 = 26;
			*(u8*)VERA_DATA0 = 0;
		}
		*(u8*)VERA_ADDR_L = 0x00;
		*(u8*)VERA_ADDR_M = *(u8*)VERA_ADDR_M+1;
	}

	combination[0]=rndcircle();
	combination[1]=rndcircle();
	combination[2]=rndcircle();
	combination[3]=rndcircle();

	preplines();
	//Configure sprites
	configuresprites();
	// Enable sprites
	*(u8*)VERA_DC_VIDEO = *(u8*)VERA_DC_VIDEO|0x40; 

	enamouse();

	grayoutbutton();

	while(1){
		x = getmouse(TMP_PTR0);
		mousex = *(u16*)TMP_PTR0;
		mousey = *(u16*)TMP_PTR1;
		if (x==1) {
			if (sprite>0) {
				set_sprite_coord(sprite+0, mousex-8, mousey-8);
				set_sprite_coord(sprite+1, mousex, mousey-8);
				set_sprite_coord(sprite+2, mousex-8, mousey);
				set_sprite_coord(sprite+3, mousex, mousey);
			} else {
				if ((mousey>=0x48) && (mousey<0x58)) {
					if ((mousex>=0xA0) && (mousex<=0xAF))
						sprite = RED_CIRCLE;
					if ((mousex>=0xB8) && (mousex<=0xC7))
						sprite = YELLOW_CIRCLE;
					if ((mousex>=0xD0) && (mousex<=0xDF))
						sprite = BLUE_CIRCLE;
					if ((mousex>=0xE8) && (mousex<=0xF7))
						sprite = GREEN_CIRCLE;
					if ((mousex>=0x100) && (mousex<=0x10F))
						sprite = PURPLE_CIRCLE;
					if ((mousex>=0x118) && (mousex<=0x127))
						sprite = CYAN_CIRCLE;
				} else if ((mousey>=192) && (mousey<=224) &&
					   (mousex>=160) && (mousex<=288) && (btnenabled==1)) {
					pressedbutton();
					btnpressed=1;
				}

			}
		} else {
			// If we are dragging a sprite and releasing the left mousebutton
			if (sprite>0) {
				for (cnt=0;cnt<10;cnt++) {
					// If the line is not already guessed
					if (lineinfo[cnt].isDone==0) {
						// If the mousecursor is within Y coords of line
						if ((mousey>=lineinfo[cnt].miny) &&
						    (mousey<=lineinfo[cnt].maxy)) {
							*(u8*)VERA_ADDR_H = 0x21;
							if ((mousex>=FIELDMINX0) &&
							    (mousex<=FIELDMAXX0)) {
								lineinfo[cnt].fieldcolor[0]=sprite;
								gotoxy(FIELDTX0, lineinfo[cnt].tiley);
								showcircle(sprite);
							} else
							if ((mousex>=FIELDMINX1) &&
							    (mousex<=FIELDMAXX1)) {
								lineinfo[cnt].fieldcolor[1]=sprite;
								gotoxy(FIELDTX1, lineinfo[cnt].tiley);
								showcircle(sprite);
							} else
							if ((mousex>=FIELDMINX2) &&
							    (mousex<=FIELDMAXX2)) {
								lineinfo[cnt].fieldcolor[2]=sprite;
								gotoxy(FIELDTX2, lineinfo[cnt].tiley);
								showcircle(sprite);
							} else
							if ((mousex>=FIELDMINX3) &&
							    (mousex<=FIELDMAXX3)) {
								lineinfo[cnt].fieldcolor[3]=sprite;
								gotoxy(FIELDTX3, lineinfo[cnt].tiley);
								showcircle(sprite);
							}
							if ((lineinfo[cnt].fieldcolor[0]!=0) &&
							    (lineinfo[cnt].fieldcolor[1]!=0) &&
							    (lineinfo[cnt].fieldcolor[2]!=0) &&
							    (lineinfo[cnt].fieldcolor[3]!=0))
							    	enablebutton();
						}
					}
				}
				set_sprite_coord(sprite+0, 400, 0);
				set_sprite_coord(sprite+1, 400, 0);
				set_sprite_coord(sprite+2, 400, 0);
				set_sprite_coord(sprite+3, 400, 0);
				sprite=0;
			}
			// If we have depressed the button, we are now letting go
			// and a guess should be considered <-- (TBD)
			if (btnpressed==1) {
				btnpressed=0;
				grayoutbutton();
				considerguess();
			}
		}
	} 

	return (0);
}
