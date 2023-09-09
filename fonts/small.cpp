//this one prints out one word at a time, with bounds
void printSmall_overflow(int x, int y, int margin, String text, uint16_t c){
  int xCoord = x;
  int yCoord = y;
  String word = "";
  int wordlength = 0;
  for(uint8_t letter = 0; letter<text.length(); letter++){
    String character = text.substring(letter,letter+1);
    if(character != " " && character != "-"){
      if(wordlength+xCoord>=screenWidth-margin){
        xCoord = x;
        yCoord += 7;
        // Serial.println(word);
        if(yCoord>screenHeight)
          return;
      }
      word+=character;
      wordlength+=4;
    }
    else{
      word+=character;
      printSmall(xCoord, yCoord, word, c);
      word = "";
      xCoord+=wordlength+4;
      wordlength = 0;
    }
  }
  //print whatever's remaining
  printSmall(xCoord, yCoord, word, c);
}
void printSmall_centered(int x, int y, String t, uint16_t c){
  x -= t.length()*2;
  printSmall(x,y,t,c);
}
void printSmall(int x, int y, char text, uint16_t c){
  switch(text){
    case ' ':
      x-=2;
      return;
    case 'A':
      display.drawBitmap(x,y,small_font[0],3,5,c);
      break;
    case 'B':
      display.drawBitmap(x,y,small_font[1],3,5,c);
      break;
    case 'C':
      display.drawBitmap(x,y,small_font[2],3,5,c);
      break;
    case 'D':
      display.drawBitmap(x,y,small_font[3],3,5,c);
      break;
    case 'E':
      display.drawBitmap(x,y,small_font[4],3,5,c);
      break;
    case 'F':
      display.drawBitmap(x,y,small_font[5],3,5,c);
      break;
    case 'G':
      display.drawBitmap(x,y,small_font[6],3,5,c);
      break;
    case 'H':
      display.drawBitmap(x,y,small_font[7],3,5,c);
      break;
    case 'I':
      display.drawBitmap(x,y,small_font[8],3,5,c);
      break;
    case 'J':
      display.drawBitmap(x,y,small_font[9],3,5,c);
      break;
    case 'K':
      display.drawBitmap(x,y,small_font[10],3,5,c);
      break;
    case 'L':
      display.drawBitmap(x,y,small_font[11],3,5,c);
      break;
    case 'M':
      display.drawBitmap(x,y,small_font[12],3,5,c);
      break;
    case 'N':
      display.drawBitmap(x,y,small_font[13],3,5,c);
      break;
    case 'O':
      display.drawBitmap(x,y,small_font[14],3,5,c);
      break;
    case 'P':
      display.drawBitmap(x,y,small_font[15],3,5,c);
      break;
    case 'Q':
      display.drawBitmap(x,y,small_font[16],3,5,c);
      break;
    case 'R':
      display.drawBitmap(x,y,small_font[17],3,5,c);
      break;
    case 'S':
      display.drawBitmap(x,y,small_font[18],3,5,c);
      break;
    case 'T':
      display.drawBitmap(x,y,small_font[19],3,5,c);
      break;
    case 'U':
      display.drawBitmap(x,y,small_font[20],3,5,c);
      break;
    case 'V':
      display.drawBitmap(x,y,small_font[21],3,5,c);
      break;
    case 'W':
      display.drawBitmap(x,y,small_font[22],3,5,c);
      break;
    case 'X':
      display.drawBitmap(x,y,small_font[23],3,5,c);
      break;
    case 'Y':
      display.drawBitmap(x,y,small_font[24],3,5,c);
      break;
    case 'Z':
      display.drawBitmap(x,y,small_font[25],3,5,c);
      break;
    case 'a':
      display.drawBitmap(x,y,small_font[0],3,5,c);
      break;
    case 'b':
      display.drawBitmap(x,y,small_font[1],3,5,c);
      break;
    case 'c':
      display.drawBitmap(x,y,small_font[2],3,5,c);
      break;
    case 'd':
      display.drawBitmap(x,y,small_font[3],3,5,c);
      break;
    case 'e':
      display.drawBitmap(x,y,small_font[4],3,5,c);
      break;
    case 'f':
      display.drawBitmap(x,y,small_font[5],3,5,c);
      break;
    case 'g':
      display.drawBitmap(x,y,small_font[6],3,5,c);
      break;
    case 'h':
      display.drawBitmap(x,y,small_font[7],3,5,c);
      break;
    case 'i':
      display.drawBitmap(x,y,small_font[8],3,5,c);
      break;
    case 'j':
      display.drawBitmap(x,y,small_font[9],3,5,c);
      break;
    case 'k':
      display.drawBitmap(x,y,small_font[10],3,5,c);
      break;
    case 'l':
      display.drawBitmap(x,y,small_font[11],3,5,c);
      break;
    case 'm':
      display.drawBitmap(x,y,small_font[12],3,5,c);
      break;
    case 'n':
      display.drawBitmap(x,y,small_font[13],3,5,c);
      break;
    case 'o':
      display.drawBitmap(x,y,small_font[14],3,5,c);
      break;
    case 'p':
      display.drawBitmap(x,y,small_font[15],3,5,c);
      break;
    case 'q':
      display.drawBitmap(x,y,small_font[16],3,5,c);
      break;
    case 'r':
      display.drawBitmap(x,y,small_font[17],3,5,c);
      break;
    case 's':
      display.drawBitmap(x,y,small_font[18],3,5,c);
      break;
    case 't':
      display.drawBitmap(x,y,small_font[19],3,5,c);
      break;
    case 'u':
      display.drawBitmap(x,y,small_font[20],3,5,c);
      break;
    case 'v':
      display.drawBitmap(x,y,small_font[21],3,5,c);
      break;
    case 'w':
      display.drawBitmap(x,y,small_font[22],3,5,c);
      break;
    case 'x':
      display.drawBitmap(x,y,small_font[23],3,5,c);
      break;
    case 'y':
      display.drawBitmap(x,y,small_font[24],3,5,c);
      break;
    case 'z':
      display.drawBitmap(x,y,small_font[25],3,5,c);
      break;
    case '0':
      display.drawBitmap(x,y,small_font[26],3,5,c);
      break;
    case '1':
      display.drawBitmap(x,y,small_font[27],3,5,c);
      break;
    case '2':
      display.drawBitmap(x,y,small_font[28],3,5,c);
      break;
    case '3':
      display.drawBitmap(x,y,small_font[29],3,5,c);
      break;
    case '4':
      display.drawBitmap(x,y,small_font[30],3,5,c);
      break;
    case '5':
      display.drawBitmap(x,y,small_font[31],3,5,c);
      break;
    case '6':
      display.drawBitmap(x,y,small_font[32],3,5,c);
      break;
    case '7':
      display.drawBitmap(x,y,small_font[33],3,5,c);
      break;
    case '8':
      display.drawBitmap(x,y,small_font[34],3,5,c);
      break;
    case '9':
      display.drawBitmap(x,y,small_font[35],3,5,c);
      break;
    case '.':
      display.drawBitmap(x,y,small_font[36],3,5,c);
      break;
    case ':':
      display.drawBitmap(x,y,small_font[37],3,5,c);
      break;
    case ';':
      display.drawBitmap(x,y,small_font[38],3,5,c);
      break;
    case '+':
      display.drawBitmap(x,y,small_font[39],3,5,c);
      break;
    case '[':
      display.drawBitmap(x,y,small_font[40],3,5,c);
      break;
    case ']':
      display.drawBitmap(x,y,small_font[41],3,5,c);
      break;
    case '/':
      display.drawBitmap(x,y,small_font[42],3,5,c);
      break;
    case ',':
      display.drawBitmap(x,y,small_font[44],3,5,c);
      break;
    case '?':
      display.drawBitmap(x,y,small_font[45],3,5,c);
      break;
    case '!':
      display.drawBitmap(x,y,small_font[46],3,5,c);
      break;
    case '^':
      display.drawBitmap(x,y,small_font[47],3,5,c);
      break;
    case '&':
      display.drawBitmap(x,y,small_font[48],3,5,c);
      break;
    case '<':
      display.drawBitmap(x,y,small_font[49],3,5,c);
      break;
    case '>':
      display.drawBitmap(x,y,small_font[50],3,5,c);
      break;
    case '\'':
      display.drawBitmap(x,y,small_font[51],3,5,c);
      break;
    case '%':
      display.drawBitmap(x,y,small_font[52],3,5,c);
      break;
    //special case: 2 char symbol so it needs to increment x a bit
    case '#':
      display.drawBitmap(x,y,small_font[53],2,5,c);
      display.drawBitmap(x+2,y,small_font[54],3,5,c);
      x+=2;
      break;
    case '-':
      display.drawBitmap(x,y,small_font[55],3,5,c);
      break;
    case '\\':
      y+=7;
      break;
    //phi
    case '@':
      display.drawBitmap(x,y,small_font[56],3,5,c);
      break;
    case '(':
      display.drawBitmap(x,y,small_font[57],3,5,c);
      break;
    case ')':
      display.drawBitmap(x,y,small_font[58],3,5,c);
      break;
    //note
    case '$':
      display.drawBitmap(x,y,small_font[59],3,5,c);
      break;
    case '=':
      display.drawBitmap(x,y,small_font[60],3,5,c);
      break;
    case '*':
      display.drawBitmap(x,y,small_font[61],3,5,c);
      break;
    case '|':
      display.drawBitmap(x,y,small_font[62],3,5,c);
      break;
    // case '~':
    //   display.drawBitmap(x,y-2,small_font[63],7,7,c);
    //   x+=4;
    //   break;
    //double quotes
    case '\"':
      display.drawBitmap(x,y,small_font[64],3,5,c);
      break;
    //divide
    case '~':
      display.drawBitmap(x,y,small_font[65],3,5,c);
      break;
  }
}
void printSmall(int x, int y, String text, uint16_t c){
  for(int letter = 0; letter<text.length(); letter++){
    unsigned char character = text.charAt(letter);
    switch(character){
      case ' ':
        x-=2;
        continue;
      case 'A':
        display.drawBitmap(x+4*letter,y,small_font[0],3,5,c);
        break;
      case 'B':
        display.drawBitmap(x+4*letter,y,small_font[1],3,5,c);
        break;
      case 'C':
        display.drawBitmap(x+4*letter,y,small_font[2],3,5,c);
        break;
      case 'D':
        display.drawBitmap(x+4*letter,y,small_font[3],3,5,c);
        break;
      case 'E':
        display.drawBitmap(x+4*letter,y,small_font[4],3,5,c);
        break;
      case 'F':
        display.drawBitmap(x+4*letter,y,small_font[5],3,5,c);
        break;
      case 'G':
        display.drawBitmap(x+4*letter,y,small_font[6],3,5,c);
        break;
      case 'H':
        display.drawBitmap(x+4*letter,y,small_font[7],3,5,c);
        break;
      case 'I':
        display.drawBitmap(x+4*letter,y,small_font[8],3,5,c);
        break;
      case 'J':
        display.drawBitmap(x+4*letter,y,small_font[9],3,5,c);
        break;
      case 'K':
        display.drawBitmap(x+4*letter,y,small_font[10],3,5,c);
        break;
      case 'L':
        display.drawBitmap(x+4*letter,y,small_font[11],3,5,c);
        break;
      case 'M':
        display.drawBitmap(x+4*letter,y,small_font[12],3,5,c);
        break;
      case 'N':
        display.drawBitmap(x+4*letter,y,small_font[13],3,5,c);
        break;
      case 'O':
        display.drawBitmap(x+4*letter,y,small_font[14],3,5,c);
        break;
      case 'P':
        display.drawBitmap(x+4*letter,y,small_font[15],3,5,c);
        break;
      case 'Q':
        display.drawBitmap(x+4*letter,y,small_font[16],3,5,c);
        break;
      case 'R':
        display.drawBitmap(x+4*letter,y,small_font[17],3,5,c);
        break;
      case 'S':
        display.drawBitmap(x+4*letter,y,small_font[18],3,5,c);
        break;
      case 'T':
        display.drawBitmap(x+4*letter,y,small_font[19],3,5,c);
        break;
      case 'U':
        display.drawBitmap(x+4*letter,y,small_font[20],3,5,c);
        break;
      case 'V':
        display.drawBitmap(x+4*letter,y,small_font[21],3,5,c);
        break;
      case 'W':
        display.drawBitmap(x+4*letter,y,small_font[22],3,5,c);
        break;
      case 'X':
        display.drawBitmap(x+4*letter,y,small_font[23],3,5,c);
        break;
      case 'Y':
        display.drawBitmap(x+4*letter,y,small_font[24],3,5,c);
        break;
      case 'Z':
        display.drawBitmap(x+4*letter,y,small_font[25],3,5,c);
        break;
      case 'a':
        display.drawBitmap(x+4*letter,y,small_font[0],3,5,c);
        break;
      case 'b':
        display.drawBitmap(x+4*letter,y,small_font[1],3,5,c);
        break;
      case 'c':
        display.drawBitmap(x+4*letter,y,small_font[2],3,5,c);
        break;
      case 'd':
        display.drawBitmap(x+4*letter,y,small_font[3],3,5,c);
        break;
      case 'e':
        display.drawBitmap(x+4*letter,y,small_font[4],3,5,c);
        break;
      case 'f':
        display.drawBitmap(x+4*letter,y,small_font[5],3,5,c);
        break;
      case 'g':
        display.drawBitmap(x+4*letter,y,small_font[6],3,5,c);
        break;
      case 'h':
        display.drawBitmap(x+4*letter,y,small_font[7],3,5,c);
        break;
      case 'i':
        display.drawBitmap(x+4*letter,y,small_font[8],3,5,c);
        break;
      case 'j':
        display.drawBitmap(x+4*letter,y,small_font[9],3,5,c);
        break;
      case 'k':
        display.drawBitmap(x+4*letter,y,small_font[10],3,5,c);
        break;
      case 'l':
        display.drawBitmap(x+4*letter,y,small_font[11],3,5,c);
        break;
      case 'm':
        display.drawBitmap(x+4*letter,y,small_font[12],3,5,c);
        break;
      case 'n':
        display.drawBitmap(x+4*letter,y,small_font[13],3,5,c);
        break;
      case 'o':
        display.drawBitmap(x+4*letter,y,small_font[14],3,5,c);
        break;
      case 'p':
        display.drawBitmap(x+4*letter,y,small_font[15],3,5,c);
        break;
      case 'q':
        display.drawBitmap(x+4*letter,y,small_font[16],3,5,c);
        break;
      case 'r':
        display.drawBitmap(x+4*letter,y,small_font[17],3,5,c);
        break;
      case 's':
        display.drawBitmap(x+4*letter,y,small_font[18],3,5,c);
        break;
      case 't':
        display.drawBitmap(x+4*letter,y,small_font[19],3,5,c);
        break;
      case 'u':
        display.drawBitmap(x+4*letter,y,small_font[20],3,5,c);
        break;
      case 'v':
        display.drawBitmap(x+4*letter,y,small_font[21],3,5,c);
        break;
      case 'w':
        display.drawBitmap(x+4*letter,y,small_font[22],3,5,c);
        break;
      case 'x':
        display.drawBitmap(x+4*letter,y,small_font[23],3,5,c);
        break;
      case 'y':
        display.drawBitmap(x+4*letter,y,small_font[24],3,5,c);
        break;
      case 'z':
        display.drawBitmap(x+4*letter,y,small_font[25],3,5,c);
        break;
      case '0':
        display.drawBitmap(x+4*letter,y,small_font[26],3,5,c);
        break;
      case '1':
        display.drawBitmap(x+4*letter,y,small_font[27],3,5,c);
        break;
      case '2':
        display.drawBitmap(x+4*letter,y,small_font[28],3,5,c);
        break;
      case '3':
        display.drawBitmap(x+4*letter,y,small_font[29],3,5,c);
        break;
      case '4':
        display.drawBitmap(x+4*letter,y,small_font[30],3,5,c);
        break;
      case '5':
        display.drawBitmap(x+4*letter,y,small_font[31],3,5,c);
        break;
      case '6':
        display.drawBitmap(x+4*letter,y,small_font[32],3,5,c);
        break;
      case '7':
        display.drawBitmap(x+4*letter,y,small_font[33],3,5,c);
        break;
      case '8':
        display.drawBitmap(x+4*letter,y,small_font[34],3,5,c);
        break;
      case '9':
        display.drawBitmap(x+4*letter,y,small_font[35],3,5,c);
        break;
      case '.':
        display.drawBitmap(x+4*letter,y,small_font[36],3,5,c);
        break;
      case ':':
        display.drawBitmap(x+4*letter-1,y,small_font[37],3,5,c);
        x-=2;
        break;
      case ';':
        display.drawBitmap(x+4*letter,y,small_font[38],3,5,c);
        break;
      case '+':
        display.drawBitmap(x+4*letter,y,small_font[39],3,5,c);
        break;
      case '[':
        display.drawBitmap(x+4*letter,y,small_font[40],3,5,c);
        break;
      case ']':
        display.drawBitmap(x+4*letter,y,small_font[41],3,5,c);
        break;
      case '/':
        display.drawBitmap(x+4*letter,y,small_font[42],3,5,c);
        break;
      case ',':
        display.drawBitmap(x+4*letter,y,small_font[44],3,5,c);
        break;
      case '?':
        display.drawBitmap(x+4*letter,y,small_font[45],3,5,c);
        break;
      case '!':
        display.drawBitmap(x+4*letter,y,small_font[46],3,5,c);
        break;
      case '^':
        display.drawBitmap(x+4*letter,y,small_font[47],3,5,c);
        break;
      case '&':
        display.drawBitmap(x+4*letter,y,small_font[48],3,5,c);
        break;
      case '<':
        display.drawBitmap(x+4*letter,y,small_font[49],3,5,c);
        break;
      case '>':
        display.drawBitmap(x+4*letter,y,small_font[50],3,5,c);
        break;
      //single quote
      case '\'':
        display.drawBitmap(x+4*letter,y,small_font[51],3,5,c);
        break;
      case '%':
        display.drawBitmap(x+4*letter,y,small_font[52],3,5,c);
        break;
      //special case: 2 char symbol so it needs to increment x a bit
      case '#':
        display.drawBitmap(x+4*letter,y,small_font[53],2,5,c);
        display.drawBitmap(x+2+4*letter,y,small_font[54],3,5,c);
        x+=2;
        break;
      case '-':
        display.drawBitmap(x+4*letter,y,small_font[55],3,5,c);
        break;
      //newline
      case '\\':
        y+=7;
        break;
      //phi
      case '@':
        display.drawBitmap(x+4*letter,y,small_font[56],3,5,c);
        break;
      case '(':
        display.drawBitmap(x+4*letter,y,small_font[57],3,5,c);
        break;
      case ')':
        display.drawBitmap(x+4*letter,y,small_font[58],3,5,c);
        break;
      //note icon
      case '$':
        display.drawBitmap(x+4*letter,y,small_font[59],3,5,c);
        break;
      case '=':
        display.drawBitmap(x+4*letter,y,small_font[60],3,5,c);
        break;
      case '*':
        display.drawBitmap(x+4*letter,y,small_font[61],3,5,c);
        break;
      case '|':
        display.drawBitmap(x+4*letter,y,small_font[62],3,5,c);
        break;
      //delta
      // case '~':
      //   display.drawBitmap(x,y-2,small_font[63],7,7,c);
      //   x+=4;
        // break;
      //double quotes
      case '\"':
        display.drawBitmap(x+4*letter,y,small_font[64],3,5,c);
        break;
      //divide
      case '~':
        display.drawBitmap(x+4*letter,y,small_font[65],3,5,c);
        break;
    }
  }
}