

void printItalic(int x, int y, char character, uint16_t c){
  switch(character){
    case ' ':
      break;
    case 'A':
      display.drawBitmap(x,y,italic_font[0],7,7,c);
      break;
    case 'B':
      display.drawBitmap(x,y,italic_font[1],7,7,c);
      break;
    case 'C':
      display.drawBitmap(x,y,italic_font[2],7,7,c);
      break;
    case 'D':
      display.drawBitmap(x,y,italic_font[3],7,7,c);
      break;
    case 'E':
      display.drawBitmap(x,y,italic_font[4],7,7,c);
      break;
    case 'F':
      display.drawBitmap(x,y,italic_font[5],7,7,c);
      break;
    case 'G':
      display.drawBitmap(x,y,italic_font[6],7,7,c);
      break;
    case 'H':
      display.drawBitmap(x,y,italic_font[7],7,7,c);
      break;
    case 'I':
      display.drawBitmap(x,y,italic_font[8],7,7,c);
      break;
    case 'J':
      display.drawBitmap(x,y,italic_font[9],7,7,c);
      break;
    case 'K':
      display.drawBitmap(x,y,italic_font[10],7,7,c);
      break;
    case 'L':
      display.drawBitmap(x,y,italic_font[11],7,7,c);
      break;
    case 'M':
      display.drawBitmap(x,y,italic_font[12],7,7,c);
      break;
    case 'N':
      display.drawBitmap(x,y,italic_font[13],7,7,c);
      break;
    case 'O':
      display.drawBitmap(x,y,italic_font[14],7,7,c);
      break;
    case 'P':
      display.drawBitmap(x,y,italic_font[15],7,7,c);
      break;
    case 'Q':
      display.drawBitmap(x,y,italic_font[16],7,7,c);
      break;
    case 'R':
      display.drawBitmap(x,y,italic_font[17],7,7,c);
      break;
    case 'S':
      display.drawBitmap(x,y,italic_font[18],7,7,c);
      break;
    case 'T':
      display.drawBitmap(x,y,italic_font[19],7,7,c);
      break;
    case 'U':
      display.drawBitmap(x,y,italic_font[20],7,7,c);
      break;
    case 'V':
      display.drawBitmap(x,y,italic_font[21],7,7,c);
      break;
    case 'W':
      display.drawBitmap(x,y,italic_font[22],7,7,c);
      break;
    case 'X':
      display.drawBitmap(x,y,italic_font[23],7,7,c);
      break;
    case 'Y':
      display.drawBitmap(x,y,italic_font[24],7,7,c);
      break;
    case 'Z':
      display.drawBitmap(x,y,italic_font[25],7,7,c);
      break;
    case 'a':
      display.drawBitmap(x,y,italic_font[0],7,7,c);
      break;
    case 'b':
      display.drawBitmap(x,y,italic_font[1],7,7,c);
      break;
    case 'c':
      display.drawBitmap(x,y,italic_font[2],7,7,c);
      break;
    case 'd':
      display.drawBitmap(x,y,italic_font[3],7,7,c);
      break;
    case 'e':
      display.drawBitmap(x,y,italic_font[4],7,7,c);
      break;
    case 'f':
      display.drawBitmap(x,y,italic_font[5],7,7,c);
      break;
    case 'g':
      display.drawBitmap(x,y,italic_font[6],7,7,c);
      break;
    case 'h':
      display.drawBitmap(x,y,italic_font[7],7,7,c);
      break;
    case 'i':
      display.drawBitmap(x,y,italic_font[8],7,7,c);
      break;
    case 'j':
      display.drawBitmap(x,y,italic_font[9],7,7,c);
      break;
    case 'k':
      display.drawBitmap(x,y,italic_font[10],7,7,c);
      break;
    case 'l':
      display.drawBitmap(x,y,italic_font[11],7,7,c);
      break;
    case 'm':
      display.drawBitmap(x,y,italic_font[12],7,7,c);
      break;
    case 'n':
      display.drawBitmap(x,y,italic_font[13],7,7,c);
      break;
    case 'o':
      display.drawBitmap(x,y,italic_font[14],7,7,c);
      break;
    case 'p':
      display.drawBitmap(x,y,italic_font[15],7,7,c);
      break;
    case 'q':
      display.drawBitmap(x,y,italic_font[16],7,7,c);
      break;
    case 'r':
      display.drawBitmap(x,y,italic_font[17],7,7,c);
      break;
    case 's':
      display.drawBitmap(x,y,italic_font[18],7,7,c);
      break;
    case 't':
      display.drawBitmap(x,y,italic_font[19],7,7,c);
      break;
    case 'u':
      display.drawBitmap(x,y,italic_font[20],7,7,c);
      break;
    case 'v':
      display.drawBitmap(x,y,italic_font[21],7,7,c);
      break;
    case 'w':
      display.drawBitmap(x,y,italic_font[22],7,7,c);
      break;
    case 'x':
      display.drawBitmap(x,y,italic_font[23],7,7,c);
      break;
    case 'y':
      display.drawBitmap(x,y,italic_font[24],7,7,c);
      break;
    case 'z':
      display.drawBitmap(x,y,italic_font[25],7,7,c);
      break;
  }
}
void printItalic(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        display.drawBitmap(x+8*letter,y,italic_font[0],7,7,c);
        break;
      case 'B':
        display.drawBitmap(x+8*letter,y,italic_font[1],7,7,c);
        break;
      case 'C':
        display.drawBitmap(x+8*letter,y,italic_font[2],7,7,c);
        break;
      case 'D':
        display.drawBitmap(x+8*letter,y,italic_font[3],7,7,c);
        break;
      case 'E':
        display.drawBitmap(x+8*letter,y,italic_font[4],7,7,c);
        break;
      case 'F':
        display.drawBitmap(x+8*letter,y,italic_font[5],7,7,c);
        break;
      case 'G':
        display.drawBitmap(x+8*letter,y,italic_font[6],7,7,c);
        break;
      case 'H':
        display.drawBitmap(x+8*letter,y,italic_font[7],7,7,c);
        break;
      case 'I':
        display.drawBitmap(x+8*letter,y,italic_font[8],7,7,c);
        break;
      case 'J':
        display.drawBitmap(x+8*letter,y,italic_font[9],7,7,c);
        break;
      case 'K':
        display.drawBitmap(x+8*letter,y,italic_font[10],7,7,c);
        break;
      case 'L':
        display.drawBitmap(x+8*letter,y,italic_font[11],7,7,c);
        break;
      case 'M':
        display.drawBitmap(x+8*letter,y,italic_font[12],7,7,c);
        break;
      case 'N':
        display.drawBitmap(x+8*letter,y,italic_font[13],7,7,c);
        break;
      case 'O':
        display.drawBitmap(x+8*letter,y,italic_font[14],7,7,c);
        break;
      case 'P':
        display.drawBitmap(x+8*letter,y,italic_font[15],7,7,c);
        break;
      case 'Q':
        display.drawBitmap(x+8*letter,y,italic_font[16],7,7,c);
        break;
      case 'R':
        display.drawBitmap(x+8*letter,y,italic_font[17],7,7,c);
        break;
      case 'S':
        display.drawBitmap(x+8*letter,y,italic_font[18],7,7,c);
        break;
      case 'T':
        display.drawBitmap(x+8*letter,y,italic_font[19],7,7,c);
        break;
      case 'U':
        display.drawBitmap(x+8*letter,y,italic_font[20],7,7,c);
        break;
      case 'V':
        display.drawBitmap(x+8*letter,y,italic_font[21],7,7,c);
        break;
      case 'W':
        display.drawBitmap(x+8*letter,y,italic_font[22],7,7,c);
        break;
      case 'X':
        display.drawBitmap(x+8*letter,y,italic_font[23],7,7,c);
        break;
      case 'Y':
        display.drawBitmap(x+8*letter,y,italic_font[24],7,7,c);
        break;
      case 'Z':
        display.drawBitmap(x+8*letter,y,italic_font[25],7,7,c);
        break;
      case 'a':
        display.drawBitmap(x+8*letter,y,italic_font[0],7,7,c);
        break;
      case 'b':
        display.drawBitmap(x+8*letter,y,italic_font[1],7,7,c);
        break;
      case 'c':
        display.drawBitmap(x+8*letter,y,italic_font[2],7,7,c);
        break;
      case 'd':
        display.drawBitmap(x+8*letter,y,italic_font[3],7,7,c);
        break;
      case 'e':
        display.drawBitmap(x+8*letter,y,italic_font[4],7,7,c);
        break;
      case 'f':
        display.drawBitmap(x+8*letter,y,italic_font[5],7,7,c);
        break;
      case 'g':
        display.drawBitmap(x+8*letter,y,italic_font[6],7,7,c);
        break;
      case 'h':
        display.drawBitmap(x+8*letter,y,italic_font[7],7,7,c);
        break;
      case 'i':
        display.drawBitmap(x+8*letter,y,italic_font[8],7,7,c);
        break;
      case 'j':
        display.drawBitmap(x+8*letter,y,italic_font[9],7,7,c);
        break;
      case 'k':
        display.drawBitmap(x+8*letter,y,italic_font[10],7,7,c);
        break;
      case 'l':
        display.drawBitmap(x+8*letter,y,italic_font[11],7,7,c);
        break;
      case 'm':
        display.drawBitmap(x+8*letter,y,italic_font[12],7,7,c);
        break;
      case 'n':
        display.drawBitmap(x+8*letter,y,italic_font[13],7,7,c);
        break;
      case 'o':
        display.drawBitmap(x+8*letter,y,italic_font[14],7,7,c);
        break;
      case 'p':
        display.drawBitmap(x+8*letter,y,italic_font[15],7,7,c);
        break;
      case 'q':
        display.drawBitmap(x+8*letter,y,italic_font[16],7,7,c);
        break;
      case 'r':
        display.drawBitmap(x+8*letter,y,italic_font[17],7,7,c);
        break;
      case 's':
        display.drawBitmap(x+8*letter,y,italic_font[18],7,7,c);
        break;
      case 't':
        display.drawBitmap(x+8*letter,y,italic_font[19],7,7,c);
        break;
      case 'u':
        display.drawBitmap(x+8*letter,y,italic_font[20],7,7,c);
        break;
      case 'v':
        display.drawBitmap(x+8*letter,y,italic_font[21],7,7,c);
        break;
      case 'w':
        display.drawBitmap(x+8*letter,y,italic_font[22],7,7,c);
        break;
      case 'x':
        display.drawBitmap(x+8*letter,y,italic_font[23],7,7,c);
        break;
      case 'y':
        display.drawBitmap(x+8*letter,y,italic_font[24],7,7,c);
        break;
      case 'z':
        display.drawBitmap(x+8*letter,y,italic_font[25],7,7,c);
        break;
    }
  }
}
