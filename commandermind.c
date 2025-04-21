#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "commandermind.h"
#include "splashpalette.h"

u16 mousex=0;
u16 mousey=0;
u8 sprite=0;
u8 btnpressed=0;
u8 btnenabled=0;
struct _lineinfo lineinfo[10];
u8 combination[4];
u8 musicplaying=1;
u8 btn2pressed=0;

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

/******************************************************************************
 Ensure the 10x4 positions on the playing field are reset and ready for the
 user to place pieces and guess at the coe
******************************************************************************/
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

/******************************************************************************
 Set VERA Address according to the coordinates provided to the function
 ******************************************************************************/
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
	*(u8*)VERA_DATA0=(sa->height<<6)|(sa->width<<4)|(sa->paletteoffset&0x0F);

	// Restore VERA address
	*(u8*)VERA_ADDR_H=oldaddrhi;
	*(u16*)VERA_ADDR=oldaddr;
}

/******************************************************************************
 Get sprite attributes from VRAM
 The function ensure to set the correct VERA address and then reads the
 sprite information into the spriteattributes struct.
******************************************************************************/
void getspriteattribs(u8 spriteID, struct _spriteattributes *sa) {
	u16 oldaddr;
	u8 oldaddrhi, tmp;

	// Save VERA addresses
	oldaddr=*(u16*)VERA_ADDR;
	oldaddrhi=*(u8*)VERA_ADDR_H;

	// Set VERA address to start of spriteID
	*(u16*)VERA_ADDR=0xFC00+(spriteID*8);
	*(u8*)VERA_ADDR_H=0x11;

	sa->address = *(u8*)VERA_DATA0;
	tmp = *(u8*)VERA_DATA0;
	sa->address += ((tmp&0x0F)<<8);
	sa->address = sa->address<<5;
	sa->mode = tmp>>7;

	sa->x = *(u8*)VERA_DATA0;
	tmp = *(u8*)VERA_DATA0&0x03;
	sa->x += (tmp<<8);

	sa->y = *(u8*)VERA_DATA0;
	tmp = *(u8*)VERA_DATA0&0x03;
	sa->y += (tmp<<8);

	tmp = *(u8*)VERA_DATA0;
	sa->hflip = tmp&0x01;
	sa->vflip = (tmp&0x02)>>1;
	sa->zdepth = (tmp&0x0C)>>2;
	sa->collisionmask = (tmp&0xF0)>>4;
	
	tmp = *(u8*)VERA_DATA0;
	sa->paletteoffset = (tmp&0x0F);
	sa->width = (tmp&0x30)>>4;
	sa->height = (tmp&0xC0)>>6;

	// Restore VERA address
	*(u8*)VERA_ADDR_H=oldaddrhi;
	*(u16*)VERA_ADDR=oldaddr;
}

/******************************************************************************
 Move a sprite to the specified coordinates
******************************************************************************/
void set_sprite_coord(u8 spriteid, u16 x, u16 y){
	// Set VERA address to offset 2 of Sprite ID = X coordinate
	*(u16*)VERA_ADDR=(0xFC00+(spriteid*8))+2;
	*(u8*)VERA_ADDR_H=0x11;
	*(u8*)VERA_DATA0=x;
	*(u8*)VERA_DATA0=x>>8;
	*(u8*)VERA_DATA0=y;
	*(u8*)VERA_DATA0=y>>8;
}

/******************************************************************************
 Configure the sprites used in the game
******************************************************************************/
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
	sa.paletteoffset 	= 0;

	// The sprites are the same as some of the tiles used
	// so the only thin changed is the address of the sprite image data
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

/******************************************************************************
 Place a circle on layer 1. A circle consists of 4 tiles
******************************************************************************/
void showcircle(u8 circle) {
	switch (circle) {
		case RED_CIRCLE:	circle=2;  break;
		case YELLOW_CIRCLE:	circle=6;  break;
		case BLUE_CIRCLE:	circle=10; break;
		case GREEN_CIRCLE:	circle=14; break;
		case PURPLE_CIRCLE:	circle=18; break;
		case CYAN_CIRCLE:	circle=22; break;
	}
	// Upper left part of circle
	*(u8*)VERA_DATA0 = circle;
	// Upper right part of circle
	*(u8*)VERA_DATA0 = ++circle;
	// Change VERA address to go down 1 line and left
	*(u8*)VERA_ADDR_M = *(u8*)VERA_ADDR_M + 1;
	*(u8*)VERA_ADDR_L = *(u8*)VERA_ADDR_L - 4;
	// Lower left part of the circle
	*(u8*)VERA_DATA0 = ++circle;
	// Lower right part of the circle
	*(u8*)VERA_DATA0 = ++circle;
}

/******************************************************************************
 Show tiles on Layer 1 that makes the buttn look enabled (blank tiles)
******************************************************************************/
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

/******************************************************************************
 Show tiles on Layer 1 that makes the button look disabled
******************************************************************************/
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

/******************************************************************************
 Show tiles on Layer 1 that makes the button look pressed down
******************************************************************************/
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

/******************************************************************************
 Fill Layer 1 with see-through tiles = clear screen
******************************************************************************/
void clrscr() {
	u8 x, y;

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
}

/******************************************************************************
 Show the secret code that the computer had made up
******************************************************************************/
void showresult() {
	*(u8*)VERA_ADDR_H = 0x21;
	gotoxy(23, 18);
	showcircle(combination[0]);
	gotoxy(26, 18);
	showcircle(combination[1]);
	gotoxy(29, 18);
	showcircle(combination[2]);
	gotoxy(32, 18);
	showcircle(combination[3]);
}

/******************************************************************************
 Create a rectangle of tiles on Layer 1
******************************************************************************/
void box(u8 x, u8 y, u8 w, u8 h, u8 ch) {
	u8 cntx, cnty;

	*(u8*)VERA_ADDR_H = 0x21;
	for (cnty=y;cnty<y+h;cnty++) {
		gotoxy(x, cnty);
		for (cntx=x;cntx<x+w;cntx++) {
			*(u8*)VERA_DATA0 = ch;
		}
	}
}

/******************************************************************************
 Show text on the screen, asking the user to play again and presenting the
 Yes and No options
******************************************************************************/
void playagain() {
	*(u8*)VERA_ADDR_H = 0x21;
	gotoxy(23, 13);
	*(u8*)VERA_DATA0 = 82; // P
	*(u8*)VERA_DATA0 = 79; // L
	*(u8*)VERA_DATA0 = 75; // A
	*(u8*)VERA_DATA0 = 87; // Y
	*(u8*)VERA_DATA0 = 26; // [spc]
	*(u8*)VERA_DATA0 = 75; // A
	*(u8*)VERA_DATA0 = 77; // G
	*(u8*)VERA_DATA0 = 75; // A
	*(u8*)VERA_DATA0 = 78; // I
	*(u8*)VERA_DATA0 = 80; // N
	*(u8*)VERA_DATA0 = 74; // ?
	gotoxy(25, 15);
	*(u8*)VERA_DATA0 = 87; // Y
	*(u8*)VERA_DATA0 = 76; // E
	*(u8*)VERA_DATA0 = 83; // S
	*(u8*)VERA_DATA0 = 26; // [spc]
	*(u8*)VERA_DATA0 = 26; // [spc]
	*(u8*)VERA_DATA0 = 80; // N
	*(u8*)VERA_DATA0 = 81; // O
}

/******************************************************************************
 Show text indicating the the user won the game
******************************************************************************/
void youwon() {
	gotoxy(24, 12);
	*(u8*)VERA_ADDR_H = 0x21;
	*(u8*)VERA_DATA0 = 87; // Y
	*(u8*)VERA_DATA0 = 81; // O
	*(u8*)VERA_DATA0 = 85; // U
	*(u8*)VERA_DATA0 = 26; // [spc]
	*(u8*)VERA_DATA0 = 86; // W
	*(u8*)VERA_DATA0 = 81; // O
	*(u8*)VERA_DATA0 = 80; // N
	*(u8*)VERA_DATA0 = 73; // !
	*(u8*)VERA_DATA0 = 73; // !
	playagain();
}

/******************************************************************************
 Show text indicating the the user lost the game
******************************************************************************/
void youlost() {
	gotoxy(24, 12);
	*(u8*)VERA_ADDR_H = 0x21;
	*(u8*)VERA_DATA0 = 87; // Y
	*(u8*)VERA_DATA0 = 81; // O
	*(u8*)VERA_DATA0 = 85; // U
	*(u8*)VERA_DATA0 = 26; // [spc]
	*(u8*)VERA_DATA0 = 79; // L
	*(u8*)VERA_DATA0 = 81; // O
	*(u8*)VERA_DATA0 = 83; // S
	*(u8*)VERA_DATA0 = 84; // T
	*(u8*)VERA_DATA0 = 73; // !
	playagain();
}

/******************************************************************************
 Initialize the game, variables and onscreen assets
******************************************************************************/
void initgame() {
	clrscr();
	preplines();
	grayoutbutton();
	combination[0]=rndcircle();
	combination[1]=rndcircle();
	combination[2]=rndcircle();
	combination[3]=rndcircle();
	box(23, 18, 11, 2, 27);
}

/******************************************************************************
 Wait for the user to press the Yes or the No text
******************************************************************************/
void getynclick() {
	u8 exitloop=0;
	u8 btn;

	while (exitloop==0) {
		// Get mouse information into normal variables
		btn = getmouse(TMP_PTR0);
		mousex = *(u16*)TMP_PTR0;
		mousey = *(u16*)TMP_PTR1;
		// If left mousebutton pressed
		if (btn==1) {
			// If the mouse is within the horizontal coordinates of the text
			if ((mousey<=127) && (mousey>=120)) {
				// If mouse is within the vertical coordinates of Yes text
				if ((mousex>=200) && (mousex<224)) { //YES button pressed
					initgame();
					exitloop=1;
				} else
				// If mouse is within the vertical coordinates of No text
				if ((mousex>=240) && (mousex<256)) { //NO button pressed
					returntobasic();
				}
			}
		} else if (btn==2) {
			if (btn2pressed==0) {
				btn2pressed=1;
				if (musicplaying==0) {
					zsmplay(0);
					musicplaying=1;
				} else {
					zsmstop(0);
					musicplaying=0;
				}
			}
		} else btn2pressed=0;
	}
}

/******************************************************************************
 Show the result and tell the user that they have won the game
******************************************************************************/
void showwinner() {
	box(23, 18, 11, 2, 26);
	showresult();
	youwon();
	getynclick();
}

/******************************************************************************
 Show the result and tell the user that they have lost the game
******************************************************************************/
void showloser() {
	box(23, 18, 11, 2, 26);
	showresult();
	youlost();
	getynclick();
}

/******************************************************************************
 Check through all lines that have not already been checked and see if they
 have correct color and or placements and show the result to the user
******************************************************************************/
void considerguess() {
	u8 cnt, f0used, f1used, f2used, f3used;
	u8 resx, resy;
	u8 fc[4];

	// Count though all 10 lines
	for (cnt=0;cnt<10;cnt++) {
		resx=13;
		resy=0;
		f0used=0; f1used=0; f2used=0; f3used=0;
		// If the current line has not already been checked
		if (lineinfo[cnt].isDone!=1) {
			fc[0]=lineinfo[cnt].fieldcolor[0];
			fc[1]=lineinfo[cnt].fieldcolor[1];
			fc[2]=lineinfo[cnt].fieldcolor[2];
			fc[3]=lineinfo[cnt].fieldcolor[3];

			// If all fields in the line has pieces placed
			if ((fc[0]!=0) && (fc[1]!=0) && (fc[2]!=0) && (fc[3]!=0)) {
				// Indicate that the line is done
				lineinfo[cnt].isDone=1;

				if (fc[0]==combination[0]) {
					gotoxy(resx, lineinfo[cnt].tiley+resy);
					*(u8*)VERA_DATA0 = 1;
					f0used=2;
					if (resx==13) ++resx;
					else {resx=13;++resy;}
				}
				if (fc[1]==combination[1]) {
					gotoxy(resx, lineinfo[cnt].tiley+resy);
					*(u8*)VERA_DATA0 = 1;
					f1used=2;
					if (resx==13) ++resx;
					else {resx=13;++resy;}
				}
				if (fc[2]==combination[2]) {
					gotoxy(resx, lineinfo[cnt].tiley+resy);
					*(u8*)VERA_DATA0 = 1;
					f2used=2;
					if (resx==13) ++resx;
					else {resx=13;++resy;}
				}
				if (fc[3]==combination[3]) {
					gotoxy(resx, lineinfo[cnt].tiley+resy);
					*(u8*)VERA_DATA0 = 1;
					f3used=2;
					if (resx==13) ++resx;
					else {resx=13;++resy;}
				}
				// If we have found all 4 colors correctly placed, end the loop
				if ((f0used==2) && (f1used==2) && (f2used==2) && (f3used==2)) break;

				// Figure out if there are correct colors that are not placed at
				// the correct spot
				if (f0used!=2) {
					if ((fc[0]==combination[1]) && (f1used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f1used=1;
					} else
					if ((fc[0]==combination[2]) && (f2used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f2used=1;
					} else
					if ((fc[0]==combination[3]) && (f1used==3)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f3used=1;
					}
				}
				if (f1used!=2) {
					if ((fc[1]==combination[0]) && (f0used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f0used=1;
					} else
					if ((fc[1]==combination[2]) && (f2used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f2used=1;
					} else
					if ((fc[1]==combination[3]) && (f3used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f3used=1;
					}
				}
				if (f2used!=2) {
					if ((fc[2]==combination[0]) && (f0used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f0used=1;
					} else
					if ((fc[2]==combination[1]) && (f1used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f1used=1;
					} else
					if ((fc[2]==combination[3]) && (f3used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
						if (resx==13) ++resx;
						else {resx=13;++resy;}
						f3used=1;
					}
				}
				if (f3used!=2) {
					if ((fc[3]==combination[0]) && (f0used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
					} else
					if ((fc[3]==combination[1]) && (f1used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
					} else
					if ((fc[3]==combination[2]) && (f2used==0)) {
						gotoxy(resx, lineinfo[cnt].tiley+resy);
						*(u8*)VERA_DATA0 = 0;
					}
				}
			}

		}
	}
	// If there are 4 correct colors at the correct places, the game is won
	if ((f0used==2) && (f1used==2) && (f2used==2) && (f3used==2)) showwinner();
	resx=0; // Reuse variable to see if game is lost
	// Search through all 10 lines to see if they are all done
	for (cnt=0;cnt<10;cnt++) if (lineinfo[cnt].isDone==0) resx=1;
	// If all lines are done, the game is lost
	if (resx==0) showloser();
}

/******************************************************************************
 Entry point to the program
******************************************************************************/
int main() {
	u8 cnt, btn;
	struct _spriteattributes sa;

	printf("loading assets...");

	*(u8*)VERA_DC_HSCALE = 0x40;
	*(u8*)VERA_DC_VSCALE = 0x40;

	// Enable mouse (copying sprite data to VERA address 0x13000)
	enamouse();
	// Set the palette entry for the splash image
	setsplashpal();

	// Copy mouse pointer from default VERA position 0x13000 to 0x0A300
	*(u8*)VERA_ADDR_H = 0x11;
	*(u16*)VERA_ADDR = 0x3000;
	*(u8*)VERA_CTRL = 1;
	*(u8*)VERA_ADDR_H = 0x10;
	*(u16*)VERA_ADDR = 0xA300;

	for (cnt=0;cnt<255;cnt++)
		*(u8*)VERA_DATA1 = *(u8*)VERA_DATA0;
	*(u8*)VERA_DATA1 = *(u8*)VERA_DATA0;

	*(u8*)VERA_CTRL = 0;

	// Change mouse sprite to point to data at address 0x0A300
	getspriteattribs(0, &sa);
	sa.address = 0xA300;
	sa.address_hi = 0x00;
	configSprite(0, &sa);

	// Load assets to memory
	vload("bgimg.bin", 0x0000, 0);
	vload("splash.bin", 0x0000, 1);
	vload("tiles.bin", 0x9800, 0);
	bload("zsmkit-a000.bin", 0xA000, 1);
	bload("music.zsm", 0xA000, 2);
	initzsm();

	*(u8*)VERA_L0_CONFIG = 0x06; //bitmap mode, 4bpp color
	*(u8*)VERA_L0_TILEBASE = SPLASH_BASE;
	*(u8*)VERA_L0_HSCROLL_H = SPLASH_PAL_OFFSET;

	// map height = 0 = 32 tiles
	// map width = 2 = 128 tiles
	// color depth = 2 = 4 bpp
	*(u8*)VERA_L1_CONFIG = 0b00100010;

	// tile width = 0 = 8 pixels
	// tile height = 0 = 8 pixels
	// tile_base = address of tile data = 0x9600
	*(u8*)VERA_L1_TILEBASE = 0x4C;

	//Configure sprites
	configuresprites();

	// Clear screen to ensure that splash image is shown correctly
	clrscr();
	
	// Enable Layer 0 & sprites
	*(u8*)VERA_DC_VIDEO = *(u8*)VERA_DC_VIDEO|0x50;

	// Start playing the music
	zsmplay(0);

	// Wait for the user to start the game and let them toggle the music
	while ((btn=getmouse(TMP_PTR0))!=1) {
		if (btn==2) {
			if (btn2pressed==0) {
				btn2pressed=1;
				if (musicplaying==0) {
					zsmplay(0);
					musicplaying=1;
				} else {
					zsmstop(0);
					musicplaying=0;
				}
			}
		} else btn2pressed=0;
	}

	// Reset L0 settings to point to the background image instead of splash image
	*(u8*)VERA_L0_HSCROLL_H = 0; // Reset palette back to default
	*(u8*)VERA_L0_TILEBASE = 0;

	//Initialize the game
	initgame();

	while(1){
		// Get mouse information and store it in "normal" variables
		btn = getmouse(TMP_PTR0);
		mousex = *(u16*)TMP_PTR0;
		mousey = *(u16*)TMP_PTR1;

		// If Both right and left mousebutton is pressed
		if (btn==3) {
			// Ask the user if they want to play again or end the game
			playagain();
			getynclick();
		} else
		// If right mousebutton is pressed
		if (btn==2) {
			// toggle the playback of the music
			if (btn2pressed==0) {
				btn2pressed=1;
				if (musicplaying==0) {
					zsmplay(0);
					musicplaying=1;
				} else {
					zsmstop(0);
					musicplaying=0;
				}
			}
		} else
		// If left mousebutton is pressed
		if (btn==1) {
			// If a sprite is chosen, update it's coordinates according to the
			// mousepointer coordinates
			if (sprite>0) {
				set_sprite_coord(sprite+0, mousex-8, mousey-8);
				set_sprite_coord(sprite+1, mousex, mousey-8);
				set_sprite_coord(sprite+2, mousex-8, mousey);
				set_sprite_coord(sprite+3, mousex, mousey);
			// If a sprite is not chosen
			} else {
				// Check if pointer is within areas of colored pieces
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
				// Check if pointer is inside the "guess" button
				} else if ((mousey>=192) && (mousey<=224) &&
					   (mousex>=160) && (mousex<=288) && (btnenabled==1)) {
					pressedbutton();
					btnpressed=1;
				}

			}
		// If neither right, left or both buttons are pressed on the mouse
		} else {
			btn2pressed=0;
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
				// Hide sprite
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

	returntobasic();
	return (0);
}
