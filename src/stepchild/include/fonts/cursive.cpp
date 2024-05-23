//Cursive font
void printCursive(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        continue;
      case 'a':
        display.drawBitmap(x+6*letter,y,cursive_font[0],6,6,c);
        break;
      case 'b':
        display.drawBitmap(x+6*letter,y,cursive_font[1],6,8,c);
        break;
      case 'c':
        display.drawBitmap(x+6*letter,y,cursive_font[2],6,6,c);
        break;
      case 'd':
        display.drawBitmap(x+6*letter,y,cursive_font[3],6,6,c);
        break;
      case 'e':
        display.drawBitmap(x+6*letter,y,cursive_font[4],6,6,c);
        break;
      case 'f':
        // display.drawBitmap(x+6*letter,y,cursive_font[5],6,6,c);
        display.drawBitmap(x+6*letter,y,cursive_font[5],6,12,c);
        break;
      case 'g':
        display.drawBitmap(x+6*letter,y,cursive_font[6],6,12,c);
        break;
      case 'h':
        display.drawBitmap(x+6*letter,y,cursive_font[7],6,6,c);
        break;
      case 'i':
        display.drawBitmap(x+6*letter,y,cursive_font[8],6,6,c);
        break;
      case 'j':
        display.drawBitmap(x+6*letter,y,cursive_font[9],6,12,c);
        break;
      case 'k':
        display.drawBitmap(x+6*letter,y,cursive_font[10],6,6,c);
        break;
      case 'l':
        display.drawBitmap(x+6*letter,y,cursive_font[11],6,6,c);
        break;
      case 'm':
        display.drawBitmap(x+6*letter,y,cursive_font[12],6,6,c);
        break;
      case 'n':
        display.drawBitmap(x+6*letter,y,cursive_font[13],6,6,c);
        break;
      case 'o':
        display.drawBitmap(x+6*letter,y,cursive_font[14],6,6,c);
        break;
      case 'p':
        display.drawBitmap(x+6*letter,y,cursive_font[15],6,12,c);
        break;
      case 'q':
        display.drawBitmap(x+6*letter,y,cursive_font[16],6,6,c);
        break;
      case 'r':
        display.drawBitmap(x+6*letter,y,cursive_font[17],6,6,c);
        break;
      case 's':
        display.drawBitmap(x+6*letter,y,cursive_font[18],6,6,c);
        break;
      case 't':
        display.drawBitmap(x+6*letter,y,cursive_font[19],6,6,c);
        break;
      case 'u':
        display.drawBitmap(x+6*letter,y,cursive_font[20],6,6,c);
        break;
      case 'v':
        display.drawBitmap(x+6*letter,y,cursive_font[21],6,6,c);
        break;
      case 'w':
        display.drawBitmap(x+6*letter,y,cursive_font[22],6,6,c);
        break;
      case 'x':
        display.drawBitmap(x+6*letter,y,cursive_font[23],6,6,c);
        break;
      case 'y':
        display.drawBitmap(x+6*letter,y,cursive_font[24],6,12,c);
        break;
      case 'z':
        display.drawBitmap(x+6*letter,y,cursive_font[25],6,6,c);
        break;
      //uppercase
      case 'A':
        display.drawBitmap(x+6*letter,y,cursive_font[0],6,6,c);
        break;
      case 'B':
        display.drawBitmap(x+6*letter,y,cursive_font[1],6,8,c);
        break;
      case 'C':
        display.drawBitmap(x+6*letter,y,cursive_font[2],6,6,c);
        break;
      case 'D':
        display.drawBitmap(x+6*letter,y,cursive_font[3],6,6,c);
        break;
      case 'E':
        display.drawBitmap(x+6*letter,y,cursive_font[4],6,6,c);
        break;
      case 'F':
        // display.drawBitmap(x+6*letter,y,cursive_font[5],6,6,c);
        display.drawBitmap(x+6*letter,y,cursive_font[5],6,12,c);
        break;
      case 'G':
        display.drawBitmap(x+6*letter,y,cursive_font[6],6,12,c);
        break;
      case 'H':
        display.drawBitmap(x+6*letter,y,cursive_font[7],6,6,c);
        break;
      case 'I':
        display.drawBitmap(x+6*letter,y,cursive_font[8],6,6,c);
        break;
      case 'J':
        display.drawBitmap(x+6*letter,y,cursive_font[9],6,12,c);
        break;
      case 'K':
        display.drawBitmap(x+6*letter,y,cursive_font[10],6,6,c);
        break;
      case 'L':
        display.drawBitmap(x+6*letter,y,cursive_font[11],6,6,c);
        break;
      case 'M':
        display.drawBitmap(x+6*letter,y,cursive_font[12],6,6,c);
        break;
      case 'N':
        display.drawBitmap(x+6*letter,y,cursive_font[13],6,6,c);
        break;
      case 'O':
        display.drawBitmap(x+6*letter,y,cursive_font[14],6,6,c);
        break;
      case 'P':
        display.drawBitmap(x+6*letter,y,cursive_font[15],6,12,c);
        break;
      case 'Q':
        display.drawBitmap(x+6*letter,y,cursive_font[16],6,6,c);
        break;
      case 'R':
        display.drawBitmap(x+6*letter,y,cursive_font[17],6,6,c);
        break;
      case 'S':
        display.drawBitmap(x+6*letter,y,cursive_font[18],6,6,c);
        break;
      case 'T':
        display.drawBitmap(x+6*letter,y,cursive_font[19],6,6,c);
        break;
      case 'U':
        display.drawBitmap(x+6*letter,y,cursive_font[20],6,6,c);
        break;
      case 'V':
        display.drawBitmap(x+6*letter,y,cursive_font[21],6,6,c);
        break;
      case 'W':
        display.drawBitmap(x+6*letter,y,cursive_font[22],6,6,c);
        break;
      case 'X':
        display.drawBitmap(x+6*letter,y,cursive_font[23],6,6,c);
        break;
      case 'Y':
        display.drawBitmap(x+6*letter,y,cursive_font[24],6,12,c);
        break;
      case 'Z':
        display.drawBitmap(x+6*letter,y,cursive_font[25],6,6,c);
        break;
      //numbers
      case '0':
        display.drawBitmap(x+6*letter,y,cursive_font[26],6,6,c);
        break;
      case '1':
        display.drawBitmap(x+6*letter,y,cursive_font[27],6,6,c);
        break;
      case '2':
        display.drawBitmap(x+6*letter,y,cursive_font[28],6,6,c);
        break;
      case '3':
        display.drawBitmap(x+6*letter,y,cursive_font[29],6,6,c);
        break;
      case '4':
        display.drawBitmap(x+6*letter,y,cursive_font[30],6,6,c);
        break;
      case '5':
        display.drawBitmap(x+6*letter,y,cursive_font[31],6,6,c);
        break;
      case '6':
        display.drawBitmap(x+6*letter,y,cursive_font[32],6,6,c);
        break;
      case '7':
        display.drawBitmap(x+6*letter,y,cursive_font[33],6,6,c);
        break;
      case '8':
        display.drawBitmap(x+6*letter,y,cursive_font[34],6,6,c);
        break;
      case '9':
        display.drawBitmap(x+6*letter,y,cursive_font[35],6,6,c);
        break;
      case '.':
        display.drawBitmap(x+6*letter,y,cursive_font[36],6,6,c);
        x-=3;
        break;
      case '-':
        display.drawBitmap(x+6*letter,y,cursive_font[37],6,6,c);
        break;
      case '+':
        display.drawBitmap(x+6*letter,y,cursive_font[38],6,6,c);
        break;
    }
  }
}

void printCursive_centered(int x, int y, String t, uint16_t c){
  printCursive(x-t.length()*3,y,t,c);
}

