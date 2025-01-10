#include <Arduino.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include "markerLEDs.h"
#include "colors.h"
#include "definitions.h"
#include "functions.h"
#include "smileytongue24.h"
//--------------------------------------

extern FastLED_NeoMatrix* matrix;
extern volatile boolean LEDsTurnedOff;
extern volatile unsigned int nextChangeMillis;
extern volatile byte nextSongPart;
extern volatile boolean nextChangeMillisAlreadyCalculated;
//extern const boolean LEDGITBOARD;
extern boolean LEDGITBOARD;	// defined in definitions.h

static const uint8_t PROGMEM mono_bmp[][8] = {
{   // 0: checkered 1
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
		},
{   // 1: checkered 2
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
	B01010101,
	B10101010,
		},
{   // 2: smiley
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10100101,
	B10011001,
	B01000010,
	B00111100 },
{   // 3: neutral
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10111101,
	B10000001,
	B01000010,
	B00111100 },
{   // 4; frowny
	B00111100,
	B01000010,
	B10100101,
	B10000001,
	B10011001,
	B10100101,
	B01000010,
	B00111100 },
};

#if defined(LEDMATRIX)

	//============================================================================= war frueher mal in LEDMatrix.h
	//========== Andres remapping function für das LED-GIT-BOARD =================
	//============================================================================
	const int MISSING_LED = 300;
	uint16_t myRemapFn(uint16_t x, uint16_t y) {

		switch (y) {
		case 0:
			switch (x) {
			case 4: return 2;
			case 5: return 1;
			case 6: return 0;
			default: return MISSING_LED;
			}
			break;
		case 1:
			switch (x) {
			case 2: return 3;
			case 3: return 4;
			case 4: return 5;
			case 5: return 6;
			case 6: return 7;
			case 7: return 8;
			default: return MISSING_LED;
			}
			break;
		case 2:
			switch (x) {
			case 1: return 16;
			case 2: return 15;
			case 3: return 14;
			case 4: return 13;
			case 5: return 12;
			case 6: return 11;
			case 7: return 10;
			case 8: return 9;
			case 16: return 26;
			case 17: return 27;
			case 18: return 28;
			case 19: return 29;
			default: return MISSING_LED;
			}
			break;
		case 3:
			switch (x) {
			case 1: return 17;
			case 2: return 18;
			case 3: return 19;
			case 4: return 20;
			case 5: return 21;
			case 6: return 22;
			case 7: return 23;
			case 8: return 24;
			case 9: return 25;
			case 15: return 36;
			case 16: return 35;
			case 17: return 34;
			case 18: return 33;
			case 19: return 32;
			case 20: return 31;
			case 21: return 30;
			default: return MISSING_LED;
			}
			break;
		case 4:
			switch (x) {
			case 0: return 56;
			case 1: return 55;
			case 2: return 54;
			case 3: return 53;
			case 4: return 52;
			case 5: return 51;
			case 6: return 50;
			case 7: return 49;
			case 8: return 48;
			case 9: return 47;
			case 10: return 46;
			case 11: return 45;
			case 12: return 44;
			case 13: return 43;
			case 14: return 42;
			case 15: return 41;
			case 16: return 40;
			case 17: return 39;
			case 18: return 38;
			case 19: return 37;
			default: return MISSING_LED;
			}
			break;
		case 5:
			switch (x) {
			case 0: return 57;
			case 1: return 58;
			case 2: return 59;
			case 3: return 60;
			case 4: return 61;
			case 5: return 62;
			case 6: return 63;
			case 7: return 64;
			case 8: return 65;
			case 9: return 66;
			case 10: return 67;
			case 11: return 68;
			case 12: return 69;
			case 13: return 70;
			case 14: return 71;
			case 15: return 72;
			case 16: return 73;
			case 17: return 74;
			case 18: return 75;
			case 19: return 76;
			default: return MISSING_LED;
			}
			break;
		case 6:
			switch (x) {
			case 0: return 91;
			case 1: return 90;
			case 2: return 89;
			case 3: return 88;
			case 4: return 87;
			case 5: return 86;
			case 6: return 85;
			case 7: return 84;
			case 8: return 83;
			case 9: return 82;
			case 10: return 81;
			case 11: return 80;
			case 12: return 79;
			case 13: return 78;
			case 14: return 77;
			default: return MISSING_LED;
			}
			break;
		case 7:
			switch (x) {
			case 0: return 92;
			case 1: return 93;
			case 2: return 94;
			case 3: return 95;
			case 4: return 96;
			case 10: return 127;
			case 11: return 128;
			case 12: return 129;
			case 13: return 130;
			case 14: return 131;
			default: return MISSING_LED;
			}
			break;
		case 8:
			switch (x) {
			case 0: return 101;
			case 1: return 100;
			case 2: return 99;
			case 3: return 98;
			case 4: return 97;
			case 10: return 136;
			case 11: return 135;
			case 12: return 134;
			case 13: return 133;
			case 14: return 132;
			default: return MISSING_LED;
			}
			break;
		case 9:
			switch (x) {
			case 0: return 102;
			case 1: return 103;
			case 2: return 104;
			case 3: return 105;
			case 4: return 106;
			case 10: return 137;
			case 11: return 138;
			case 12: return 139;
			case 13: return 140;
			case 14: return 141;
			default: return MISSING_LED;
			}
			break;
		case 10:
			switch (x) {
			case 0: return 111;
			case 1: return 110;
			case 2: return 109;
			case 3: return 108;
			case 4: return 107;
			case 10: return 146;
			case 11: return 145;
			case 12: return 144;
			case 13: return 143;
			case 14: return 142;
			default: return MISSING_LED;
			}
			break;
		case 11:
			switch (x) {
			case 0: return 112;
			case 1: return 113;
			case 2: return 114;
			case 3: return 115;
			case 4: return 116;
			case 10: return 147;
			case 11: return 148;
			case 12: return 149;
			case 13: return 150;
			case 14: return 151;
			default: return MISSING_LED;
			}
			break;
		case 12:
			switch (x) {
			case 0: return 121;
			case 1: return 120;
			case 2: return 119;
			case 3: return 118;
			case 4: return 117;
			case 10: return 156;
			case 11: return 155;
			case 12: return 154;
			case 13: return 153;
			case 14: return 152;
			default: return MISSING_LED;
			}
			break;
		case 13:
			switch (x) {
			case 0: return 122;
			case 1: return 123;
			case 2: return 124;
			case 3: return 125;
			case 4: return 126;
			case 10: return 157;
			case 11: return 158;
			case 12: return 159;
			case 13: return 160;
			case 14: return 161;
			default: return MISSING_LED;
			}
			break;
		case 14:
			switch (x) {
			case 1: return 162;
			case 2: return 163;
			case 3: return 164;
			case 4: return 165;
			case 5: return 166;
			case 6: return 167;
			case 7: return 168;
			case 8: return 169;
			case 9: return 170;
			case 10: return 171;
			case 11: return 172;
			case 12: return 173;
			case 13: return 174;
			case 14: return 175;
			default: return MISSING_LED;
			}
			break;
		case 15:
			switch (x) {
			case 2: return 192;
			case 3: return 191;
			case 4: return 190;
			case 5: return 189;
			case 6: return 188;
			case 7: return 187;
			case 8: return 186;
			case 9: return 185;
			case 10: return 184;
			case 11: return 183;
			case 12: return 182;
			case 13: return 181;
			case 14: return 180;
			case 15: return 179;
			case 16: return 178;
			case 17: return 177;
			case 18: return 176;
			default: return MISSING_LED;
			}
			break;
		case 16:
			switch (x) {
			case 1: return 193;
			case 2: return 194;
			case 3: return 195;
			case 4: return 196;
			case 5: return 197;
			case 6: return 198;
			case 7: return 199;
			case 8: return 200;
			case 9: return 201;
			case 10: return 202;
			case 11: return 203;
			case 12: return 204;
			case 13: return 205;
			case 14: return 206;
			case 15: return 207;
			case 16: return 208;
			case 17: return 209;
			case 18: return 210;
			default: return MISSING_LED;
			}
			break;
		case 17:
			switch (x) {
			case 1: return 229;
			case 2: return 228;
			case 3: return 227;
			case 4: return 226;
			case 5: return 225;
			case 6: return 224;
			case 7: return 223;
			case 8: return 222;
			case 9: return 221;
			case 10: return 220;
			case 11: return 219;
			case 12: return 218;
			case 13: return 217;
			case 14: return 216;
			case 15: return 215;
			case 16: return 214;
			case 17: return 213;
			case 18: return 212;
			case 19: return 211;
			default: return MISSING_LED;
			}
			break;
		case 18:
			switch (x) {
			case 1: return 230;
			case 2: return 231;
			case 3: return 232;
			case 4: return 233;
			case 5: return 234;
			case 6: return 235;
			case 7: return 236;
			case 8: return 237;
			case 9: return 238;
			case 10: return 239;
			case 11: return 240;
			case 12: return 241;
			case 13: return 242;
			case 14: return 243;
			case 15: return 244;
			case 16: return 245;
			case 17: return 246;
			case 18: return 247;
			case 19: return 248;
			case 20: return 249;
			default: return MISSING_LED;
			}
			break;
		case 19:
			switch (x) {
			case 1: return 262;
			case 2: return 261;
			case 3: return 260;
			case 4: return 259;
			case 5: return 258;
			case 6: return 257;
			case 7: return 256;
			case 8: return 255;
			case 9: return 254;
			case 16: return 253;
			case 17: return 252;
			case 18: return 251;
			case 19: return 250;
			default: return MISSING_LED;
			}
			break;
		case 20:
			switch (x) {
			case 2: return 263;
			case 3: return 264;
			case 4: return 265;
			case 5: return 266;
			case 6: return 267;
			case 7: return 268;
			case 8: return 269;
			default: return MISSING_LED;
			}
			break;
		case 21:
			switch (x) {
			case 3: return 274;
			case 4: return 273;
			case 5: return 272;
			case 6: return 271;
			case 7: return 270;
			default: return MISSING_LED;
			}
			break;
		case 22:
			switch (x) {
			case 4: return 275;
			case 5: return 276;
			case 6: return 277;
			default: return MISSING_LED;
			}
			break;
		}
		return MISSING_LED;	// not neccessary but to avoid error
	}
//--------------------------------

	// TODO Checken ob dies auf dem ESP32 geht
// These bitmaps were written for a backend that only supported
// 4 bits per color with Blue/Green/Red ordering while neomatrix
// uses native 565 color mapping as RGB.  
// I'm leaving the arrays as is because it's easier to read
// which color is what when separated on a 4bit boundary
// The demo code will modify the arrays at runtime to be compatible
// with the neomatrix color ordering and bit depth.

	static const uint16_t PROGMEM RGB_bmp[][64] = {
	// 00: blue, blue/red, red, red/green, green, green/blue, blue, white
	{	
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x101, 0x202, 0x303, 0x404, 0x606, 0x808, 0xA0A, 0xF0F,
  0x001, 0x002, 0x003, 0x004, 0x006, 0x008, 0x00A, 0x00F,
  0x011, 0x022, 0x033, 0x044, 0x066, 0x088, 0x0AA, 0x0FF,
  0x010, 0x020, 0x030, 0x040, 0x060, 0x080, 0x0A0, 0x0F0,
  0x110, 0x220, 0x330, 0x440, 0x660, 0x880, 0xAA0, 0xFF0,
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x111, 0x222, 0x333, 0x444, 0x666, 0x888, 0xAAA, 0xFFF, },

  // 01: grey to white
  {	0x111, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x222, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x333, 0x333, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x555, 0x555, 0x555, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x777, 0x777, 0x777, 0x777, 0x777, 0x999, 0xAAA, 0xFFF,
0x999, 0x999, 0x999, 0x999, 0x999, 0x999, 0xAAA, 0xFFF,
0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xFFF,
0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, },

// 02: low red to high red
{	0x001, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x002, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x003, 0x003, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x005, 0x005, 0x005, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x007, 0x007, 0x007, 0x007, 0x007, 0x009, 0x00A, 0x00F,
0x009, 0x009, 0x009, 0x009, 0x009, 0x009, 0x00A, 0x00F,
0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00F,
0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, },

// 03: low green to high green
{	0x010, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x020, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x030, 0x030, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x050, 0x050, 0x050, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x070, 0x070, 0x070, 0x070, 0x070, 0x090, 0x0A0, 0x0F0,
0x090, 0x090, 0x090, 0x090, 0x090, 0x090, 0x0A0, 0x0F0,
0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0F0,
0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, },

// 04: low blue to high blue
{	0x100, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x200, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x300, 0x300, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x500, 0x500, 0x500, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x700, 0x700, 0x700, 0x700, 0x700, 0x900, 0xA00, 0xF00,
0x900, 0x900, 0x900, 0x900, 0x900, 0x900, 0xA00, 0xF00,
0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xF00,
0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, },

// 05: 1 black, 2R, 2O, 2G, 1B with 4 blue lines rising right
{	0x000, 0x200, 0x000, 0x400, 0x000, 0x800, 0x000, 0xF00,
  0x000, 0x201, 0x002, 0x403, 0x004, 0x805, 0x006, 0xF07,
0x008, 0x209, 0x00A, 0x40B, 0x00C, 0x80D, 0x00E, 0xF0F,
0x000, 0x211, 0x022, 0x433, 0x044, 0x855, 0x066, 0xF77,
0x088, 0x299, 0x0AA, 0x4BB, 0x0CC, 0x8DD, 0x0EE, 0xFFF,
0x000, 0x210, 0x020, 0x430, 0x040, 0x850, 0x060, 0xF70,
0x080, 0x290, 0x0A0, 0x4B0, 0x0C0, 0x8D0, 0x0E0, 0xFF0,
0x000, 0x200, 0x000, 0x500, 0x000, 0x800, 0x000, 0xF00, },

// 06: 4 lines of increasing red and then green
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x010, 0x010, 0x020, 0x020, 0x030, 0x030,
0x040, 0x040, 0x050, 0x050, 0x060, 0x060, 0x070, 0x070,
0x080, 0x080, 0x090, 0x090, 0x0A0, 0x0A0, 0x0B0, 0x0B0,
0x0C0, 0x0C0, 0x0D0, 0x0D0, 0x0E0, 0x0E0, 0x0F0, 0x0F0, },

// 07: 4 lines of increasing red and then blue
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x100, 0x100, 0x200, 0x200, 0x300, 0x300,
0x400, 0x400, 0x500, 0x500, 0x600, 0x600, 0x700, 0x700,
0x800, 0x800, 0x900, 0x900, 0xA00, 0xA00, 0xB00, 0xB00,
0xC00, 0xC00, 0xD00, 0xD00, 0xE00, 0xE00, 0xF00, 0xF00, },

// 08: criss cross of green and red with diagonal blue.
{	0xF00, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0x000,
0x020, 0xF21, 0x023, 0x025, 0x027, 0x02A, 0x02F, 0x020,
0x040, 0x041, 0xF43, 0x045, 0x047, 0x04A, 0x04F, 0x040,
0x060, 0x061, 0x063, 0xF65, 0x067, 0x06A, 0x06F, 0x060,
0x080, 0x081, 0x083, 0x085, 0xF87, 0x08A, 0x08F, 0x080,
0x0A0, 0x0A1, 0x0A3, 0x0A5, 0x0A7, 0xFAA, 0x0AF, 0x0A0,
0x0F0, 0x0F1, 0x0F3, 0x0F5, 0x0F7, 0x0FA, 0xFFF, 0x0F0,
0x000, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0xF00, },

// 09: 2 lines of green, 2 red, 2 orange, 2 green
{ 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, },

// 10: multicolor smiley face
{ 0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x00F, 0x000, 0xF00, 0x000, 0x000, 0xF00, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x00F,
0x00F, 0x000, 0x0F0, 0x000, 0x000, 0x0F0, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x0F4, 0x0F3, 0x000, 0x000, 0x00F,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, },
};
#else

// TODO Checken ob dies auf dem ESP32 geht
// These bitmaps were written for a backend that only supported
// 4 bits per color with Blue/Green/Red ordering while neomatrix
// uses native 565 color mapping as RGB.  
// I'm leaving the arrays as is because it's easier to read
// which color is what when separated on a 4bit boundary
// The demo code will modify the arrays at runtime to be compatible
// with the neomatrix color ordering and bit depth.

	static const uint16_t PROGMEM RGB_bmp[][64] = {
	// 00: blue, blue/red, red, red/green, green, green/blue, blue, white
	{	
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x101, 0x202, 0x303, 0x404, 0x606, 0x808, 0xA0A, 0xF0F,
  0x001, 0x002, 0x003, 0x004, 0x006, 0x008, 0x00A, 0x00F,
  0x011, 0x022, 0x033, 0x044, 0x066, 0x088, 0x0AA, 0x0FF,
  0x010, 0x020, 0x030, 0x040, 0x060, 0x080, 0x0A0, 0x0F0,
  0x110, 0x220, 0x330, 0x440, 0x660, 0x880, 0xAA0, 0xFF0,
  0x100, 0x200, 0x300, 0x400, 0x600, 0x800, 0xA00, 0xF00,
  0x111, 0x222, 0x333, 0x444, 0x666, 0x888, 0xAAA, 0xFFF, },

  // 01: grey to white
  {	0x111, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x222, 0x222, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x333, 0x333, 0x333, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x555, 0x555, 0x555, 0x555, 0x777, 0x999, 0xAAA, 0xFFF,
0x777, 0x777, 0x777, 0x777, 0x777, 0x999, 0xAAA, 0xFFF,
0x999, 0x999, 0x999, 0x999, 0x999, 0x999, 0xAAA, 0xFFF,
0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xAAA, 0xFFF,
0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, },

// 02: low red to high red
{	0x001, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x002, 0x002, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x003, 0x003, 0x003, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x005, 0x005, 0x005, 0x005, 0x007, 0x009, 0x00A, 0x00F,
0x007, 0x007, 0x007, 0x007, 0x007, 0x009, 0x00A, 0x00F,
0x009, 0x009, 0x009, 0x009, 0x009, 0x009, 0x00A, 0x00F,
0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00A, 0x00F,
0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, 0x00F, },

// 03: low green to high green
{	0x010, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x020, 0x020, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x030, 0x030, 0x030, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x050, 0x050, 0x050, 0x050, 0x070, 0x090, 0x0A0, 0x0F0,
0x070, 0x070, 0x070, 0x070, 0x070, 0x090, 0x0A0, 0x0F0,
0x090, 0x090, 0x090, 0x090, 0x090, 0x090, 0x0A0, 0x0F0,
0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0A0, 0x0F0,
0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, 0x0F0, },

// 04: low blue to high blue
{	0x100, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x200, 0x200, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x300, 0x300, 0x300, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x500, 0x500, 0x500, 0x500, 0x700, 0x900, 0xA00, 0xF00,
0x700, 0x700, 0x700, 0x700, 0x700, 0x900, 0xA00, 0xF00,
0x900, 0x900, 0x900, 0x900, 0x900, 0x900, 0xA00, 0xF00,
0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xA00, 0xF00,
0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, 0xF00, },

// 05: 1 black, 2R, 2O, 2G, 1B with 4 blue lines rising right
{	0x000, 0x200, 0x000, 0x400, 0x000, 0x800, 0x000, 0xF00,
  0x000, 0x201, 0x002, 0x403, 0x004, 0x805, 0x006, 0xF07,
0x008, 0x209, 0x00A, 0x40B, 0x00C, 0x80D, 0x00E, 0xF0F,
0x000, 0x211, 0x022, 0x433, 0x044, 0x855, 0x066, 0xF77,
0x088, 0x299, 0x0AA, 0x4BB, 0x0CC, 0x8DD, 0x0EE, 0xFFF,
0x000, 0x210, 0x020, 0x430, 0x040, 0x850, 0x060, 0xF70,
0x080, 0x290, 0x0A0, 0x4B0, 0x0C0, 0x8D0, 0x0E0, 0xFF0,
0x000, 0x200, 0x000, 0x500, 0x000, 0x800, 0x000, 0xF00, },

// 06: 4 lines of increasing red and then green
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x010, 0x010, 0x020, 0x020, 0x030, 0x030,
0x040, 0x040, 0x050, 0x050, 0x060, 0x060, 0x070, 0x070,
0x080, 0x080, 0x090, 0x090, 0x0A0, 0x0A0, 0x0B0, 0x0B0,
0x0C0, 0x0C0, 0x0D0, 0x0D0, 0x0E0, 0x0E0, 0x0F0, 0x0F0, },

// 07: 4 lines of increasing red and then blue
{ 0x000, 0x000, 0x001, 0x001, 0x002, 0x002, 0x003, 0x003,
0x004, 0x004, 0x005, 0x005, 0x006, 0x006, 0x007, 0x007,
0x008, 0x008, 0x009, 0x009, 0x00A, 0x00A, 0x00B, 0x00B,
0x00C, 0x00C, 0x00D, 0x00D, 0x00E, 0x00E, 0x00F, 0x00F,
0x000, 0x000, 0x100, 0x100, 0x200, 0x200, 0x300, 0x300,
0x400, 0x400, 0x500, 0x500, 0x600, 0x600, 0x700, 0x700,
0x800, 0x800, 0x900, 0x900, 0xA00, 0xA00, 0xB00, 0xB00,
0xC00, 0xC00, 0xD00, 0xD00, 0xE00, 0xE00, 0xF00, 0xF00, },

// 08: criss cross of green and red with diagonal blue.
{	0xF00, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0x000,
0x020, 0xF21, 0x023, 0x025, 0x027, 0x02A, 0x02F, 0x020,
0x040, 0x041, 0xF43, 0x045, 0x047, 0x04A, 0x04F, 0x040,
0x060, 0x061, 0x063, 0xF65, 0x067, 0x06A, 0x06F, 0x060,
0x080, 0x081, 0x083, 0x085, 0xF87, 0x08A, 0x08F, 0x080,
0x0A0, 0x0A1, 0x0A3, 0x0A5, 0x0A7, 0xFAA, 0x0AF, 0x0A0,
0x0F0, 0x0F1, 0x0F3, 0x0F5, 0x0F7, 0x0FA, 0xFFF, 0x0F0,
0x000, 0x001, 0x003, 0x005, 0x007, 0x00A, 0x00F, 0xF00, },

// 09: 2 lines of green, 2 red, 2 orange, 2 green
{ 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,		// 0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0,
0x0F0, 0x0F0, 0x0FF, 0x0FF, 0x00F, 0x00F, 0x0F0, 0x0F0, },

// 10: multicolor smiley face
{ 0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x00F, 0x000, 0xF00, 0x000, 0x000, 0xF00, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x00F,
0x00F, 0x000, 0x0F0, 0x000, 0x000, 0x0F0, 0x000, 0x00F,
0x00F, 0x000, 0x000, 0x0F4, 0x0F3, 0x000, 0x000, 0x00F,
0x000, 0x00F, 0x000, 0x000, 0x000, 0x000, 0x00F, 0x000,
0x000, 0x000, 0x00F, 0x00F, 0x00F, 0x00F, 0x000, 0x000, },
};
#endif


// Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
void fixdrawRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h) {
	// work around "a15 cannot be used in asm here" compiler bug when using an array on ESP8266
	// uint16_t RGB_bmp_fixed[w * h];
	static uint16_t* RGB_bmp_fixed = (uint16_t*)malloc(w * h * 2);
	for (uint16_t pixel = 0; pixel < w * h; pixel++) {
		uint8_t r, g, b;
		uint16_t color = pgm_read_word(bitmap + pixel);

		//Serial.print(color, HEX);
		b = (color & 0xF00) >> 8;
		g = (color & 0x0F0) >> 4;
		r = color & 0x00F;
		// expand from 4/4/4 bits per color to 5/6/5
		b = map(b, 0, 15, 0, 31);
		g = map(g, 0, 15, 0, 63);
		r = map(r, 0, 15, 0, 31);
		RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
		//Serial.print(" -> ");
		//Serial.println(RGB_bmp_fixed[pixel], HEX);
	}
	matrix->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
}

//TODO FIXEN!!!!
// In a case of a tile of neomatrices, this test is helpful to make sure that the
// pixels are all in sequence (to check your wiring order and the tile options you
// gave to the constructor).
void count_pixels() {
		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();
		for (uint16_t i = 0; i < MATRIX_HEIGHT; i++) {
			for (uint16_t j = 0; j < MATRIX_WIDTH; j++) {
				matrix->drawPixel(j, i, i % 3 == 0 ? (uint16_t)LED_BLUE_HIGH : i % 3 == 1 ? (uint16_t)LED_RED_HIGH : (uint16_t)LED_GREEN_HIGH);
				// depending on the matrix size, it's too slow to display each pixel, so
				// make the scan init faster. This will however be too fast on a small matrix.
				matrix->show();
			}
		}
	}
}

// Fill the screen with multiple levels of white to gauge the quality
void display_four_white() {
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();
		matrix->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, LED_WHITE_HIGH);
		matrix->drawRect(1, 1, MATRIX_WIDTH - 2, MATRIX_HEIGHT - 2, LED_WHITE_MEDIUM);
		matrix->drawRect(2, 2, MATRIX_WIDTH - 4, MATRIX_HEIGHT - 4, LED_WHITE_LOW);
		matrix->drawRect(3, 3, MATRIX_WIDTH - 6, MATRIX_HEIGHT - 6, LED_WHITE_VERYLOW);
		matrix->show();
	}
}

void display_bitmap(uint8_t bmp_num, uint16_t color) {
	static uint16_t bmx, bmy;
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		//bmx = 4;
		//bmy = 4;
		
		// Clear the space under the bitmap that will be drawn as
		// drawing a single color pixmap does not write over pixels
		// that are nul, and leaves the data that was underneath
		matrix->fillRect(bmx, bmy, bmx + 8, bmy + 8, LED_BLACK);
		matrix->drawBitmap(bmx, bmy, mono_bmp[bmp_num], 8, 8, color);
		bmx += 8;
		if (bmx >= mw) bmx = 0;
		if (!bmx) bmy += 8;
		if (bmy >= mh) bmy = 0;
		matrix->show();
	}
}

void progDisplay_bitmap(unsigned int durationMillis, byte nextPart, uint8_t bmp_num, uint16_t color) {

	//--- standard-part um dauer und naechstes programm zu speichern ----
	if (!nextChangeMillisAlreadyCalculated) {
		FastLED.clear(true);
		// workaround: die eigentlichen millis werden korrigiert auf die faktische dauer
		//nextChangeMillis = round((float)durationMillis / (float)1.0f);	// TODO: diesen wert eurieren und anpassen!!
		nextChangeMillis = durationMillis;
		nextSongPart = nextPart;
		nextChangeMillisAlreadyCalculated = true;

		display_bitmap(bmp_num, color);
	}
	//---------------------------------------------------------------------
}

void display_rgbBitmap(uint8_t bmp_num) {
	static uint16_t bmx, bmy;

	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		fixdrawRGBBitmap(bmx, bmy, RGB_bmp[bmp_num], 8, 8);
		bmx += 8;
		if (bmx >= MATRIX_WIDTH) bmx = 0;
		if (!bmx) bmy += 8;
		if (bmy >= MATRIX_HEIGHT) bmy = 0;
		matrix->show();

		if (LEDGITBOARD == false) {
			gitBlindingLEDs_OFF_MarkerLEDs_ON(); // funktioniert alleine so nicht gut ...zusaetzlich die bitmap editieren und schwarz setzen
			FastLED.show();
		}
	}
}

void display_lines() {
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();

		// 4 levels of crossing red lines.
		matrix->drawLine(0, mh / 2 - 2, mw - 1, 2, LED_RED_VERYLOW);
		matrix->drawLine(0, mh / 2 - 1, mw - 1, 3, LED_RED_LOW);
		matrix->drawLine(0, mh / 2, mw - 1, mh / 2, LED_RED_MEDIUM);
		matrix->drawLine(0, mh / 2 + 1, mw - 1, mh / 2 + 1, LED_RED_HIGH);

		// 4 levels of crossing green lines.
		matrix->drawLine(mw / 2 - 2, 0, mw / 2 - 2, mh - 1, LED_GREEN_VERYLOW);
		matrix->drawLine(mw / 2 - 1, 0, mw / 2 - 1, mh - 1, LED_GREEN_LOW);
		matrix->drawLine(mw / 2 + 0, 0, mw / 2 + 0, mh - 1, LED_GREEN_MEDIUM);
		matrix->drawLine(mw / 2 + 1, 0, mw / 2 + 1, mh - 1, LED_GREEN_HIGH);

		// Diagonal blue line.
		matrix->drawLine(0, 0, mw - 1, mh - 1, LED_BLUE_HIGH);
		matrix->drawLine(0, mh - 1, mw - 1, 0, LED_ORANGE_MEDIUM);
		matrix->show();
	}
}

void display_boxes() {
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();
		matrix->drawRect(0, 0, mw, mh, LED_BLUE_HIGH);
		matrix->drawRect(1, 1, mw - 2, mh - 2, LED_GREEN_MEDIUM);
		matrix->fillRect(2, 2, mw - 4, mh - 4, LED_RED_HIGH);
		matrix->fillRect(3, 3, mw - 6, mh - 6, LED_ORANGE_MEDIUM);
		matrix->show();
	}
}

void display_circles() {
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
	matrix->clear();
	matrix->drawCircle(MATRIX_WIDTH / 2, MATRIX_HEIGHT / 2, 2, LED_RED_MEDIUM);
	matrix->drawCircle(MATRIX_WIDTH / 2 - 1 - min(MATRIX_WIDTH, MATRIX_HEIGHT) / 8, MATRIX_HEIGHT / 2 - 1 - min(MATRIX_WIDTH, MATRIX_HEIGHT) / 8, min(MATRIX_WIDTH, MATRIX_HEIGHT) / 4, LED_BLUE_HIGH);
	matrix->drawCircle(MATRIX_WIDTH / 2 + 1 + min(MATRIX_WIDTH, MATRIX_HEIGHT) / 8, MATRIX_HEIGHT / 2 + 1 + min(MATRIX_WIDTH, MATRIX_HEIGHT) / 8, min(MATRIX_WIDTH, MATRIX_HEIGHT) / 4 - 1, LED_ORANGE_MEDIUM);
	matrix->drawCircle(1, MATRIX_HEIGHT - 2, 1, LED_GREEN_LOW);
	matrix->drawCircle(MATRIX_WIDTH - 2, 1, 1, LED_GREEN_HIGH);
	if (min(MATRIX_WIDTH, MATRIX_HEIGHT) > 12) matrix->drawCircle(MATRIX_WIDTH / 2 - 1, mh / 2 - 1, min(MATRIX_HEIGHT / 2 - 1, MATRIX_WIDTH / 2 - 1), LED_CYAN_HIGH);
	matrix->show();
	}
}

void display_resolution() {
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->setTextSize(1);
		// not wide enough;
		if (MATRIX_WIDTH < 16) return;
		matrix->clear();
		// Font is 5x7, if display is too small
		// 8 can only display 1 char
		// 16 can almost display 3 chars
		// 24 can display 4 chars
		// 32 can display 5 chars
		matrix->setCursor(0, 0);
		matrix->setTextColor(matrix->Color(255, 0, 0));
		if (MATRIX_WIDTH > 10) matrix->print(MATRIX_WIDTH / 10);
		matrix->setTextColor(matrix->Color(255, 128, 0));
		matrix->print(MATRIX_WIDTH % 10);
		matrix->setTextColor(matrix->Color(0, 255, 0));
		matrix->print('x');
		// not wide enough to print 5 chars, go to next line
		if (MATRIX_WIDTH < 25) {
			if (MATRIX_HEIGHT == 13) matrix->setCursor(6, 7);
			else if (MATRIX_HEIGHT >= 13) {
				matrix->setCursor(MATRIX_WIDTH - 11, 8);
			}
			else {
				// we're not tall enough either, so we wait and display
				// the 2nd value on top.
				matrix->show();
				delay(2000);
				matrix->clear();
				matrix->setCursor(MATRIX_WIDTH - 11, 0);
			}
		}
		matrix->setTextColor(matrix->Color(0, 255, 128));
		matrix->print(MATRIX_HEIGHT / 10);
		matrix->setTextColor(matrix->Color(0, 128, 255));
		matrix->print(MATRIX_HEIGHT % 10);
		// enough room for a 2nd line
		if ((MATRIX_WIDTH > 25 && MATRIX_HEIGHT > 14) || MATRIX_HEIGHT > 16) {
			matrix->setCursor(0, MATRIX_HEIGHT - 7);
			matrix->setTextColor(matrix->Color(0, 255, 255));
			if (MATRIX_WIDTH > 16) matrix->print('*');
			matrix->setTextColor(matrix->Color(255, 0, 0));
			matrix->print('R');
			matrix->setTextColor(matrix->Color(0, 255, 0));
			matrix->print('G');
			matrix->setTextColor(matrix->Color(0, 0, 255));
			matrix->print("B");
			matrix->setTextColor(matrix->Color(255, 255, 0));
			// this one could be displayed off screen, but we don't care :)
			matrix->print("*");

			// We have a big array, great, let's assume 32x32 and add something in the middle
			if (MATRIX_HEIGHT > 24 && MATRIX_WIDTH > 25) {
				for (uint16_t i = 0; i < MATRIX_WIDTH; i += 8) fixdrawRGBBitmap(i, MATRIX_HEIGHT / 2 - 7 + (i % 16) / 8 * 6, RGB_bmp[10], 8, 8);
			}
		}

		matrix->show();
	}
}

void display_scrollText() {
	uint8_t size = max(int(MATRIX_WIDTH / 8), 1);
	if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
		matrix->clear();
		matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
		matrix->setTextSize(1);
		matrix->setRotation(0);
		for (int8_t x = 7; x >= -42; x--) {
			yield();
			matrix->clear();
			matrix->setCursor(x, 0);
			matrix->setTextColor(LED_GREEN_HIGH);
			matrix->print("Hello");
			if (MATRIX_HEIGHT > 11) {
				matrix->setCursor(-20 - x, MATRIX_HEIGHT - 7);
				matrix->setTextColor(LED_ORANGE_HIGH);
				matrix->print("World");
			}
			matrix->show();
			delay(50);
		}

		matrix->setRotation(3);
		matrix->setTextSize(size);
		matrix->setTextColor(LED_BLUE_HIGH);
		for (int16_t x = 8 * size; x >= -6 * 8 * size; x--) {
			yield();
			matrix->clear();
			matrix->setCursor(x, MATRIX_WIDTH / 2 - size * 4);
			matrix->print("Rotate");
			matrix->show();
			// note that on a big array the refresh rate from show() will be slow enough that
			// the delay become irrelevant. This is already true on a 32x32 array.
			delay(50 / size);
		}
		matrix->setRotation(0);
		matrix->setCursor(0, 0);
		matrix->show();
	}
}

// Scroll within big bitmap so that all of it becomes visible or bounce a small one.
// If the bitmap is bigger in one dimension and smaller in the other one, it will
// be both panned and bounced in the appropriate dimensions.
void display_panOrBounceBitmap(uint8_t bitmapSize) {
	// keep integer math, deal with values 16 times too big
	// start by showing upper left of big bitmap or centering if the display is big
	int16_t xf = max(0, (MATRIX_WIDTH - bitmapSize) / 2) << 4;
	int16_t yf = max(0, (MATRIX_HEIGHT - bitmapSize) / 2) << 4;
	// scroll speed in 1/16th
	int16_t xfc = 6;
	int16_t yfc = 3;
	// scroll down and right by moving upper left corner off screen 
	// more up and left (which means negative numbers)
	int16_t xfdir = -1;
	int16_t yfdir = -1;

	for (uint16_t i = 1; i < 200; i++) {
		bool updDir = false;

		// Get actual x/y by dividing by 16.
		int16_t x = xf >> 4;
		int16_t y = yf >> 4;

		if (!LEDsTurnedOff) {	// nur wenn LEDs an sind (for rotary encoder button push)
			matrix->clear();
			// bounce 8x8 tri color smiley face around the screen
			if (bitmapSize == 8) fixdrawRGBBitmap(x, y, RGB_bmp[10], 8, 8);
			// pan 24x24 pixmap
			if (bitmapSize == 24) matrix->drawRGBBitmap(x, y, (const uint16_t*)bitmap24, bitmapSize, bitmapSize);
	#ifdef BM32
			if (bitmapSize == 32) matrix->drawRGBBitmap(x, y, (const uint16_t*)bitmap32, bitmapSize, bitmapSize);
	#endif
			matrix->show();
		}

		// Only pan if the display size is smaller than the pixmap
		// but not if the difference is too small or it'll look bad.
		if (bitmapSize - MATRIX_WIDTH > 2) {
			xf += xfc * xfdir;
			if (xf >= 0) { xfdir = -1; updDir = true; };
			// we don't go negative past right corner, go back positive
			if (xf <= ((MATRIX_WIDTH - bitmapSize) << 4)) { xfdir = 1;  updDir = true; };
		}
		if (bitmapSize - MATRIX_HEIGHT > 2) {
			yf += yfc * yfdir;
			// we shouldn't display past left corner, reverse direction.
			if (yf >= 0) { yfdir = -1; updDir = true; };
			if (yf <= ((MATRIX_HEIGHT - bitmapSize) << 4)) { yfdir = 1;  updDir = true; };
		}
		// only bounce a pixmap if it's smaller than the display size
		if (MATRIX_WIDTH > bitmapSize) {
			xf += xfc * xfdir;
			// Deal with bouncing off the 'walls'
			if (xf >= (MATRIX_WIDTH - bitmapSize) << 4) { xfdir = -1; updDir = true; };
			if (xf <= 0) { xfdir = 1; updDir = true; };
		}
		if (MATRIX_HEIGHT > bitmapSize) {
			yf += yfc * yfdir;
			if (yf >= (MATRIX_HEIGHT - bitmapSize) << 4) { yfdir = -1; updDir = true; };
			if (yf <= 0) { yfdir = 1; updDir = true; };
		}

		if (updDir) {
			// Add -1, 0 or 1 but bind result to 1 to 1.
			// Let's take 3 is a minimum speed, otherwise it's too slow.
			xfc = constrain(xfc + random(-1, 2), 3, 16);
			yfc = constrain(xfc + random(-1, 2), 3, 16);
		}
		delay(10);
	}
}

