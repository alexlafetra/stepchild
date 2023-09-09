
void printChunky(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        continue;
      case 'A':
        display.drawBitmap(x+6*letter,y,chunkyFont[0],5,6,c);
        break;
      case 'B':
        display.drawBitmap(x+6*letter,y,chunkyFont[1],5,6,c);
        break;
      case 'C':
        display.drawBitmap(x+6*letter,y,chunkyFont[2],5,6,c);
        break;
      case 'D':
        display.drawBitmap(x+6*letter,y,chunkyFont[3],5,6,c);
        break;
      case 'E':
        display.drawBitmap(x+6*letter,y,chunkyFont[4],5,6,c);
        break;
      case 'F':
        display.drawBitmap(x+6*letter,y,chunkyFont[5],5,6,c);
        break;
      case 'G':
        display.drawBitmap(x+6*letter,y,chunkyFont[6],5,6,c);
        break;
      case 'H':
        display.drawBitmap(x+6*letter,y,chunkyFont[7],5,6,c);
        break;
      case 'I':
        display.drawBitmap(x+6*letter,y,chunkyFont[8],5,6,c);
        break;
      case 'J':
        display.drawBitmap(x+6*letter,y,chunkyFont[9],5,6,c);
        break;
      case 'K':
        display.drawBitmap(x+6*letter,y,chunkyFont[10],5,6,c);
        break;
      case 'L':
        display.drawBitmap(x+6*letter,y,chunkyFont[11],5,6,c);
        break;
      case 'M':
        display.drawBitmap(x+6*letter,y,chunkyFont[12],5,6,c);
        break;
      case 'N':
        display.drawBitmap(x+6*letter,y,chunkyFont[13],5,6,c);
        break;
      case 'O':
        display.drawBitmap(x+6*letter,y,chunkyFont[14],5,6,c);
        break;
      case 'P':
        display.drawBitmap(x+6*letter,y,chunkyFont[15],5,6,c);
        break;
      case 'Q':
        display.drawBitmap(x+6*letter,y,chunkyFont[16],5,6,c);
        break;
      case 'R':
        display.drawBitmap(x+6*letter,y,chunkyFont[17],5,6,c);
        break;
      case 'S':
        display.drawBitmap(x+6*letter,y,chunkyFont[18],5,6,c);
        break;
      case 'T':
        display.drawBitmap(x+6*letter,y,chunkyFont[19],5,6,c);
        break;
      case 'U':
        display.drawBitmap(x+6*letter,y,chunkyFont[20],5,6,c);
        break;
      case 'V':
        display.drawBitmap(x+6*letter,y,chunkyFont[21],5,6,c);
        break;
      case 'W':
        display.drawBitmap(x+6*letter,y,chunkyFont[22],5,6,c);
        break;
      case 'X':
        display.drawBitmap(x+6*letter,y,chunkyFont[23],5,6,c);
        break;
      case 'Y':
        display.drawBitmap(x+6*letter,y,chunkyFont[24],5,6,c);
        break;
      case 'Z':
        display.drawBitmap(x+6*letter,y,chunkyFont[25],5,6,c);
        break;
      case 'a':
        display.drawBitmap(x+6*letter,y,chunkyFont[0],5,6,c);
        break;
      case 'b':
        display.drawBitmap(x+6*letter,y,chunkyFont[1],5,6,c);
        break;
      case 'c':
        display.drawBitmap(x+6*letter,y,chunkyFont[2],5,6,c);
        break;
      case 'd':
        display.drawBitmap(x+6*letter,y,chunkyFont[3],5,6,c);
        break;
      case 'e':
        display.drawBitmap(x+6*letter,y,chunkyFont[4],5,6,c);
        break;
      case 'f':
        display.drawBitmap(x+6*letter,y,chunkyFont[5],5,6,c);
        break;
      case 'g':
        display.drawBitmap(x+6*letter,y,chunkyFont[6],5,6,c);
        break;
      case 'h':
        display.drawBitmap(x+6*letter,y,chunkyFont[7],5,6,c);
        break;
      case 'i':
        display.drawBitmap(x+6*letter,y,chunkyFont[8],5,6,c);
        break;
      case 'j':
        display.drawBitmap(x+6*letter,y,chunkyFont[9],5,6,c);
        break;
      case 'k':
        display.drawBitmap(x+6*letter,y,chunkyFont[10],5,6,c);
        break;
      case 'l':
        display.drawBitmap(x+6*letter,y,chunkyFont[11],5,6,c);
        break;
      case 'm':
        display.drawBitmap(x+6*letter,y,chunkyFont[12],5,6,c);
        break;
      case 'n':
        display.drawBitmap(x+6*letter,y,chunkyFont[13],5,6,c);
        break;
      case 'o':
        display.drawBitmap(x+6*letter,y,chunkyFont[14],5,6,c);
        break;
      case 'p':
        display.drawBitmap(x+6*letter,y,chunkyFont[15],5,6,c);
        break;
      case 'q':
        display.drawBitmap(x+6*letter,y,chunkyFont[16],5,6,c);
        break;
      case 'r':
        display.drawBitmap(x+6*letter,y,chunkyFont[17],5,6,c);
        break;
      case 's':
        display.drawBitmap(x+6*letter,y,chunkyFont[18],5,6,c);
        break;
      case 't':
        display.drawBitmap(x+6*letter,y,chunkyFont[19],5,6,c);
        break;
      case 'u':
        display.drawBitmap(x+6*letter,y,chunkyFont[20],5,6,c);
        break;
      case 'v':
        display.drawBitmap(x+6*letter,y,chunkyFont[21],5,6,c);
        break;
      case 'w':
        display.drawBitmap(x+6*letter,y,chunkyFont[22],5,6,c);
        break;
      case 'x':
        display.drawBitmap(x+6*letter,y,chunkyFont[23],5,6,c);
        break;
      case 'y':
        display.drawBitmap(x+6*letter,y,chunkyFont[24],5,6,c);
        break;
      case 'z':
        display.drawBitmap(x+6*letter,y,chunkyFont[25],5,6,c);
        break;
      //same as 'o'
      case '0':
        display.drawBitmap(x+6*letter,y,chunkyFont[14],5,6,c);
        break;
      case '1':
        display.drawBitmap(x+6*letter,y,chunkyFont[26],5,6,c);
        break;
      case '2':
        display.drawBitmap(x+6*letter,y,chunkyFont[27],5,6,c);
        break;
      case '3':
        display.drawBitmap(x+6*letter,y,chunkyFont[28],5,6,c);
        break;
      case '4':
        display.drawBitmap(x+6*letter,y,chunkyFont[29],5,6,c);
        break;
      case '5':
        display.drawBitmap(x+6*letter,y,chunkyFont[30],5,6,c);
        break;
      case '6':
        display.drawBitmap(x+6*letter,y,chunkyFont[31],5,6,c);
        break;
      case '7':
        display.drawBitmap(x+6*letter,y,chunkyFont[32],5,6,c);
        break;
      case '8':
        display.drawBitmap(x+6*letter,y,chunkyFont[33],5,6,c);
        break;
      case '9':
        display.drawBitmap(x+6*letter,y,chunkyFont[34],5,6,c);
        break;
      case '.':
        display.drawBitmap(x+6*letter,y,chunkyFont[35],5,6,c);
        break;
      case ':':
        display.drawBitmap(x+4*letter-1,y,small_font[36],5,6,c);
        break;
      case '[':
        display.drawBitmap(x+6*letter,y,chunkyFont[37],5,6,c);
        break;
      case ']':
        display.drawBitmap(x+6*letter,y,chunkyFont[38],5,6,c);
        break;
      case ',':
        display.drawBitmap(x+6*letter,y,chunkyFont[39],5,6,c);
        break;
      case '?':
        display.drawBitmap(x+6*letter,y,chunkyFont[40],5,6,c);
        break;
      case '!':
        display.drawBitmap(x+6*letter,y,chunkyFont[41],5,6,c);
        break;
      case '%':
        display.drawBitmap(x+6*letter,y,chunkyFont[42],5,6,c);
        break;
      case '\\':
        y+=7;
        break;
    }
  }
}