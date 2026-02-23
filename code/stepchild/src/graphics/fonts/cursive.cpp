/*
At some point, this (and all the font functions) should just read out bytes from spritemaps.
Maybe consider making them GFX glyphs? So you can integrate it w/ the adafruit libraries.
(don't reinvent the wheel!)
*/


#include "StepchildGraphics.h"
#include "Stepchild.h"
extern Stepchild stepchild;
using namespace std;


//CURSIVE FONT CHARACTERS----------------------------
// 'a', 6x6px
const unsigned char cursive_a []  = {
	0x00, 0x00, 0x18, 0x28, 0x48, 0xb4
};
// 'b', 6x6px
const unsigned char cursive_b []  = {
  0x80, 0x40, 0x70, 0x50, 0x48, 0xf4
};
// 'c', 6x6px
const unsigned char cursive_c []  = {
	0x00, 0x00, 0x20, 0x40, 0x48, 0xb4
};
// 'd', 6x6px
const unsigned char cursive_d []  = {
	0x04, 0x04, 0x08, 0x38, 0x68, 0xb4
};
// 'e', 6x6px
const unsigned char cursive_e []  = {
	0x00, 0x10, 0x28, 0x30, 0x64, 0x98
};
// 'f', 6x6px
const unsigned char cursive_f []  = {
  0x18, 0x20, 0x30, 0x68, 0xa4, 0x20
};
// 'g', 6x6px
const unsigned char cursive_g []  = {
  0x18, 0x28, 0x28, 0x18, 0x4c, 0xb0
};
// 'h', 6x6px
const unsigned char cursive_h []  = {
  0x20, 0x20, 0x40, 0x60, 0xd4, 0x58
};
// 'i', 6x6px
const unsigned char cursive_i []  = {
	0x00, 0x04, 0x00, 0x10, 0x30, 0xcc
};
// 'k', 6x6px
const unsigned char cursive_k []  = {
  0x40, 0x20, 0x28, 0x30, 0x70, 0xac
};
// 'j', 6x6px
const unsigned char cursive_j []  = {
  0x08, 0x00, 0x10, 0x30, 0x58, 0xb4
};
// 'l', 6x6px
const unsigned char cursive_l []  = {
  0x08, 0x08, 0x18, 0x10, 0x30, 0xcc
};
// 'm', 6x6px
const unsigned char cursive_m []  = {
	0x00, 0x00, 0x28, 0x54, 0xd4, 0xd4
};
// 'n', 6x6px
const unsigned char cursive_n []  = {
  0x00, 0x00, 0x08, 0x14, 0x34, 0xd4
};
// 'o', 6x6px
const unsigned char cursive_o []  = {
  0x00, 0x00, 0x18, 0x24, 0x64, 0x98
};
// 'p', 6x6px
const unsigned char cursive_p []  = {
	0x18, 0x14, 0x1c, 0x30, 0xa8, 0x64
};
// 'q', 6x6px
const unsigned char cursive_q []  = {
  0x40, 0xa0, 0xa0, 0x60, 0x24, 0xd8
};
// 's', 6x6px
const unsigned char cursive_s []  = {
	0x00, 0x08, 0x18, 0x28, 0x48, 0xb4
};
// 'r', 6x6px
const unsigned char cursive_r []  = {
	0x00, 0x00, 0x38, 0x48, 0x50, 0x8c
};
// 't', 6x6px
const unsigned char cursive_t []  = {
	0x10, 0x70, 0x1c, 0x30, 0x60, 0x9c
};
// 'v', 6x6px
const unsigned char cursive_v []  = {
  0x00, 0x00, 0x24, 0x24, 0x54, 0x88
};
// 'x', 6x6px
const unsigned char cursive_x []  = {
  0x00, 0x00, 0x48, 0x30, 0x34, 0xc8
};
// 'w', 6x6px
const unsigned char cursive_w []  = {
	0x00, 0x00, 0x40, 0x94, 0xa4, 0x58
};
// 'z', 6x6px
const unsigned char cursive_z []  = {
  0x10, 0x28, 0x48, 0x94, 0x04, 0x08
};
// 'y', 6x6px
const unsigned char cursive_y []  = {
  0x20, 0x28, 0x28, 0x78, 0xd4, 0x20
};
// 'u', 6x6px
const unsigned char cursive_u []  = {
	0x00, 0x00, 0x24, 0x24, 0x6c, 0xb8
};
// 'big_f', 6x12px
const unsigned char cursive_big_f []  = {
	0x18, 0x14, 0x24, 0x28, 0x50, 0x7c, 0xe0, 0x50, 0x50, 0x48, 0x28, 0x10
};
// 'big_g', 6x12px
const unsigned char cursive_big_g []  = {
	0x00, 0x00, 0x18, 0x28, 0x68, 0xbc, 0x08, 0x18, 0x28, 0x50, 0x50, 0x60
};
// 'big_y', 6x12px
const unsigned char cursive_big_y []  = {
	0x00, 0x00, 0x20, 0x68, 0xa8, 0x9c, 0x08, 0x18, 0x28, 0x50, 0x50, 0x60
};
// 'big_p', 6x12px
const unsigned char cursive_big_p []  = {
	0x00, 0x18, 0x14, 0x24, 0x38, 0xe4, 0x40, 0x40, 0x40, 0x80, 0x80, 0x00
};
// 'big_j', 6x12px
const unsigned char cursive_big_j []  = {
	0x10, 0x00, 0x10, 0x30, 0x54, 0x98, 0x30, 0x50, 0x50, 0x90, 0xa0, 0x40
};
// 'big_b', 6x8px
const unsigned char cursive_big_b []  = {
	0x20, 0x50, 0x50, 0x60, 0xcc, 0x48, 0x48, 0x30
};

// '0', 6x6px
const unsigned char cursive_0 []  = {
	0x38, 0x44, 0xc4, 0x8c, 0x88, 0x70
};
// '9', 6x6px
const unsigned char cursive_9 []  = {
	0x78, 0x88, 0x78, 0x10, 0x20, 0x20
};
// '8', 6x6px
const unsigned char cursive_8 []  = {
	0x3c, 0x44, 0x7c, 0x88, 0x88, 0xf0
};
// '7', 6x6px
const unsigned char cursive_7 []  = {
	0xc4, 0x38, 0x08, 0x10, 0x20, 0x20
};
// '6', 6x6px
const unsigned char cursive_6 []  = {
	0x1c, 0x20, 0x70, 0x88, 0x88, 0x70
};
// '5', 6x6px
const unsigned char cursive_5 []  = {
	0x3c, 0x40, 0x70, 0x08, 0x88, 0x70
};
// '4', 6x6px
const unsigned char cursive_4 []  = {
	0x20, 0x44, 0x88, 0xfc, 0x10, 0x20
};
// '3', 6x6px
const unsigned char cursive_3 []  = {
	0x38, 0x44, 0x08, 0x1c, 0x88, 0x70
};
// '2', 6x6px
const unsigned char cursive_2 []  = {
	0x18, 0x24, 0x04, 0x08, 0x30, 0xfc
};
// '1', 6x6px
const unsigned char cursive_1 []  = {
	0x18, 0x38, 0x08, 0x10, 0x30, 0xfc
};
// 'period', 6x6px
const unsigned char cursive_period []  = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40
};
// 'dash', 6x6px
const unsigned char cursive_dash []  = {
	0x00, 0x00, 0x7c, 0xf8, 0x00, 0x00
};
// 'plus', 6x6px
const unsigned char cursive_plus []  = {
	0x10, 0x10, 0x7c, 0xf8, 0x20, 0x20
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 832)
const unsigned char* cursive_font[39] = {
	cursive_a,
	// cursive_b,
	cursive_big_b,
	cursive_c,
	cursive_d,
	cursive_e,
	cursive_big_f,
	// cursive_f,
	cursive_big_g,
	// cursive_g,
	cursive_h,
	cursive_i,
	cursive_big_j,
	// cursive_j,
	cursive_k,
	cursive_l,
	cursive_m,
	cursive_n,
	cursive_o,
	cursive_big_p,
	// cursive_p,
	cursive_q,
	cursive_r,
	cursive_s,
	cursive_t,
	cursive_u,
	cursive_v,
	cursive_w,
	cursive_x,
	cursive_big_y,
	// cursive_y,
	cursive_z,
	cursive_0,
	cursive_1,
	cursive_2,
	cursive_3,
	cursive_4,
	cursive_5,
	cursive_6,
	cursive_7,
	cursive_8,
	cursive_9,
	cursive_period,
	cursive_dash,
	cursive_plus
};

//Cursive font
void StepchildGraphics::printCursive(int x, int y, String text, uint16_t c){
  for(uint16_t letter = 0; letter<text.length(); letter++){
    char character = text.charAt(letter);
    switch(character){
      case ' ':
        continue;
      case 'a':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[0],6,6,c);
        break;
      case 'b':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[1],6,8,c);
        break;
      case 'c':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[2],6,6,c);
        break;
      case 'd':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[3],6,6,c);
        break;
      case 'e':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[4],6,6,c);
        break;
      case 'f':
        // stepchild.display.drawBitmap(x+6*letter,y,cursive_font[5],6,6,c);
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[5],6,12,c);
        break;
      case 'g':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[6],6,12,c);
        break;
      case 'h':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[7],6,6,c);
        break;
      case 'i':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[8],6,6,c);
        break;
      case 'j':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[9],6,12,c);
        break;
      case 'k':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[10],6,6,c);
        break;
      case 'l':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[11],6,6,c);
        break;
      case 'm':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[12],6,6,c);
        break;
      case 'n':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[13],6,6,c);
        break;
      case 'o':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[14],6,6,c);
        break;
      case 'p':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[15],6,12,c);
        break;
      case 'q':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[16],6,6,c);
        break;
      case 'r':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[17],6,6,c);
        break;
      case 's':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[18],6,6,c);
        break;
      case 't':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[19],6,6,c);
        break;
      case 'u':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[20],6,6,c);
        break;
      case 'v':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[21],6,6,c);
        break;
      case 'w':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[22],6,6,c);
        break;
      case 'x':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[23],6,6,c);
        break;
      case 'y':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[24],6,12,c);
        break;
      case 'z':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[25],6,6,c);
        break;
      //uppercase
      case 'A':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[0],6,6,c);
        break;
      case 'B':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[1],6,8,c);
        break;
      case 'C':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[2],6,6,c);
        break;
      case 'D':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[3],6,6,c);
        break;
      case 'E':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[4],6,6,c);
        break;
      case 'F':
        // stepchild.display.drawBitmap(x+6*letter,y,cursive_font[5],6,6,c);
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[5],6,12,c);
        break;
      case 'G':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[6],6,12,c);
        break;
      case 'H':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[7],6,6,c);
        break;
      case 'I':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[8],6,6,c);
        break;
      case 'J':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[9],6,12,c);
        break;
      case 'K':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[10],6,6,c);
        break;
      case 'L':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[11],6,6,c);
        break;
      case 'M':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[12],6,6,c);
        break;
      case 'N':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[13],6,6,c);
        break;
      case 'O':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[14],6,6,c);
        break;
      case 'P':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[15],6,12,c);
        break;
      case 'Q':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[16],6,6,c);
        break;
      case 'R':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[17],6,6,c);
        break;
      case 'S':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[18],6,6,c);
        break;
      case 'T':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[19],6,6,c);
        break;
      case 'U':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[20],6,6,c);
        break;
      case 'V':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[21],6,6,c);
        break;
      case 'W':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[22],6,6,c);
        break;
      case 'X':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[23],6,6,c);
        break;
      case 'Y':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[24],6,12,c);
        break;
      case 'Z':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[25],6,6,c);
        break;
      //numbers
      case '0':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[26],6,6,c);
        break;
      case '1':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[27],6,6,c);
        break;
      case '2':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[28],6,6,c);
        break;
      case '3':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[29],6,6,c);
        break;
      case '4':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[30],6,6,c);
        break;
      case '5':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[31],6,6,c);
        break;
      case '6':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[32],6,6,c);
        break;
      case '7':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[33],6,6,c);
        break;
      case '8':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[34],6,6,c);
        break;
      case '9':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[35],6,6,c);
        break;
      case '.':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[36],6,6,c);
        x-=3;
        break;
      case '-':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[37],6,6,c);
        break;
      case '+':
        stepchild.display.drawBitmap(x+6*letter,y,cursive_font[38],6,6,c);
        break;
    }
  }
}

void StepchildGraphics::printCursive_centered(int x, int y, String t, uint16_t c){
  printCursive(x-t.length()*3,y,t,c);
}

