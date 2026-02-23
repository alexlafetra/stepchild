#include "StepchildGraphics.h"
#include "Stepchild.h"
extern Stepchild stepchild;
using namespace std;

//arp menu title
// 'a', 5x7px
const unsigned char arp_a []  = {
	0xf8, 0xf8, 0xc8, 0xf8, 0xc8, 0xc8, 0xc8
};
// 'b', 5x7px
const unsigned char arp_b []  = {
	0xf8, 0xf8, 0xc8, 0xf0, 0xc8, 0xc8, 0xf8
};
// 'c', 5x7px
const unsigned char arp_c []  = {
	0xf8, 0xf8, 0xc0, 0xc0, 0xc0, 0xc0, 0xf8
};
// 'd', 5x7px
const unsigned char arp_d []  = {
	0xf0, 0xf8, 0xc8, 0xc8, 0xc8, 0xc8, 0xf8
};
// 'e', 5x7px
const unsigned char arp_e []  = {
	0xf8, 0xf8, 0xc0, 0xf8, 0xc0, 0xc0, 0xf8
};
// 'f', 5x7px
const unsigned char arp_f []  = {
	0xf8, 0xf8, 0xc0, 0xf8, 0xc0, 0xc0, 0xc0
};
// 'g', 5x7px
const unsigned char arp_g []  = {
	0xf8, 0xf8, 0xc0, 0xc0, 0xd8, 0xc8, 0xf8
};
// 'h', 5x7px
const unsigned char arp_h []  = {
	0xc8, 0xc8, 0xf8, 0xf8, 0xc8, 0xc8, 0xc8
};
// 'i', 5x7px
const unsigned char arp_i []  = {
	0x70, 0x70, 0x00, 0x70, 0x70, 0x70, 0x70
};
// 'j', 5x7px
const unsigned char arp_j []  = {
	0x18, 0x18, 0x00, 0x18, 0x18, 0x98, 0xf8
};
// 'k', 5x7px
const unsigned char arp_k []  = {
	0xc8, 0xc8, 0xf8, 0xf0, 0xf0, 0xc8, 0xc8
};
// 'l', 5x7px
const unsigned char arp_l []  = {
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xf8
};
// 'm', 6x7px
const unsigned char arp_m []  = {
	0xfc, 0xfc, 0xd4, 0xd4, 0xd4, 0xd4, 0xd4
};
// 'n', 5x7px
const unsigned char arp_n []  = {
	0xf8, 0xf8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8
};
// 'p', 5x7px
const unsigned char arp_p []  = {
	0xf8, 0xf8, 0xc8, 0xf8, 0xc0, 0xc0, 0xc0
};
// 'o', 5x7px
const unsigned char arp_o []  = {
	0xf8, 0xf8, 0xc8, 0xc8, 0xc8, 0xc8, 0xf8
};
// 'r', 5x7px
const unsigned char arp_r []  = {
	0xf8, 0xf8, 0xc8, 0xf8, 0xe0, 0xd0, 0xc8
};
// 's', 5x7px
const unsigned char arp_s []  = {
	0x78, 0xf8, 0xc0, 0xf0, 0x08, 0x08, 0xf0
};
// 't', 5x7px
const unsigned char arp_t []  = {
	0xf8, 0xf8, 0x20, 0x20, 0x20, 0x20, 0x20
};
// 'u', 5x7px
const unsigned char arp_u []  = {
	0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xc8, 0xf8
};
// 'v', 5x7px
const unsigned char arp_v []  = {
	0xc8, 0xc8, 0xc8, 0xc8, 0xd8, 0x70, 0x60
};
// 'w', 6x7px
const unsigned char arp_w []  = {
	0xc4, 0xd4, 0xd4, 0xd4, 0xd4, 0xfc, 0x7c
};
// 'x', 5x7px
const unsigned char arp_x []  = {
	0x88, 0xd8, 0x70, 0x20, 0x70, 0xd8, 0x88
};
// 'y', 5x7px
const unsigned char arp_y []  = {
	0xc8, 0xc8, 0xc8, 0xf8, 0x30, 0x30, 0x30
};
// 'q', 6x7px
const unsigned char arp_q []  = {
	0x7c, 0xfc, 0xc4, 0xc4, 0xd4, 0xc8, 0xf4
};
// 'z', 5x7px
const unsigned char arp_z []  = {
	0xf8, 0xf8, 0x38, 0x70, 0xe0, 0xc0, 0xf8
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 768)
const unsigned char* arpFont[26] = {
	arp_a,
	arp_b,
	arp_c,
	arp_d,
	arp_e,
	arp_f,
	arp_g,
	arp_h,
	arp_i,
	arp_j,
	arp_k,
	arp_l,
	arp_m,
	arp_n,
	arp_o,
	arp_p,
	arp_q,
	arp_r,
	arp_s,
	arp_t,
	arp_u,
	arp_v,
	arp_w,
	arp_x,
	arp_y,
	arp_z
};


void StepchildGraphics::printArp_wiggly(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    int8_t offset = 3*sin(float(millis())/float(200)+float(letter*200));
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[0],5,7,c);
        break;
      case 'B':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[1],5,7,c);
        break;
      case 'C':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[2],5,7,c);
        break;
      case 'D':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[3],5,7,c);
        break;
      case 'E':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[4],5,7,c);
        break;
      case 'F':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[5],5,7,c);
        break;
      case 'G':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[6],5,7,c);
        break;
      case 'H':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[7],5,7,c);
        break;
      case 'I':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[8],5,7,c);
        break;
      case 'J':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[9],5,7,c);
        break;
      case 'K':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[10],5,7,c);
        break;
      case 'L':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[11],5,7,c);
        break;
      case 'M':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[12],6,7,c);
        x++;
        break;
      case 'N':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[13],6,7,c);
        break;
      case 'O':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[14],6,7,c);
        break;
      case 'P':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[15],6,7,c);
        break;
      case 'Q':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[16],6,7,c);
        break;
      case 'R':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[17],6,7,c);
        break;
      case 'S':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[18],6,7,c);
        break;
      case 'T':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[19],6,7,c);
        break;
      case 'U':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[20],6,7,c);
        break;
      case 'V':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[21],6,7,c);
        break;
      case 'W':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[22],6,7,c);
        x++;
        break;
      case 'X':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[23],6,7,c);
        break;
      case 'Y':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[24],6,7,c);
        break;
      case 'Z':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[25],6,7,c);
        break;
      case 'a':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[0],6,7,c);
        break;
      case 'b':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[1],6,7,c);
        break;
      case 'c':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[2],6,7,c);
        break;
      case 'd':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[3],6,7,c);
        break;
      case 'e':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[4],6,7,c);
        break;
      case 'f':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[5],6,7,c);
        break;
      case 'g':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[6],6,7,c);
        break;
      case 'h':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[7],6,7,c);
        break;
      case 'i':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[8],6,7,c);
        break;
      case 'j':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[9],6,7,c);
        break;
      case 'k':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[10],6,7,c);
        break;
      case 'l':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[11],6,7,c);
        break;
      case 'm':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[12],6,7,c);
        x++;
        break;
      case 'n':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[13],6,7,c);
        break;
      case 'o':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[14],6,7,c);
        break;
      case 'p':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[15],6,7,c);
        break;
      case 'q':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[16],6,7,c);
        break;
      case 'r':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[17],6,7,c);
        break;
      case 's':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[18],6,7,c);
        break;
      case 't':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[19],6,7,c);
        break;
      case 'u':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[20],6,7,c);
        break;
      case 'v':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[21],6,7,c);
        break;
      case 'w':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[22],6,7,c);
        x++;
        break;
      case 'x':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[23],6,7,c);
        break;
      case 'y':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[24],6,7,c);
        break;
      case 'z':
        stepchild.display.drawBitmap(x+6*letter,y+offset,arpFont[25],6,7,c);
        break;
    }
  }
}
void StepchildGraphics::printArp(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[0],5,7,c);
        break;
      case 'B':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[1],5,7,c);
        break;
      case 'C':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[2],5,7,c);
        break;
      case 'D':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[3],5,7,c);
        break;
      case 'E':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[4],5,7,c);
        break;
      case 'F':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[5],5,7,c);
        break;
      case 'G':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[6],5,7,c);
        break;
      case 'H':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[7],5,7,c);
        break;
      case 'I':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[8],5,7,c);
        break;
      case 'J':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[9],5,7,c);
        break;
      case 'K':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[10],5,7,c);
        break;
      case 'L':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[11],5,7,c);
        break;
      case 'M':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[12],6,7,c);
        x++;
        break;
      case 'N':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[13],5,7,c);
        break;
      case 'O':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[14],5,7,c);
        break;
      case 'P':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[15],5,7,c);
        break;
      case 'Q':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[16],6,7,c);
        break;
      case 'R':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[17],5,7,c);
        break;
      case 'S':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[18],5,7,c);
        break;
      case 'T':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[19],5,7,c);
        break;
      case 'U':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[20],5,7,c);
        break;
      case 'V':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[21],5,7,c);
        break;
      case 'W':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[22],6,7,c);
        x++;
        break;
      case 'X':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[23],5,7,c);
        break;
      case 'Y':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[24],5,7,c);
        break;
      case 'Z':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[25],5,7,c);
        break;
      case 'a':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[0],5,7,c);
        break;
      case 'b':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[1],5,7,c);
        break;
      case 'c':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[2],5,7,c);
        break;
      case 'd':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[3],5,7,c);
        break;
      case 'e':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[4],5,7,c);
        break;
      case 'f':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[5],5,7,c);
        break;
      case 'g':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[6],5,7,c);
        break;
      case 'h':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[7],5,7,c);
        break;
      case 'i':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[8],5,7,c);
        break;
      case 'j':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[9],5,7,c);
        break;
      case 'k':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[10],5,7,c);
        break;
      case 'l':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[11],5,7,c);
        break;
      case 'm':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[12],6,7,c);
        x++;
        break;
      case 'n':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[13],5,7,c);
        break;
      case 'o':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[14],5,7,c);
        break;
      case 'p':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[15],5,7,c);
        break;
      case 'q':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[16],6,7,c);
        x++;
        break;
      case 'r':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[17],5,7,c);
        break;
      case 's':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[18],5,7,c);
        break;
      case 't':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[19],5,7,c);
        break;
      case 'u':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[20],5,7,c);
        break;
      case 'v':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[21],5,7,c);
        break;
      case 'w':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[22],6,7,c);
        x++;
        break;
      case 'x':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[23],5,7,c);
        break;
      case 'y':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[24],5,7,c);
        break;
      case 'z':
        stepchild.display.drawBitmap(x+6*letter,y,arpFont[25],5,7,c);
        break;
    }
  }
}