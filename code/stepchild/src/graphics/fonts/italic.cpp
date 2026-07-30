
#include "Stepchild.h"
;
using namespace std;

// 'a', 7x7px
const unsigned char italic_a []  = {
	0x3e, 0x3e, 0x6c, 0x6c, 0x7c, 0xc8, 0xc8
};
// 'b', 7x7px
const unsigned char italic_b []  = {
	0x3e, 0x3e, 0x74, 0x7c, 0x6c, 0xe8, 0xf8
};
// 'c', 7x7px
const unsigned char italic_c []  = {
	0x3e, 0x3c, 0x60, 0x60, 0x60, 0xc0, 0xf8
};
// 'd', 7x7px
const unsigned char italic_d []  = {
	0x3c, 0x3e, 0x64, 0x64, 0x64, 0xc8, 0xf8
};
// 'e', 7x7px
const unsigned char italic_e []  = {
	0x3e, 0x3c, 0x60, 0x60, 0x7c, 0xc0, 0xf8
};
// 'f', 7x7px
const unsigned char italic_f []  = {
	0x3e, 0x3c, 0x60, 0x7c, 0x60, 0xc0, 0xc0
};
// 'g', 7x7px
const unsigned char italic_g []  = {
	0x3e, 0x3c, 0x60, 0x60, 0x6c, 0xc4, 0xf8
};
// 'h', 7x7px
const unsigned char italic_h []  = {
	0x32, 0x32, 0x64, 0x7c, 0x64, 0xc8, 0xc8
};
// 'i', 7x7px
const unsigned char italic_i []  = {
	0x7e, 0x18, 0x30, 0x30, 0x30, 0x60, 0xf8
};
// 'j', 7x7px
const unsigned char italic_j []  = {
	0x06, 0x06, 0x0c, 0x4c, 0x8c, 0x98, 0xf8
};
// 'k', 7x7px
const unsigned char italic_k []  = {
	0x32, 0x36, 0x7c, 0x70, 0x70, 0xc8, 0xcc
};
// 'l', 7x7px
const unsigned char italic_l []  = {
	0x18, 0x18, 0x30, 0x30, 0x30, 0x60, 0xfc
};
// 'm', 7x7px
const unsigned char italic_m []  = {
	0x76, 0x76, 0x7e, 0x56, 0xc4, 0xcc, 0xcc
};
// 'n', 7x7px
const unsigned char italic_n []  = {
	0x32, 0x32, 0x74, 0x74, 0x7c, 0xd8, 0xd8
};
// 'o', 7x7px
const unsigned char italic_o []  = {
	0x3e, 0x3e, 0x64, 0x64, 0x4c, 0xc8, 0xf8
};
// 'p', 7x7px
const unsigned char italic_p []  = {
	0x3e, 0x3e, 0x64, 0x78, 0x60, 0xc0, 0xc0
};
// 'r', 7x7px
const unsigned char italic_r []  = {
	0x3e, 0x3e, 0x64, 0x78, 0x70, 0xd8, 0xcc
};
// 'q', 7x7px
const unsigned char italic_q []  = {
	0x3e, 0x3e, 0x64, 0x74, 0x4c, 0xcc, 0xfa
};
// 't', 7x7px
const unsigned char italic_t []  = {
	0x7e, 0xfc, 0x30, 0x70, 0x60, 0xe0, 0xc0
};
// 's', 7x7px
const unsigned char italic_s []  = {
	0x1c, 0x38, 0x60, 0x38, 0x18, 0xf8, 0x70
};
// 'w', 7x7px
const unsigned char italic_w []  = {
	0xc6, 0xc6, 0x86, 0xac, 0xec, 0xd8, 0xd8
};
// 'u', 7x7px
const unsigned char italic_u []  = {
	0x6c, 0x6c, 0x48, 0xd8, 0xd8, 0xf0, 0x70
};
// 'y', 7x7px
const unsigned char italic_y []  = {
	0x66, 0x76, 0x3c, 0x18, 0x30, 0x60, 0xc0
};
// 'v', 7x7px
const unsigned char italic_v []  = {
	0x32, 0x32, 0x66, 0x6c, 0x78, 0xf0, 0xe0
};
// 'z', 7x7px
const unsigned char italic_z []  = {
	0x3e, 0x7c, 0x18, 0x30, 0x60, 0xfc, 0xf8
};
// 'x', 7x7px
const unsigned char italic_x []  = {
	0x6c, 0x78, 0x30, 0x30, 0x30, 0x78, 0xd8
};
// 'slash', 7x7px
const unsigned char italic_slash [] = {
	0x0c, 0x1c, 0x18, 0x38, 0x30, 0x70, 0x60
};

const unsigned char* italic_font[27] = {
	italic_a,
	italic_b,
	italic_c,
	italic_d,
	italic_e,
	italic_f,
	italic_g,
	italic_h,
	italic_i,
	italic_j,
	italic_k,
	italic_l,
	italic_m,
	italic_n,
	italic_o,
	italic_p,
	italic_q,
	italic_r,
	italic_s,
	italic_t,
	italic_u,
	italic_v,
	italic_w,
	italic_x,
	italic_y,
	italic_z,
  italic_slash
};

void StepchildGraphics::printItalic_wave(int16_t x, int16_t y, String text, int8_t height, uint8_t timing, uint16_t c){
  for(uint8_t i = 0; i<text.length(); i++){
    int8_t y1 = y-float(height*2)*sin(millis()/timing+float(i)/float(text.length())*PI)+height;
    if(y1>y)
      y1 = y;
    printItalic(x+i*8, y1, text.charAt(i), c);
  }
}
void StepchildGraphics::printItalic(int16_t x, int16_t y, char character, uint16_t c){
  switch(character){
    case ' ':
      break;
    case 'A':
      stepchild.display.drawBitmap(x,y,italic_font[0],7,7,c);
      break;
    case 'B':
      stepchild.display.drawBitmap(x,y,italic_font[1],7,7,c);
      break;
    case 'C':
      stepchild.display.drawBitmap(x,y,italic_font[2],7,7,c);
      break;
    case 'D':
      stepchild.display.drawBitmap(x,y,italic_font[3],7,7,c);
      break;
    case 'E':
      stepchild.display.drawBitmap(x,y,italic_font[4],7,7,c);
      break;
    case 'F':
      stepchild.display.drawBitmap(x,y,italic_font[5],7,7,c);
      break;
    case 'G':
      stepchild.display.drawBitmap(x,y,italic_font[6],7,7,c);
      break;
    case 'H':
      stepchild.display.drawBitmap(x,y,italic_font[7],7,7,c);
      break;
    case 'I':
      stepchild.display.drawBitmap(x,y,italic_font[8],7,7,c);
      break;
    case 'J':
      stepchild.display.drawBitmap(x,y,italic_font[9],7,7,c);
      break;
    case 'K':
      stepchild.display.drawBitmap(x,y,italic_font[10],7,7,c);
      break;
    case 'L':
      stepchild.display.drawBitmap(x,y,italic_font[11],7,7,c);
      break;
    case 'M':
      stepchild.display.drawBitmap(x,y,italic_font[12],7,7,c);
      break;
    case 'N':
      stepchild.display.drawBitmap(x,y,italic_font[13],7,7,c);
      break;
    case 'O':
      stepchild.display.drawBitmap(x,y,italic_font[14],7,7,c);
      break;
    case 'P':
      stepchild.display.drawBitmap(x,y,italic_font[15],7,7,c);
      break;
    case 'Q':
      stepchild.display.drawBitmap(x,y,italic_font[16],7,7,c);
      break;
    case 'R':
      stepchild.display.drawBitmap(x,y,italic_font[17],7,7,c);
      break;
    case 'S':
      stepchild.display.drawBitmap(x,y,italic_font[18],7,7,c);
      break;
    case 'T':
      stepchild.display.drawBitmap(x,y,italic_font[19],7,7,c);
      break;
    case 'U':
      stepchild.display.drawBitmap(x,y,italic_font[20],7,7,c);
      break;
    case 'V':
      stepchild.display.drawBitmap(x,y,italic_font[21],7,7,c);
      break;
    case 'W':
      stepchild.display.drawBitmap(x,y,italic_font[22],7,7,c);
      break;
    case 'X':
      stepchild.display.drawBitmap(x,y,italic_font[23],7,7,c);
      break;
    case 'Y':
      stepchild.display.drawBitmap(x,y,italic_font[24],7,7,c);
      break;
    case 'Z':
      stepchild.display.drawBitmap(x,y,italic_font[25],7,7,c);
      break;
    case 'a':
      stepchild.display.drawBitmap(x,y,italic_font[0],7,7,c);
      break;
    case 'b':
      stepchild.display.drawBitmap(x,y,italic_font[1],7,7,c);
      break;
    case 'c':
      stepchild.display.drawBitmap(x,y,italic_font[2],7,7,c);
      break;
    case 'd':
      stepchild.display.drawBitmap(x,y,italic_font[3],7,7,c);
      break;
    case 'e':
      stepchild.display.drawBitmap(x,y,italic_font[4],7,7,c);
      break;
    case 'f':
      stepchild.display.drawBitmap(x,y,italic_font[5],7,7,c);
      break;
    case 'g':
      stepchild.display.drawBitmap(x,y,italic_font[6],7,7,c);
      break;
    case 'h':
      stepchild.display.drawBitmap(x,y,italic_font[7],7,7,c);
      break;
    case 'i':
      stepchild.display.drawBitmap(x,y,italic_font[8],7,7,c);
      break;
    case 'j':
      stepchild.display.drawBitmap(x,y,italic_font[9],7,7,c);
      break;
    case 'k':
      stepchild.display.drawBitmap(x,y,italic_font[10],7,7,c);
      break;
    case 'l':
      stepchild.display.drawBitmap(x,y,italic_font[11],7,7,c);
      break;
    case 'm':
      stepchild.display.drawBitmap(x,y,italic_font[12],7,7,c);
      break;
    case 'n':
      stepchild.display.drawBitmap(x,y,italic_font[13],7,7,c);
      break;
    case 'o':
      stepchild.display.drawBitmap(x,y,italic_font[14],7,7,c);
      break;
    case 'p':
      stepchild.display.drawBitmap(x,y,italic_font[15],7,7,c);
      break;
    case 'q':
      stepchild.display.drawBitmap(x,y,italic_font[16],7,7,c);
      break;
    case 'r':
      stepchild.display.drawBitmap(x,y,italic_font[17],7,7,c);
      break;
    case 's':
      stepchild.display.drawBitmap(x,y,italic_font[18],7,7,c);
      break;
    case 't':
      stepchild.display.drawBitmap(x,y,italic_font[19],7,7,c);
      break;
    case 'u':
      stepchild.display.drawBitmap(x,y,italic_font[20],7,7,c);
      break;
    case 'v':
      stepchild.display.drawBitmap(x,y,italic_font[21],7,7,c);
      break;
    case 'w':
      stepchild.display.drawBitmap(x,y,italic_font[22],7,7,c);
      break;
    case 'x':
      stepchild.display.drawBitmap(x,y,italic_font[23],7,7,c);
      break;
    case 'y':
      stepchild.display.drawBitmap(x,y,italic_font[24],7,7,c);
      break;
    case 'z':
      stepchild.display.drawBitmap(x,y,italic_font[25],7,7,c);
      break;
    case '/':
      stepchild.display.drawBitmap(x,y,italic_font[26],7,7,c);
      break;
  }
}
void StepchildGraphics::printItalic(int16_t x, int16_t y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[0],7,7,c);
        break;
      case 'B':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[1],7,7,c);
        break;
      case 'C':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[2],7,7,c);
        break;
      case 'D':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[3],7,7,c);
        break;
      case 'E':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[4],7,7,c);
        break;
      case 'F':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[5],7,7,c);
        break;
      case 'G':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[6],7,7,c);
        break;
      case 'H':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[7],7,7,c);
        break;
      case 'I':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[8],7,7,c);
        break;
      case 'J':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[9],7,7,c);
        break;
      case 'K':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[10],7,7,c);
        break;
      case 'L':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[11],7,7,c);
        break;
      case 'M':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[12],7,7,c);
        break;
      case 'N':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[13],7,7,c);
        break;
      case 'O':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[14],7,7,c);
        break;
      case 'P':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[15],7,7,c);
        break;
      case 'Q':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[16],7,7,c);
        break;
      case 'R':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[17],7,7,c);
        break;
      case 'S':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[18],7,7,c);
        break;
      case 'T':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[19],7,7,c);
        break;
      case 'U':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[20],7,7,c);
        break;
      case 'V':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[21],7,7,c);
        break;
      case 'W':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[22],7,7,c);
        break;
      case 'X':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[23],7,7,c);
        break;
      case 'Y':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[24],7,7,c);
        break;
      case 'Z':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[25],7,7,c);
        break;
      case 'a':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[0],7,7,c);
        break;
      case 'b':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[1],7,7,c);
        break;
      case 'c':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[2],7,7,c);
        break;
      case 'd':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[3],7,7,c);
        break;
      case 'e':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[4],7,7,c);
        break;
      case 'f':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[5],7,7,c);
        break;
      case 'g':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[6],7,7,c);
        break;
      case 'h':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[7],7,7,c);
        break;
      case 'i':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[8],7,7,c);
        break;
      case 'j':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[9],7,7,c);
        break;
      case 'k':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[10],7,7,c);
        break;
      case 'l':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[11],7,7,c);
        break;
      case 'm':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[12],7,7,c);
        break;
      case 'n':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[13],7,7,c);
        break;
      case 'o':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[14],7,7,c);
        break;
      case 'p':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[15],7,7,c);
        break;
      case 'q':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[16],7,7,c);
        break;
      case 'r':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[17],7,7,c);
        break;
      case 's':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[18],7,7,c);
        break;
      case 't':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[19],7,7,c);
        break;
      case 'u':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[20],7,7,c);
        break;
      case 'v':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[21],7,7,c);
        break;
      case 'w':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[22],7,7,c);
        break;
      case 'x':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[23],7,7,c);
        break;
      case 'y':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[24],7,7,c);
        break;
      case 'z':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[25],7,7,c);
        break;
      case '/':
        stepchild.display.drawBitmap(x+8*letter,y,italic_font[26],7,7,c);
        break;
    }
  }
}

void StepchildGraphics::printItalic_wiggly(int x, int y, String text, uint16_t c){
  for(uint8_t letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    int8_t offset = 3*sin(float(millis())/float(200)+float(letter));
    printItalic(x,y+offset,character,c);
    x+=8;
  }
}
