void printArp_wiggly(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    int8_t offset = 3*sin(float(millis())/float(200)+float(letter));
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        display.drawBitmap(x+6*letter,y+offset,arpFont[0],5,7,c);
        break;
      case 'B':
        display.drawBitmap(x+6*letter,y+offset,arpFont[1],5,7,c);
        break;
      case 'C':
        display.drawBitmap(x+6*letter,y+offset,arpFont[2],5,7,c);
        break;
      case 'D':
        display.drawBitmap(x+6*letter,y+offset,arpFont[3],5,7,c);
        break;
      case 'E':
        display.drawBitmap(x+6*letter,y+offset,arpFont[4],5,7,c);
        break;
      case 'F':
        display.drawBitmap(x+6*letter,y+offset,arpFont[5],5,7,c);
        break;
      case 'G':
        display.drawBitmap(x+6*letter,y+offset,arpFont[6],5,7,c);
        break;
      case 'H':
        display.drawBitmap(x+6*letter,y+offset,arpFont[7],5,7,c);
        break;
      case 'I':
        display.drawBitmap(x+6*letter,y+offset,arpFont[8],5,7,c);
        break;
      case 'J':
        display.drawBitmap(x+6*letter,y+offset,arpFont[9],5,7,c);
        break;
      case 'K':
        display.drawBitmap(x+6*letter,y+offset,arpFont[10],5,7,c);
        break;
      case 'L':
        display.drawBitmap(x+6*letter,y+offset,arpFont[11],5,7,c);
        break;
      case 'M':
        display.drawBitmap(x+6*letter,y+offset,arpFont[12],6,7,c);
        x++;
        break;
      case 'N':
        display.drawBitmap(x+6*letter,y+offset,arpFont[13],6,7,c);
        break;
      case 'O':
        display.drawBitmap(x+6*letter,y+offset,arpFont[14],6,7,c);
        break;
      case 'P':
        display.drawBitmap(x+6*letter,y+offset,arpFont[15],6,7,c);
        break;
      case 'Q':
        display.drawBitmap(x+6*letter,y+offset,arpFont[16],6,7,c);
        break;
      case 'R':
        display.drawBitmap(x+6*letter,y+offset,arpFont[17],6,7,c);
        break;
      case 'S':
        display.drawBitmap(x+6*letter,y+offset,arpFont[18],6,7,c);
        break;
      case 'T':
        display.drawBitmap(x+6*letter,y+offset,arpFont[19],6,7,c);
        break;
      case 'U':
        display.drawBitmap(x+6*letter,y+offset,arpFont[20],6,7,c);
        break;
      case 'V':
        display.drawBitmap(x+6*letter,y+offset,arpFont[21],6,7,c);
        break;
      case 'W':
        display.drawBitmap(x+6*letter,y+offset,arpFont[22],6,7,c);
        x++;
        break;
      case 'X':
        display.drawBitmap(x+6*letter,y+offset,arpFont[23],6,7,c);
        break;
      case 'Y':
        display.drawBitmap(x+6*letter,y+offset,arpFont[24],6,7,c);
        break;
      case 'Z':
        display.drawBitmap(x+6*letter,y+offset,arpFont[25],6,7,c);
        break;
      case 'a':
        display.drawBitmap(x+6*letter,y+offset,arpFont[0],6,7,c);
        break;
      case 'b':
        display.drawBitmap(x+6*letter,y+offset,arpFont[1],6,7,c);
        break;
      case 'c':
        display.drawBitmap(x+6*letter,y+offset,arpFont[2],6,7,c);
        break;
      case 'd':
        display.drawBitmap(x+6*letter,y+offset,arpFont[3],6,7,c);
        break;
      case 'e':
        display.drawBitmap(x+6*letter,y+offset,arpFont[4],6,7,c);
        break;
      case 'f':
        display.drawBitmap(x+6*letter,y+offset,arpFont[5],6,7,c);
        break;
      case 'g':
        display.drawBitmap(x+6*letter,y+offset,arpFont[6],6,7,c);
        break;
      case 'h':
        display.drawBitmap(x+6*letter,y+offset,arpFont[7],6,7,c);
        break;
      case 'i':
        display.drawBitmap(x+6*letter,y+offset,arpFont[8],6,7,c);
        break;
      case 'j':
        display.drawBitmap(x+6*letter,y+offset,arpFont[9],6,7,c);
        break;
      case 'k':
        display.drawBitmap(x+6*letter,y+offset,arpFont[10],6,7,c);
        break;
      case 'l':
        display.drawBitmap(x+6*letter,y+offset,arpFont[11],6,7,c);
        break;
      case 'm':
        display.drawBitmap(x+6*letter,y+offset,arpFont[12],6,7,c);
        x++;
        break;
      case 'n':
        display.drawBitmap(x+6*letter,y+offset,arpFont[13],6,7,c);
        break;
      case 'o':
        display.drawBitmap(x+6*letter,y+offset,arpFont[14],6,7,c);
        break;
      case 'p':
        display.drawBitmap(x+6*letter,y+offset,arpFont[15],6,7,c);
        break;
      case 'q':
        display.drawBitmap(x+6*letter,y+offset,arpFont[16],6,7,c);
        break;
      case 'r':
        display.drawBitmap(x+6*letter,y+offset,arpFont[17],6,7,c);
        break;
      case 's':
        display.drawBitmap(x+6*letter,y+offset,arpFont[18],6,7,c);
        break;
      case 't':
        display.drawBitmap(x+6*letter,y+offset,arpFont[19],6,7,c);
        break;
      case 'u':
        display.drawBitmap(x+6*letter,y+offset,arpFont[20],6,7,c);
        break;
      case 'v':
        display.drawBitmap(x+6*letter,y+offset,arpFont[21],6,7,c);
        break;
      case 'w':
        display.drawBitmap(x+6*letter,y+offset,arpFont[22],6,7,c);
        x++;
        break;
      case 'x':
        display.drawBitmap(x+6*letter,y+offset,arpFont[23],6,7,c);
        break;
      case 'y':
        display.drawBitmap(x+6*letter,y+offset,arpFont[24],6,7,c);
        break;
      case 'z':
        display.drawBitmap(x+6*letter,y+offset,arpFont[25],6,7,c);
        break;
    }
  }
}
void printArp(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        display.drawBitmap(x+6*letter,y,arpFont[0],5,7,c);
        break;
      case 'B':
        display.drawBitmap(x+6*letter,y,arpFont[1],5,7,c);
        break;
      case 'C':
        display.drawBitmap(x+6*letter,y,arpFont[2],5,7,c);
        break;
      case 'D':
        display.drawBitmap(x+6*letter,y,arpFont[3],5,7,c);
        break;
      case 'E':
        display.drawBitmap(x+6*letter,y,arpFont[4],5,7,c);
        break;
      case 'F':
        display.drawBitmap(x+6*letter,y,arpFont[5],5,7,c);
        break;
      case 'G':
        display.drawBitmap(x+6*letter,y,arpFont[6],5,7,c);
        break;
      case 'H':
        display.drawBitmap(x+6*letter,y,arpFont[7],5,7,c);
        break;
      case 'I':
        display.drawBitmap(x+6*letter,y,arpFont[8],5,7,c);
        break;
      case 'J':
        display.drawBitmap(x+6*letter,y,arpFont[9],5,7,c);
        break;
      case 'K':
        display.drawBitmap(x+6*letter,y,arpFont[10],5,7,c);
        break;
      case 'L':
        display.drawBitmap(x+6*letter,y,arpFont[11],5,7,c);
        break;
      case 'M':
        display.drawBitmap(x+6*letter,y,arpFont[12],6,7,c);
        x++;
        break;
      case 'N':
        display.drawBitmap(x+6*letter,y,arpFont[13],5,7,c);
        break;
      case 'O':
        display.drawBitmap(x+6*letter,y,arpFont[14],5,7,c);
        break;
      case 'P':
        display.drawBitmap(x+6*letter,y,arpFont[15],5,7,c);
        break;
      case 'Q':
        display.drawBitmap(x+6*letter,y,arpFont[16],6,7,c);
        break;
      case 'R':
        display.drawBitmap(x+6*letter,y,arpFont[17],5,7,c);
        break;
      case 'S':
        display.drawBitmap(x+6*letter,y,arpFont[18],5,7,c);
        break;
      case 'T':
        display.drawBitmap(x+6*letter,y,arpFont[19],5,7,c);
        break;
      case 'U':
        display.drawBitmap(x+6*letter,y,arpFont[20],5,7,c);
        break;
      case 'V':
        display.drawBitmap(x+6*letter,y,arpFont[21],5,7,c);
        break;
      case 'W':
        display.drawBitmap(x+6*letter,y,arpFont[22],6,7,c);
        x++;
        break;
      case 'X':
        display.drawBitmap(x+6*letter,y,arpFont[23],5,7,c);
        break;
      case 'Y':
        display.drawBitmap(x+6*letter,y,arpFont[24],5,7,c);
        break;
      case 'Z':
        display.drawBitmap(x+6*letter,y,arpFont[25],5,7,c);
        break;
      case 'a':
        display.drawBitmap(x+6*letter,y,arpFont[0],5,7,c);
        break;
      case 'b':
        display.drawBitmap(x+6*letter,y,arpFont[1],5,7,c);
        break;
      case 'c':
        display.drawBitmap(x+6*letter,y,arpFont[2],5,7,c);
        break;
      case 'd':
        display.drawBitmap(x+6*letter,y,arpFont[3],5,7,c);
        break;
      case 'e':
        display.drawBitmap(x+6*letter,y,arpFont[4],5,7,c);
        break;
      case 'f':
        display.drawBitmap(x+6*letter,y,arpFont[5],5,7,c);
        break;
      case 'g':
        display.drawBitmap(x+6*letter,y,arpFont[6],5,7,c);
        break;
      case 'h':
        display.drawBitmap(x+6*letter,y,arpFont[7],5,7,c);
        break;
      case 'i':
        display.drawBitmap(x+6*letter,y,arpFont[8],5,7,c);
        break;
      case 'j':
        display.drawBitmap(x+6*letter,y,arpFont[9],5,7,c);
        break;
      case 'k':
        display.drawBitmap(x+6*letter,y,arpFont[10],5,7,c);
        break;
      case 'l':
        display.drawBitmap(x+6*letter,y,arpFont[11],5,7,c);
        break;
      case 'm':
        display.drawBitmap(x+6*letter,y,arpFont[12],6,7,c);
        x++;
        break;
      case 'n':
        display.drawBitmap(x+6*letter,y,arpFont[13],5,7,c);
        break;
      case 'o':
        display.drawBitmap(x+6*letter,y,arpFont[14],5,7,c);
        break;
      case 'p':
        display.drawBitmap(x+6*letter,y,arpFont[15],5,7,c);
        break;
      case 'q':
        display.drawBitmap(x+6*letter,y,arpFont[16],6,7,c);
        x++;
        break;
      case 'r':
        display.drawBitmap(x+6*letter,y,arpFont[17],5,7,c);
        break;
      case 's':
        display.drawBitmap(x+6*letter,y,arpFont[18],5,7,c);
        break;
      case 't':
        display.drawBitmap(x+6*letter,y,arpFont[19],5,7,c);
        break;
      case 'u':
        display.drawBitmap(x+6*letter,y,arpFont[20],5,7,c);
        break;
      case 'v':
        display.drawBitmap(x+6*letter,y,arpFont[21],5,7,c);
        break;
      case 'w':
        display.drawBitmap(x+6*letter,y,arpFont[22],6,7,c);
        x++;
        break;
      case 'x':
        display.drawBitmap(x+6*letter,y,arpFont[23],5,7,c);
        break;
      case 'y':
        display.drawBitmap(x+6*letter,y,arpFont[24],5,7,c);
        break;
      case 'z':
        display.drawBitmap(x+6*letter,y,arpFont[25],5,7,c);
        break;
    }
  }
}