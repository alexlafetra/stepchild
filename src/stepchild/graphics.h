//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      if(x != 0 || y != 0){
        if(x == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(x == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(sel){
        selected = true;
        time = millis();
      }
    }
  }
  sel = false;
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}

void shadeArea(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade){
  for(int j = 0; j<height; j++){
    for(int i = 0;(i+j%shade)<len; i+=shade){
      // if((i+j%shade)>=0 && (i+j%shade)<screenWidth && y1+j>=0 && y1+j<screenHeight){
      if((i+j%shade)>=0 && (i+j%shade)<screenWidth){
        display.drawPixel(x1+i+j%shade,y1+j,1);
      }
      else{
        display.drawPixel(x1+i+j%shade,y1+j,0);
      }
    }
  }
}
void drawDottedLineH(unsigned short int x1, unsigned short int x2, unsigned short int y1, unsigned short int dot){
  if(x1<x2){
    for(int i = x1; i<=x2; i+=dot){
      display.drawPixel(i,y1,SSD1306_WHITE);
    }
  }
  else if(x2<x1){
    for(int i = x2; i<=x1; i+=dot){
      display.drawPixel(i,y1,SSD1306_WHITE);
    }
  }
}

void drawDottedLineV(unsigned short int x1, unsigned short int y1, unsigned short int y2, unsigned short int dot){
  if(y1<y2){
    for(int i = y1; i<=y2; i+=dot){
      display.drawPixel(x1,i,SSD1306_WHITE);
    }
  }
  else if(y2>y1){
    for(int i = y2; i<=y1; i+=dot){
      display.drawPixel(x1,i,SSD1306_WHITE);
    }
  }
}

//this one draws a black pixel every dot pixels
void drawDottedLineV2(unsigned short int x1, unsigned short int y1, unsigned short int y2, unsigned short int dot){
  if(y1<y2){
    for(int i = y1; i<=y2; i++){
      if((i%dot))
        display.drawPixel(x1,i,SSD1306_WHITE);
    }
  }
  else if(y2>y1){
    for(int i = y2; i<=y1; i++){
      if((i%dot))
        display.drawPixel(x1,i,SSD1306_WHITE);
    }
  }
}

void drawDottedLineDiagonal(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dot){
  uint8_t count;
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    uint8_t x2 = x0;
    x0 = y0;
    y0 = x2;
    x2 = x1;
    x1 = y1;
    y1 = x2;
  }

  if (x0 > x1) {
    uint8_t x2 = x0;
    x0 = x1;
    x1 = x2;
    x2 = y0;
    y0 = y1;
    y1 = x2;
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if(!(count%dot)){
      if (steep) {
        display.drawPixel(y0, x0, 2);
      }
      else {
        display.drawPixel(x0, y0, 2);
      }
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
    count++;
  }
}

void shadeRect(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade){
  display.drawRect(x1, y1, len, height, SSD1306_WHITE);
  shadeArea(x1,y1,len,height,shade);
}
void drawCenteredBracket(int x1, int y1, int length, int height){
  drawNoteBracket(x1-length/2,y1-height/2,length, height, false);
}


void drawBinarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, bool state){
  const uint8_t height = 11;
  //getting the longest string so we know how long to make the box
  uint8_t maxLength;
  if(op1.length()>op2.length())
    maxLength = op1.length();
  else
    maxLength = op2.length();
  uint8_t length = (maxLength*4+3)*2;

  display.fillRect(x1-length/2,y1-height/2,length,height,SSD1306_BLACK);
  drawSlider(x1-length/2,y1-height/2,length,height,state);
  printSmall(x1-length/4-op1.length()*2+1,y1-2,op1,2);
  printSmall(x1+length/4-op2.length()*2,y1-2,op2,2);
  if(title.length()>0){
    const uint8_t height2 = 9;
    uint8_t length2 = title.length()*4-countSpaces(title)*2;
    display.fillRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_BLACK);
    display.drawRect(x1-length2/2-2,y1-15,length2+4,height2,SSD1306_WHITE);
    printSmall(x1-length2/2,y1-height2-4,title,SSD1306_WHITE);
  }
}
//for no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //bool for breaking from the loop
  bool notChosenYet = true;
  //storing the state
  bool state = false;
  lastTime = millis();
  
  while(true){
    display.clearDisplay();
    drawingFunction();
    drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    display.display();
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      //x to select option
      if(x != 0){
        if(x == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(x == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/delete to cancel
      if(menu_Press || del){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(n || sel){
        lastTime = millis();
        return state;
      }
    }
  }
  return false;
}


void shadeLineV(int16_t xPos, int16_t yPos, int16_t len, uint8_t shade){
  for(uint8_t i = 0; i<=len; i++){
    if(!((yPos+i)%shade)){
      display.drawPixel(xPos,yPos+i,SSD1306_WHITE);
    }
    else
      display.drawPixel(xPos,yPos+i,SSD1306_BLACK);
  }
}

void drawDottedRect(int8_t x1, int8_t y1, uint8_t w, uint8_t h, uint8_t dot){
  //top
  drawDottedLineH(x1,x1+w,y1,dot);
  //bottom
  drawDottedLineH(x1,x1+w,y1+h,dot);
  //left
  drawDottedLineV(x1,y1,y1+h,dot);
  //right
  drawDottedLineV(x1+w,y1,y1+h,dot);
}

void drawRotatedRect(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, float angle, uint16_t c){
  int points[4][2] = {{-length/2,-height/2},{-length/2+length,-height/2},{-length/2+length,-height/2+height},{-length/2,-height/2+height}};
  int points2[4][2];

  //y' = -sin(angle)*x+cos(angle)*y
  //x' = cos(angle)*x+sin(angle)*y
  for(int i = 0; i<4; i++){
    points2[i][0] = cos(radians(angle))*points[i][0]+sin(radians(angle))*points[i][1];
    points2[i][1] = -sin(radians(angle))*points[i][0]+cos(radians(angle))*points[i][1];
  }
  if(points[0][0]<0)
    points[0][0] = 0;
  if(points[0][1]<0)
    points[0][1] = 0;
  if(points[1][0]<0)
    points[1][0] = 0;
  if(points[1][1]<0)
    points[1][1] = 0;
  if(points[2][0]<0)
    points[2][0] = 0;
  if(points[2][1]<0)
    points[2][1] = 0;
  if(points[3][0]<0)
    points[3][0] = 0;
  if(points[3][1]<0)
    points[3][1] = 0;

  display.drawLine(points2[0][0]+x1, points2[0][1]+y1, points2[1][0]+x1, points2[1][1]+y1, c);
  display.drawLine(points2[1][0]+x1, points2[1][1]+y1, points2[2][0]+x1, points2[2][1]+y1, c);
  display.drawLine(points2[2][0]+x1, points2[2][1]+y1, points2[3][0]+x1, points2[3][1]+y1, c);
  display.drawLine(points2[3][0]+x1, points2[3][1]+y1, points2[0][0]+x1, points2[0][1]+y1, c);
}


/*-------------------------------------------
                 Fractions!
-------------------------------------------*/
//fractions are 7x11 (if it's 1 digit numerator)
void printFraction(uint8_t x, uint8_t y, uint8_t numerator, uint8_t denominator){
  printSmall(x,y,stringify(numerator),SSD1306_WHITE);
  display.drawLine(x+1+4*(stringify(numerator).length()-1),y+7,x+5+4*(stringify(numerator).length()-1),y+3,SSD1306_WHITE);
  printSmall(x+4+4*(stringify(numerator).length()-1),y+6,stringify(denominator),SSD1306_WHITE);
}
//feed it a number in the form "1 7/8"
void printFraction(uint8_t x1, uint8_t y1, String fraction){
  String whole;
  String denominator = "";
  String numerator = "";
  bool foundSpaceOrSlash = false;
  uint8_t wholeIndex;
  for(uint8_t i = 0; i<fraction.length(); i++){
    if(fraction.charAt(i) == ' '){
      foundSpaceOrSlash = true;
      whole = fraction.substring(0,i);
      wholeIndex = i;
    }
    if(fraction.charAt(i) == '/'){
      foundSpaceOrSlash = true;
      numerator = fraction.substring(wholeIndex,i);
      denominator = fraction.substring(i+1,fraction.length());
      break;
    }
  }
  //if you didn't find a space or a slash, treat the fraction as a wholenumber
  if(!foundSpaceOrSlash){
    whole = fraction;
  }
  display.setCursor(x1,y1-1);
  display.print(whole);
  //if there's no numerator, return after printing the whole number
  uint16_t num = toInt(numerator);
  uint16_t denom = toInt(denominator);
  //if there's actually a numerator and a denominator
  if(num != 0 && denom != 0)
    printFraction(x1+whole.length()*6, y1-3, num, denom);
}

void printFractionCentered(uint8_t x1, uint8_t y1, String fraction){
  String whole;
  String denominator;
  String numerator;
  uint8_t wholeIndex;
  bool foundSpaceOrSlash = false;
  for(uint8_t i = 0; i<fraction.length(); i++){
    if(fraction.charAt(i) == ' '){
      whole = fraction.substring(0,i);
      wholeIndex = i;
      foundSpaceOrSlash = true;
    }
    if(fraction.charAt(i) == '/'){
      numerator = fraction.substring(wholeIndex,i);
      denominator = fraction.substring(i+1,fraction.length());
      foundSpaceOrSlash = true;
      break;
    }
  }
    //if you didn't find a space or a slash, treat the fraction as a wholenumber
  if(!foundSpaceOrSlash){
    whole = fraction;
  }
  uint16_t num = toInt(numerator);
  uint16_t denom = toInt(denominator);

  int16_t x2;
  //if there is a fractional part
  if(num != 0 && denom != 0)
    x2 = x1-whole.length()*3-numerator.length()*2-denominator.length()*2;
  //if not
  else
    x2 = x1-whole.length()*3;
  display.setCursor(x2,y1-1);
  display.print(whole);
  //if there's actually a numerator and a denominator
  if(num != 0 && denom != 0)
    printFraction(x2+whole.length()*6, y1-3, num, denom);
}

//feed it a number in the form "1 7/8"
uint8_t printFraction_small(uint8_t x1, uint8_t y1, String fraction){
  uint8_t length = 0;
  String whole;
  String denominator;
  String numerator;
  uint8_t wholeIndex;
  bool foundSpaceOrSlash = false;
  for(uint8_t i = 0; i<fraction.length(); i++){
    if(fraction.charAt(i) == ' '){
      whole = fraction.substring(0,i);
      wholeIndex = i;
      foundSpaceOrSlash = true;
    }
    if(fraction.charAt(i) == '/'){
      numerator = fraction.substring(wholeIndex,i);
      denominator = fraction.substring(i+1,fraction.length());
      foundSpaceOrSlash = true;
      break;
    }
  }
  if(!foundSpaceOrSlash){
    whole = fraction;
  }
  printSmall(x1,y1,whole,SSD1306_WHITE);

  length += (4*uint8_t(log10(toInt(whole))+1));
  uint16_t num = toInt(numerator);
  uint16_t denom = toInt(denominator);

  //if there's actually a numerator and a denominator
  if(num != 0 && denom != 0){
    printFraction(x1+whole.length()*4, y1-3, num, denom);
    length = length + uint8_t(log10(num)+1)*4 + uint8_t(log10(denom)+1)*4;
  }
  return length;
}

//centers the fractions
void printFraction_small_centered(uint8_t x1, uint8_t y1, String fraction){
  uint8_t length = 0;
  String whole;
  String denominator;
  String numerator;
  uint8_t wholeIndex;
  bool foundSpaceOrSlash = false;
  for(uint8_t i = 0; i<fraction.length(); i++){
    if(fraction.charAt(i) == ' '){
      whole = fraction.substring(0,i);
      wholeIndex = i;
      foundSpaceOrSlash = true;
    }
    if(fraction.charAt(i) == '/'){
      numerator = fraction.substring(wholeIndex,i);
      denominator = fraction.substring(i+1,fraction.length());
      foundSpaceOrSlash = true;
      break;
    }
  }
  if(!foundSpaceOrSlash){
    whole = fraction;
  }
    
  uint16_t num = toInt(numerator);
  uint16_t denom = toInt(denominator);
    
  length = whole.length();
  //if there's actually a numerator and a denominator
  if(num != 0 && denom != 0){
    length += numerator.length()+denominator.length()-1;//-1 because there's a slash
    printFraction(x1-length*2+whole.length()*4, y1-3, num, denom);
  }
  printSmall(x1-length*2,y1,whole,SSD1306_WHITE);
}

//-------------------------------------------------------------

void drawCheckbox(int x, int y, bool checked, bool selected){
  int size = 7;
  display.fillRect(x,y,size,size,SSD1306_BLACK);
  display.drawRect(x,y,size,size,SSD1306_WHITE);
  if(checked){
    display.drawLine(x+2,y+2,x+size-3,y+size-3,SSD1306_WHITE);
    display.drawLine(x+size-3,y+size-3,x+size+3,y+size-9,SSD1306_WHITE);
  }
  if(selected){
    display.drawRoundRect(x-2,y-2,size+4,size+4,3,SSD1306_WHITE);
  }
}

void drawX(uint8_t x, uint8_t y, uint8_t length, uint8_t thickness){
  for(uint8_t i = 0; i<thickness; i++){
    //drawing top left --> bottom right 
    display.drawLine(x-length/4+i,y-length/4-i,x+length/2+i,y+length/2-i,2);
    //drawing bottom left
    display.drawLine(x-length/4+i,y+length/4+i,x+length/2+i,y-length/2+i,2);
  }
}

void drawPixelAlongCircle(int xOffset, int yOffset, int r, int angle, uint16_t c){
 float x1 = xOffset+r*cos(radians(angle));
 float y1 = yOffset+r*sin(radians(angle));
 if(x1>=0 && x1<screenWidth && y1>=0 && y1 < screenHeight){
  display.drawPixel(x1,y1,c);
 }
}
void drawRadian(uint8_t h, uint8_t k, int a, int b, float angle, uint16_t c) {
  float x1 = h + a * cos(radians(angle));
  float y1;
  if (angle > 180) {
    y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  else {
    y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  display.drawLine(x1, y1, h, k, c);
}

vector<uint8_t> getRadian(uint8_t h, uint8_t k, int a, int b, float angle) {
  float x1 = h + a * cos(radians(angle));
  float y1;
  if (angle > 180) {
    y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  else {
    y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  }
  vector<uint8_t> coords = {uint8_t(x1),uint8_t(y1)};
  return coords;
}


void drawCircleRadian(uint8_t h, uint8_t k, int r, float angle, uint16_t c) {
  while(angle>360){
    angle-=360;
  }
  float x1 = h + r * cos(radians(angle));
  float y1;
  if (angle > 180) {
    y1 = k - r * sqrt(1 - pow((x1 - h), 2) / pow(r, 2));
  }
  else {
    y1 = k + r * sqrt(1 - pow((x1 - h), 2) / pow(r, 2));
  }
  display.drawLine(x1, y1, h, k, c);
}
void fillEllipse(uint8_t h, uint8_t k, int a, int b,uint16_t c){
  for(int i = 0; i<360; i++){
    drawRadian(h,k,a,b,i,c);
  }
}
void fillEllipse_cheap(uint8_t h, uint8_t k, int a, int b,uint16_t c){
  for(int i = 0; i<360; i+=6){
    drawRadian(h,k,a,b,i,c);
  }
}
//useful for drawing rotary encoders
void drawPixelAlongEllipse(uint8_t h, uint8_t k, int a, int b, int angle, uint16_t c){
  float x1 = h+a*cos(radians(angle));
  float y1;
  if(angle>180){
    y1 = k - b*sqrt(1-pow((x1-h),2)/pow(a,2));
  }
  else{
    y1 = k + b*sqrt(1-pow((x1-h),2)/pow(a,2));
  }
  display.drawPixel(x1,y1,c);
}

#define RIGHT 0
#define LEFT 1
#define UP 2
#define DOWN 3

void drawArrow(uint8_t pointX, uint8_t pointY, uint8_t size, uint8_t direction, bool full){
  switch(direction){
    //right
    case RIGHT:
      if(full)
        display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_WHITE);
      else{
        display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_BLACK);
        display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX-size, pointY+size,SSD1306_WHITE);
      }
      break;
    //left
    case LEFT:
      if(full)
        display.fillTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_WHITE);
      else{
        display.fillTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_BLACK);
        display.drawTriangle(pointX, pointY,pointX+size, pointY-size, pointX+size, pointY+size,SSD1306_WHITE);
      }
      break;
    //up
    case UP:
      if(full)
        display.fillTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_WHITE);
      else{
        display.fillTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_BLACK);
        display.drawTriangle(pointX, pointY, pointX-size, pointY+size, pointX+size, pointY+size, SSD1306_WHITE);
      }
      break;
    //down
    case DOWN:
      if(full)
        display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_WHITE);
      else{
        display.fillTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_BLACK);
        display.drawTriangle(pointX, pointY, pointX-size, pointY-size, pointX+size, pointY-size, SSD1306_WHITE);
      }
      break;
  }
}

void drawArrow_highlight(uint8_t pointX,uint8_t pointY, uint8_t size, uint8_t direction){
  drawArrow(pointX,pointY,size+2,direction,true);
  switch(direction){
    case RIGHT:
      drawArrow(pointX-1,pointY,size,direction,false);
      break;
    case LEFT:
      drawArrow(pointX+1,pointY,size,direction,false);
      break;
    case UP:
      drawArrow(pointX,pointY+1,size,direction,false);
      break;
    case DOWN:
      drawArrow(pointX,pointY-1,size,direction,false);
      break;
  }
}

//draws a horizontal bar graph
void drawBarGraphH(int xStart, int yStart, int thickness, int length, float progress){
  float percentage = progress*length;
  display.fillRect(xStart,yStart,length,thickness,SSD1306_BLACK);
  display.drawRect(xStart,yStart,length,thickness,SSD1306_WHITE);
  display.fillRect(xStart,yStart,percentage,thickness,SSD1306_WHITE);
  display.drawRect(xStart+1,yStart+1,length-2,thickness-2,SSD1306_BLACK);
}

//bracket around a note
void drawNoteBracket(int x1, int y1, int length, int height, bool animated){
  float offset;
  if(animated){
    offset = ((millis()/400)%2);
  }
  else
    offset = 0;
  x1++;
  y1++;
  length-=2;
  height-=2;
  if(x1>=viewStart){
    //topL
    display.drawLine(x1-2-offset,y1-2-offset,x1+1-offset,y1-2-offset,SSD1306_WHITE);
    display.drawLine(x1-2-offset,y1-2-offset,x1-2-offset,y1+1-offset,SSD1306_WHITE);
    //bottomL
    display.drawLine(x1-2-offset,y1+height+2+offset,x1+1-offset,y1+height+2+offset,SSD1306_WHITE);
    display.drawLine(x1-2-offset,y1+height+2+offset,x1-2-offset,y1+height-1+offset,SSD1306_WHITE);
  }
  if(x1+length<=viewEnd){
    //topR
    display.drawLine(x1+length+2+offset,y1-2-offset,x1+length-1+offset,y1-2-offset,SSD1306_WHITE);
    display.drawLine(x1+length+2+offset,y1-2-offset,x1+length+2+offset,y1+1-offset,SSD1306_WHITE);
    //bottomR
    display.drawLine(x1+length+2+offset,y1+height+2+offset,x1+length-1+offset,y1+height+2+offset,SSD1306_WHITE);
    display.drawLine(x1+length+2+offset,y1+height+2+offset,x1+length+2+offset,y1+height-1+offset,SSD1306_WHITE);
  }
}

void drawNoteBracket(int x1, int y1, int length, int height){
  drawNoteBracket(x1, y1, length, height, true);
}

void drawNoteBracket(Note note, int track){
  drawNoteBracket(trackDisplay+(note.startPos-viewStart)*scale,headerHeight+(track-startTrack)*trackHeight,(note.endPos-note.startPos+1)*scale,trackHeight);
}

void drawSelectionBracket(){
  vector<uint16_t> bounds  = getSelectedNotesBoundingBox();
  //if the left side is in view
  if(bounds[0]>=viewStart){
    //if the top L corner is in view
    uint8_t x1 = (bounds[0]-viewStart)*scale+trackDisplay-((millis()/200)%2);
    if(bounds[1]>=startTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[1]-startTrack)*trackHeight+headerHeight-((millis()/200)%2);
      display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1-1,x1+5,y1-1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      display.drawLine(x1-1,y1,x1-1,y1+5,SSD1306_WHITE);
    }
    //if the bottom L corner is in view
    if(bounds[3]<=endTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[3]-startTrack+1)*trackHeight+headerHeight+((millis()/200)%2);
      display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1+1,x1+5,y1+1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      display.drawLine(x1-1,y1,x1-1,y1-5,SSD1306_WHITE);
    }
  }
  //if the right corner is in view
  if(bounds[2]<viewEnd){
    uint8_t x1 = (bounds[2]-viewStart)*scale+trackDisplay+((millis()/200)%2)+1;
    //top R corner
    if(bounds[1]>=startTrack){
      uint8_t y1 = (bounds[1]-startTrack)*trackHeight+headerHeight-((millis()/200)%2);
      display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1-1,x1-5,y1-1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      display.drawLine(x1+1,y1,x1+1,y1+5,SSD1306_WHITE);
    }
    //bottom R corner
    if(bounds[3]<=endTrack){
      uint8_t y1 = (bounds[3]-startTrack+1)*trackHeight+headerHeight+((millis()/200)%2);
      display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1+1,x1-5,y1+1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      display.drawLine(x1+1,y1,x1+1,y1-5,SSD1306_WHITE);
    }
  }
}

void drawCenteredBanner(int8_t x1, int8_t y1, String text){
  uint8_t len = text.length()*4-countSpaces(text)*2+countChar(text,'#')*2;
  x1-=len/2;
  drawBanner(x1,y1,text);
}

void drawBanner(int8_t x1, int8_t y1, String text){
  display.drawBitmap(x1-13,y1-4,bannerL_bmp,12,9,SSD1306_WHITE);
  // display.drawBitmap(x1+text.length()*4-countSpaces(text)*2,y1,bannerR_bmp,11,9,SSD1306_WHITE);
  display.setRotation(UPSIDEDOWN);
  display.drawBitmap(screenWidth-(x1+text.length()*4-countSpaces(text)*2+countChar(text,'#')*2)-12,screenHeight-y1-9,bannerL_bmp,12,9,SSD1306_WHITE);
  display.setRotation(UPRIGHT);
  display.fillRect(x1-1,y1-1,text.length()*4-countSpaces(text)*2+countChar(text,'#')*2+1,7,SSD1306_WHITE);
  printSmall(x1,y1,text,SSD1306_BLACK);
}

//draws a VU meter, where val is the angle of the needle
void drawVU(int8_t x1, int8_t y1, float val){
  display.drawBitmap(x1,y1,VUmeter_bmp,19,14,SSD1306_WHITE);
  float angle = radians(5)+(PI-radians(5))*val;
  int8_t pY = 12*sin(angle);
  int8_t pX = 12*cos(angle);
  display.drawLine(x1+9,y1+12,x1+10+pX,y1+12-pY,SSD1306_BLACK);
  display.drawRect(x1,y1,19,14,SSD1306_WHITE);
}

void drawPendulum(int16_t x2, int16_t y2, int8_t length, float val){
  drawPendulum(x2,y2,length,val,3);
}
//draws a swinging pendulum, for the clock menu
void drawPendulum(int16_t x2, int16_t y2, int8_t length, float val,uint8_t r){
  //pendulum
  int a = length;
  int h = x2;
  int k = y2;
  float x1;
  float y1;
  x1 = h + a * cos(radians(val))/float(2.4);
  // if (val > 180) {
    // y1 = k - a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  // }
  // else {
    y1 = k + a * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
  // }
  display.drawLine(x1,y1,h,k,SSD1306_WHITE);
  display.fillCircle(x1,y1,r,SSD1306_BLACK);
  display.drawCircle(x1,y1,r,SSD1306_WHITE);
}

void drawSideLabel(bool side, int8_t y1, String titleText, String bodyText){
  switch(side){
    //left
    case 0:
      display.fillRoundRect(0,y1-1,titleText.length()*4+4,7,4,1);
      display.drawRoundRect(-5,y1+2,bodyText.length()*4+4,16,3,1);
      printSmall(2,y1,titleText,2);
      break;
    //right
    case 1:
      break;
  }
}

void drawLabel_outline(uint8_t x1, uint8_t y1, String text){
  display.fillRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-2,text.length()*4-countSpaces(text)*2+5,9,3,0);
  display.drawRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-2,text.length()*4-countSpaces(text)*2+5,9,3,1);
  printSmall(x1-text.length()*2+countSpaces(text),y1,text,2);
}
void drawLabel(uint8_t x1, uint8_t y1, String text, bool wOrB){
  if(wOrB){
    display.fillRoundRect(x1-text.length()*2+countSpaces(text)-3,y1-1,text.length()*4-countSpaces(text)*2+5,7,3,wOrB == true ? 1:0 );
    printSmall(x1-text.length()*2+countSpaces(text),y1,text,2);
  }
  else{
    drawLabel_outline(x1,y1,text);
  }
}

void drawScaledBitmap(int x1, int y1, const unsigned char * bmp,int w, int h, uint8_t sc){

}


void drawSlider(uint8_t x1, uint8_t y1, uint8_t w, uint8_t h, bool state){
  display.fillRect(x1,y1,w,h,0);
  display.drawRect(x1,y1,w,h,SSD1306_WHITE);
  if(state){
    display.fillRect(x1+w/2,y1+2,w/2-2,h-4,SSD1306_WHITE);
  }
  else{
    display.fillRect(x1+2,y1+2,w/2-2,h-4,SSD1306_WHITE);
  }
}
void drawSlider(uint8_t x1, uint8_t y1, String a, String b, bool state){
  uint8_t length = a.length()*4+b.length()*4+9;
  //if length is odd, add 1
  length+=(length%2)?1:0;
  drawSlider(x1,y1,length,11,state);
  printSmall_centered(x1+length/4+2,y1+3,a,2);
  printSmall_centered(x1+3*length/4,y1+3,b,2);
}

void drawFullKeyBed(vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
  drawFullKeyBed(0,pressList,mask,activeKey,octave);
}

void drawFullKeyBed(uint8_t y1, vector<uint8_t> pressList, vector<uint8_t> mask, uint8_t activeKey,uint8_t octave){
  //white keys
  const uint8_t keyWidth = 5;
  const uint8_t wKeyHeight = 13;
  const uint8_t wKeyPattern[21] = {0, 2, 4, 5, 7,9 ,11,
                                  12,14,16,17,19,21,23,
                                  24,26,28,29,31,33,35};
  //black keys
  const uint8_t bKeyHeight = 8;
  const uint8_t bKeyPattern[15] = {1, 3, 6, 8,10,
                                  13,15,18,20,22,
                                  25,27,30,32,34};
  String text = pitchToString(activeKey,false,true);
  //these 'patterns' help reference the actual note value from the iterator i'm using to draw them
  //the reason i'm doing this in realtime, not with a bitmap, is so i can animate
  //keypresses and (maybe) even change which keys are displayed at all    
  //first draw white keys
  for(uint8_t i = 0; i<21; i++){
    //if there's no mask, or if the key is in the mask
    if(mask.size() == 0 || isInVector(wKeyPattern[i],mask)){
      //if it's pressed, draw it blinking
      if(isInVector(wKeyPattern[i]+12*octave,pressList)){
        if(millis()%800>400)
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        else
          display.fillRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey){
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
          drawArrow(i*(keyWidth+1)+2,y1+17+((millis()/200)%2),3,2,true);
          printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
        }
        display.drawPixel(i*(keyWidth+1)+2,y1+15,SSD1306_WHITE);
      }
      else{
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey){
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
          drawArrow(i*(keyWidth+1)+2,y1+17+((millis()/200)%2),3,2,true);
          printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
        }
        else
          display.fillRect(i*(keyWidth+1),y1,keyWidth,wKeyHeight,SSD1306_WHITE);
      }
    }
  }
  //then draw black keys
  uint8_t xPos = 3;
  for(uint8_t i = 0; i<15; i++){
    if(mask.size() == 0 || isInVector(bKeyPattern[i]+12*octave,mask)){
      if(isInVector(bKeyPattern[i]+12*octave,pressList) || bKeyPattern[i]+12*octave == activeKey){
        display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
        //if it's pressed
        if(isInVector(bKeyPattern[i]+12*octave,pressList)){
          if(millis()%800>400){
            display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
          }
          display.drawPixel(xPos+2,y1+15,SSD1306_WHITE);
        }
        else
          display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
        display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
        if(bKeyPattern[i]+12*octave == activeKey){
          drawArrow(xPos+2,y1+17+((millis()/200)%2),3,2,false);
          printSmall(xPos+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
        }
      }
      else{
        display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
        display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);
      }
    }
    //if it's a D# or a Bb, you're about to jump
    if(abs(bKeyPattern[i]%12) == 3 || abs(bKeyPattern[i]%12) == 10)
      xPos+= 2*(keyWidth+1); 
    //if it's not, just increment like normal
    else
      xPos+= 1+keyWidth;
  }
}

void drawX(uint8_t x1, uint8_t y1, uint8_t width){
  uint8_t points[3][2] = {{uint8_t(x1-width/2+width/4),uint8_t(y1-width/2)},{x1,uint8_t(y1-width/4)},{uint8_t(x1+width/2-width/4),uint8_t(y1-width/2)}};
}

void drawStar(uint8_t centerX, uint8_t centerY, uint8_t r1, uint8_t r2, uint8_t points){
  uint8_t numberOfPoints = points*2;//the actual number of points (both convex and concave vertices)
  uint8_t coords[numberOfPoints][2];
  for(uint8_t pt = 0; pt<numberOfPoints; pt++){
    vector<uint8_t> pair;
    //if it's odd, it's a convex point
    if(!(pt%2))
      pair = getRadian(centerX, centerY, r1, r1, pt*360/numberOfPoints);
    else
      pair = getRadian(centerX, centerY, r2, r2, pt*360/numberOfPoints);
    coords[pt][0] = pair[0];
    coords[pt][1] = pair[1];
  }
  for(uint8_t pt = 0; pt<numberOfPoints; pt++){
    if(pt == numberOfPoints-1){
      display.drawLine(coords[pt][0],coords[pt][1],coords[0][0],coords[0][1],SSD1306_WHITE);
    }
    else{
      display.drawLine(coords[pt][0],coords[pt][1],coords[pt+1][0],coords[pt+1][1],SSD1306_WHITE);
    }
  }
}

void drawPram(uint8_t x1, uint8_t y1){
  if(onBeat(2,30))
    display.drawBitmap(x1,y1+1,carriage_bmp,14,15,SSD1306_WHITE);
  else
    display.drawBitmap(x1,y1,carriage_bmp,14,15,SSD1306_WHITE);
}

void drawPlayIcon(int8_t x1, int8_t y1){
  if(clockSource == EXTERNAL && !gotClock){
    display.drawTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
  else{
    display.fillTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
}

void drawPower(uint8_t x1, uint8_t y1){
  //check if USB is plugged in
  bool usb = digitalRead(usbPin);
  if(usb){
    // display.drawBitmap(x1,y1,batt_bmp,10,7,SSD1306_WHITE);
    display.drawBitmap(x1,y1+1,tiny_usb,10,4,SSD1306_WHITE);
  }
  else{
    display.drawBitmap(x1,y1,batt_bmp,10,7,SSD1306_WHITE);
    float batt = getBattLevel();
    //for printing batt level to the screen
    // String lvl = stringify(batt);
    // printSmall(x1-lvl.length()*4,y1+1,lvl,1);
    //input ranges from ~1.8 (lowest the Pico can run on) to 3.6v (with 3AA's @ 1.2v)
    //so the range is 1.8, thus u gotta do 6 increments of 0.3, 1.8 --> 
    if(batt<=1.8){
    }
    else if(batt<2.1){
      display.fillRect(x1+2,y1+2,1,3,SSD1306_WHITE);
    }
    else if(batt<2.4){
      display.fillRect(x1+2,y1+2,2,3,SSD1306_WHITE);
    }
    else if(batt<2.7){
      display.fillRect(x1+2,y1+2,3,3,SSD1306_WHITE);
    }
    else if(batt<3.0){
      display.fillRect(x1+2,y1+2,4,3,SSD1306_WHITE);
    }
    else if(batt<3.3){
      display.fillRect(x1+2,y1+2,5,3,SSD1306_WHITE);
    }
    else{
      display.fillRect(x1+2,y1+2,6,3,SSD1306_WHITE);
    }
  }
}

//draws pram, other icons (not loop points tho)
void drawTopIcons(){
  if(!menuIsActive || (activeMenu.menuTitle == "MENU") || (activeMenu.menuTitle == "FX") || (activeMenu.menuTitle == "EDIT" && activeMenu.coords.x1>32)){
    if(maxTracksShown == 5){
      if(!playing && !recording){
        drawPram(5,0);
      }
      //pram bounces faster
      else if(playing || recording){
        //if the playhead/rechead is on a subdiv, bounce the pram (this is set in loop1, it's too hard to do accurately here)
        display.drawBitmap(5,pramOffset,carriage_bmp,14,15,SSD1306_WHITE);
      }
    }
    else{
      if(!playing && !recording){
        if(onBeat(2,30))
          display.drawBitmap(8,1,tinyPram,7,7,SSD1306_WHITE);
        else
          display.drawBitmap(8,0,tinyPram,7,7,SSD1306_WHITE);
      }
      //pram bounces faster
      else if(playing || recording){
        //if the playhead/rechead is on a subdiv, bounce the pram (this is set in loop1, it's too hard to do accurately here)
        display.drawBitmap(8,pramOffset,tinyPram,7,7,SSD1306_WHITE);
      }
    }
  }

  // if it's recording and waiting for a note
  if(recording && waitingToReceiveANote){
    if(millis()%1000>500){
      writeLEDs(0,true);
    }
    else{
      writeLEDs(0,false);
    }
  }

  //music symbol while receiving notes
  if(isReceivingOrSending()){
    if(!((animOffset/10)%2)){
      display.drawChar(21,0,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
    else{
      display.drawChar(21,1,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
    }
  }

  //note presence indicator(if notes are offscreen)
  if(areThereMoreNotes(true)){
    uint8_t y1 = (maxTracksShown>5&&!menuIsActive)?8:headerHeight;
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,y1+3,trackDisplay-3,y1+3,trackDisplay-5,y1+1, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,y1+2,trackDisplay-3,y1+2,trackDisplay-5,y1, SSD1306_WHITE);
    }
  }
  if(areThereMoreNotes(false)){
    if(!((animOffset/10)%2)){
      display.fillTriangle(trackDisplay-7,screenHeight-5,trackDisplay-3,screenHeight-5,trackDisplay-5,screenHeight-3, SSD1306_WHITE);
    }
    else{
      display.fillTriangle(trackDisplay-7,screenHeight-4,trackDisplay-3,screenHeight-4,trackDisplay-5,screenHeight-2, SSD1306_WHITE);
    }
  }

  uint8_t x1 = 32;
  //rec/play icon
  if(recording){
    if(clockSource == EXTERNAL && !gotClock){
      if(waitingToReceiveANote){
        if(millis()%1000>500){
          display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.drawCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    else if((clockSource == EXTERNAL && gotClock) || clockSource == INTERNAL){
      if(waitingToReceiveANote){
        if(millis()%1000>500){
          display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
        }
      }
      else
        display.fillCircle(trackDisplay+3,3,3,SSD1306_WHITE);
    }
    x1+=9;
    switch(recMode){
      //if one-shot rec
      case 0:
        // display.drawBitmap(x1+((millis()/200)%2),0,oneShot_rec,7,7,SSD1306_WHITE);
        printSmall(x1,1,"1",1);
        x1+=4;
        if((millis()/10)%100>50)
          display.drawBitmap(x1,1,oneshot_bmp,3,5,SSD1306_WHITE);
        x1+=4;
        break;
      //if continuous recording
      case 1:
        display.drawBitmap(x1,1,continuous_bmp,9,5,SSD1306_WHITE);
        x1+=10;
        break;
    }
    if(overwriteRecording){
      display.drawBitmap(x1,0,((millis()/10)%100>50)?overwrite_1:overwrite_2,7,7,SSD1306_WHITE);
      x1+=8;
    }
  }
  else if(playing){
    drawPlayIcon(trackDisplay+((millis()/200)%2)+1,0);
    x1 += 9;
    switch(isLooping){
      //if not looping
      case 0:
        if((millis()/10)%100>50)
          display.drawFastVLine(x1,0,7,1);
        printSmall(x1+3,1,"1",1);
        x1+=10;
        break;
      //if looping
      case 1:
        if(millis()%1000>500){
          display.drawBitmap(x1,0,toploop_arrow1,7,7,SSD1306_WHITE);
        }
        else{
          display.drawBitmap(x1,0,toploop_arrow2,7,7,SSD1306_WHITE);
        }
        x1+=10;
        break;
    }

  }
  //Data track icon
  if(autotrackData.size()>0){
    if(millis()%1600>800)
      // display.drawBitmap(x1,1,sine_small_bmp,6,4,SSD1306_WHITE);
      display.drawBitmap(x1,0,autotrack1,10,7,SSD1306_WHITE);
    else{
      // display.drawBitmap(x1,1,sine_small_reverse_bmp,6,4,SSD1306_WHITE);
      display.drawBitmap(x1,0,autotrack2,10,7,SSD1306_WHITE);
    }
    x1+=12;
  }

  //swing icon
  if(swung){
    drawPendulum(x1+2,0,7,millis()/2,1);
    x1+=10;
  }
  //fragment gem
  if(isFragmenting){
    drawTetra(x1,5,10+sin(float(millis())/float(500)),10+sin(float(millis())/float(500)),6+sin(float(millis())/float(500)),1+sin(float(millis())/float(500)),0,SSD1306_WHITE);
    x1+=8;
  }

  //velocity/chance indicator while shifting
  if(shift){
    if(displayingVel){
      if(lookupData[activeTrack][cursorPos] == 0){
        String vel = stringify(defaultVel);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;;
      }
      else{
        String vel = stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].velocity);
        printSmall(x1,1,"v:"+vel,SSD1306_WHITE);
        x1+=vel.length()*4+8;
      }
    }
    else{
      if(lookupData[activeTrack][cursorPos] == 0){
        printSmall(x1,1,"c:"+stringify(100),SSD1306_WHITE);
        x1+=20;
      }
      else{
        printSmall(x1,1,"c:"+stringify(seqData[activeTrack][lookupData[activeTrack][cursorPos]].chance),SSD1306_WHITE);
        x1+=20;
      }
    }
  }
  if(isArping){
    display.drawPixel(x1,3+2*sin(float(millis())/float(200)),1);
    display.drawPixel(x1+2,3+2*sin(float(millis())/float(200)+100),1);
    display.drawPixel(x1+4,3+2*sin(float(millis())/float(200)+200),1);
    // printSmall(x1+6,1,"a",1);
    display.drawBitmap(x1+6,2,tiny_arp_bmp,9,3,1);
    // x1+=8;
    x1+=17;
  }

  //draw menu text
  printSmall(x1,1,menuText,SSD1306_WHITE);
  x1+=menuText.length()*4+2;

  //power/battery indicator
  drawPower(screenWidth-10,0);
}

//same function, but doesn't clear or display the screen
void drawSeq(){
  drawSeq(true,true,true,true,false);
}
//sends data to screen
//move through rows/columns, printing out data
void displaySeq(){
  display.clearDisplay();
  drawSeq();
  display.display();
}

void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection){
  drawSeq(trackLabels,topLabels,loopPoints,menus,trackSelection,false,viewStart,viewEnd);
}

void drawSeqBackground(uint16_t start, uint16_t end, uint8_t startHeight, uint8_t height, bool onlyWithinLoop, bool loopFlags, bool loopPoints){
  //drawing the measure bars
  for (uint16_t step = start; step < end; step++) {
    unsigned short int x1 = trackDisplay+int((step-start)*scale);
    unsigned short int x2 = x1 + (step-start)*scale;

    //shade everything outside the loop
    if(onlyWithinLoop){
      if(step<loopData[activeLoop].start){
        shadeArea(x1,startHeight,(loopData[activeLoop].start-step)*scale,screenHeight-startHeight,3);
        step = loopData[activeLoop].start;
        //ok, step shouldn't ever be zero in this case, since that would mean it was LESS than zero to begin
        //with. But, just for thoroughnesses sake, make sure step doesn't overflow when you subtract from it
        if(step != 0){
          step--;
        }
        continue;
      }
      else if(step>loopData[activeLoop].end){
        shadeArea(x1,startHeight,(viewEnd-loopData[activeLoop].end)*scale,screenHeight-startHeight,3);
        break;
      }
    }

    //if the last track is showing
    if(endTrack == trackData.size()){
      //measure bars
      if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>1) {
        drawDottedLineV(x1,startHeight,height,2);
      }
      if(!(step%96)){
        drawDottedLineV2(x1,startHeight,height,6);
      }
    }
    else{
      //measure bars
      if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>1) {
        drawDottedLineV(x1,startHeight,height,2);
      }
      if(!(step%96)){
        drawDottedLineV2(x1,startHeight,height,6);
      }
    }

    //drawing loop points/flags
    if(loopPoints){//check
      if(step == loopData[activeLoop].start){
        if(loopFlags){
          if(movingLoop == -1 || movingLoop == 2){
            display.fillTriangle(trackDisplay+(step-start)*scale, headerHeight-3-sin(millis()/50), trackDisplay+(step-start)*scale, headerHeight-7-sin(millis()/50), trackDisplay+(step-start)*scale+4, headerHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-start)*scale,headerHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(cursorPos == step){
              display.fillTriangle(trackDisplay+(step-start)*scale, headerHeight-3, trackDisplay+(step-start)*scale, headerHeight-7, trackDisplay+(step-start)*scale+4, headerHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-start)*scale,headerHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.fillTriangle(trackDisplay+(step-start)*scale, headerHeight-1, trackDisplay+(step-start)*scale, headerHeight-5, trackDisplay+(step-start)*scale+4, headerHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(loopData[activeLoop].start-start)*scale, startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(step-start)*scale,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(step-start)*scale-1,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
        }
      }
      if(step == loopData[activeLoop].end-1){
        if(loopFlags){
          if(movingLoop == 1 || movingLoop == 2){
            display.drawTriangle(trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-3-sin(millis()/50), trackDisplay+(loopData[activeLoop].end-start)*scale-4, headerHeight-7-sin(millis()/50), trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-start)*scale,headerHeight-3,3,SSD1306_WHITE);
          }
          else{
            if(cursorPos == step+1){
              display.drawTriangle(trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-3, trackDisplay+(loopData[activeLoop].end-start)*scale-4, headerHeight-7, trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-start)*scale,headerHeight-3,3,SSD1306_WHITE);
            }
            else{
              display.drawTriangle(trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-1, trackDisplay+(loopData[activeLoop].end-start)*scale-4, headerHeight-5, trackDisplay+(loopData[activeLoop].end-start)*scale, headerHeight-5,SSD1306_WHITE);
            }
          }
        }
        else{
          display.drawPixel(trackDisplay+(loopData[activeLoop].end-start)*scale, startHeight-1,1);
        }
        if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
          display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-start)*scale+1,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
          display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-start)*scale+2,startHeight,screenHeight-startHeight-(endTrack == trackData.size()),SSD1306_WHITE);
        }
      }
      if(movingLoop == 2){
        if(step>loopData[activeLoop].start && step<loopData[activeLoop].end && step%2){
          display.drawPixel(trackDisplay+(step-start)*scale, startHeight-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(loopFlags && (step == loopData[activeLoop].start+(loopData[activeLoop].end-loopData[activeLoop].start)/2))
        printSmall(trackDisplay+(step-start)*scale-1,10,stringify(activeLoop),SSD1306_WHITE);
    }
  }
}

//this function is a mess!
void drawSeq(bool trackLabels, bool topLabels, bool loopPoints, bool menus, bool trackSelection, bool shadeOutsideLoop, uint16_t start, uint16_t end){
  if(!screenSaverActive){
    //handling the note view
    if(end>seqEnd){
      end = seqEnd;
    }
    if(start>end){
      start = 0;
    }
    uint16_t viewLength = end - start;

    uint8_t startHeight = 0;
    bool loopFlags = loopPoints;

    if(maxTracksShown == 6 && !menuIsActive){
      startHeight = 8;
      trackHeight = (screenHeight-startHeight)/maxTracksShown;
      loopFlags = false;
    }
    else{
      trackHeight = (screenHeight-headerHeight)/maxTracksShown;
      startHeight = headerHeight;
    }

    if(trackData.size()>maxTracksShown){
      endTrack = startTrack + maxTracksShown;
    }
    else{
      endTrack = startTrack + trackData.size();
    }
    while(activeTrack>=endTrack && trackData.size()>maxTracksShown){
      startTrack++;
      endTrack++;
    }
    while(activeTrack<startTrack && trackData.size()>maxTracksShown){
      startTrack--;
      endTrack--;
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    // display.setTextWrap(false);

    //drawing selection box, since it needs to overlay stepSeq data
    if(selBox.begun){
      selBox.displaySelBox();
    }
    uint8_t height;
    if(endTrack == trackData.size())
      height = startHeight+trackHeight*maxTracksShown;
    else if(trackData.size()>maxTracksShown)
      height = startHeight+trackHeight*(maxTracksShown+1);
    else
      height = startHeight+trackHeight*trackData.size();

    //drawing measure bars, loop points
    drawSeqBackground(start, end, startHeight, height, shadeOutsideLoop,loopFlags, loopPoints);

    //top and bottom bounds
    if(startTrack == 0){
      display.drawFastHLine(trackDisplay,startHeight,screenWidth,SSD1306_WHITE);
    }
    //if the bottom is in view
    if(endTrack == trackData.size()){
      display.drawFastHLine(trackDisplay,startHeight+trackHeight*maxTracksShown,screenWidth,SSD1306_WHITE);
    }
   else if(endTrack< trackData.size())
        endTrack++;
    //drawin all da steps
    //---------------------------------------------------
    for (int track = startTrack; track < endTrack; track++) {
      unsigned short int y1 = (track-startTrack) * trackHeight + startHeight;
      unsigned short int y2 = y1 + trackHeight;
      uint8_t xCoord;
      //track info display
      if(activeTrack == track){
        xCoord = 9;
        // display.setCursor(9, y1+trackHeight/2-2);
        if(trackLabels)
          drawArrow(6+((millis()/400)%2),y1+trackHeight/2+1,2,0,true);
      }
      else{
        xCoord = 5;
        // display.setCursor(5, y1+trackHeight/2-2);
      }
      if(trackLabels){
        if(!isShrunk){
          //printing note names
          if(pitchesOrNumbers){
            printTrackPitch(xCoord, y1+trackHeight/2-2,track,false,true,SSD1306_WHITE);
          }
          //just printing pitch numbers
          else{
            display.setCursor(xCoord,y1+2);
            display.print(trackData[track].pitch);
          }
          // else{
          //   if(track == activeTrack && isModulated(trackData[track].channel)){
          //     if(playing)
          //       display.drawBitmap(trackDisplay-7,y1+3+((millis()/200)%2),sine_small_bmp,6,4,SSD1306_WHITE);
          //     else
          //       display.drawBitmap(trackDisplay-7,y1+2,sine_small_bmp,6,4,SSD1306_WHITE);
          //   }
          // }
        }
        //if it's shrunk, draw it small
        else{
          String pitch = getTrackPitchOctave(track);
          if(track%2){
            printSmall(18, y1, pitchToString(trackData[track].pitch,true,true), SSD1306_WHITE);
          }
          else
            printSmall(2, y1, pitchToString(trackData[track].pitch,true,true), SSD1306_WHITE);
          if(trackData[track].noteLastSent != 255){
            display.drawRect(0,y1,trackDisplay,trackHeight+2,SSD1306_WHITE);
          }
        }
      }
      //if you're only drawing selected tracks, and this track ISN'T selected, shade it
      if(trackSelection && !trackData[track].isSelected){
        display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,3);
      }
      //if the track is muted, just hatch it out (don't draw any notes)
      //if it's solo'd and muted, draw it normal (solo overrules mute)
      else if(trackData[track].isMuted && !trackData[track].isSolo){
        // display.fillRect(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,0);
        shadeArea(trackDisplay,y1,screenWidth-trackDisplay,trackHeight,9);
        continue;
      }
      else{
        //highlight for solo'd tracks
        if(trackData[track].isSolo)
          drawNoteBracket(trackDisplay+3,y1-1,screenWidth-trackDisplay-5,trackHeight+2,true);
        // display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
        for (int step = start; step < end; step++) {
          //if you only want to draw what's within the loop
          if(shadeOutsideLoop){
            //if you're less than the loop, jump to the start of the loop
            //(this could also be in the initial for loop condition!)
            if(step<loopData[activeLoop].start){
              step = loopData[activeLoop].start;
            }
            //if you're past the loop end, break out of the for loop
            else if(step>=loopData[activeLoop].end){
              break;
            }
          }
          int id = lookupData[track][step];
          unsigned short int x1 = trackDisplay+int((step-start)*scale);
          unsigned short int x2 = x1 + (step-start)*scale;
          //drawing note
          if (id != 0){
            if(step == seqData[track][id].startPos){
              uint16_t length = (seqData[track][id].endPos - seqData[track][id].startPos)*scale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,seqData[track][id].velocity,seqData[track][id].isSelected,seqData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,seqData[track][id].chance,seqData[track][id].isSelected,seqData[track][id].muted);
              if(seqData[track][id].isSelected){
                display.drawRect(x1,y1,length+1,trackHeight,SSD1306_WHITE);
                display.drawRect(x1+1,y1+1,length-1,trackHeight-2,SSD1306_BLACK);
              }
            }
            else if(!isInView(seqData[track][id].startPos) && step == start){
              unsigned short int length = (seqData[track][id].endPos - start)*scale;
              if(displayingVel)
                drawNote_vel(id, track, x1,y1,length,trackHeight,seqData[track][id].velocity,seqData[track][id].isSelected,seqData[track][id].muted);
              else
                drawNote_chance(id, track, x1,y1,length,trackHeight,seqData[track][id].chance,seqData[track][id].isSelected,seqData[track][id].muted);
            }
          }
        }
      }
    }
    //---------------------------------------------------
    //drawing cursor
    if(!editingNote && !(menuIsActive && activeMenu.menuTitle == "EDIT" && activeMenu.page == 1)){
      uint8_t cPos = trackDisplay+int((cursorPos-start)*scale);
      if(cPos>127)
        cPos = 126;
      if(endTrack == trackData.size()){
        display.drawFastVLine(cPos, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((cursorPos-start)*scale), startHeight, 2, (endTrack-startTrack)*trackHeight, SSD1306_WHITE);
      }
      else{
        display.drawFastVLine(cPos, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        display.drawFastVLine(cPos+1, startHeight, screenHeight-startHeight, SSD1306_WHITE);
        // display.drawRect(trackDisplay+int((cursorPos-start)*scale), startHeight, 2, screenHeight-startHeight, SSD1306_WHITE);
      }
    }
    //carriage bitmap/title
    if(topLabels)
      drawTopIcons();
    //playhead/rechead
    if(playing && isInView(playheadPos))
      display.drawRoundRect(trackDisplay+(playheadPos-start)*scale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    if(recording && isInView(recheadPos))
      display.drawRoundRect(trackDisplay+(recheadPos-start)*scale,startHeight,3, screenHeight-startHeight, 3, SSD1306_WHITE);
    
    int cursorX = trackDisplay+int((cursorPos-start)*scale)-8;
    if(!playing && !recording){
      cursorX = 32;
    }
    else{
      //making sure it doesn't print over the subdiv info
      cursorX = 42;
    }
    //drawing active track highlight
    unsigned short int x1 = trackDisplay;
    unsigned short int y1 = (activeTrack-startTrack) * trackHeight + startHeight;
    unsigned short int x2 = x1+screenWidth-trackDisplay;
    unsigned short int y2 = y1 + trackHeight;
    // display.drawRect(x1, y1, screenWidth, trackHeight, SSD1306_WHITE);
    display.drawFastHLine(x1,y1,screenWidth-x1,1);
    display.drawFastHLine(x1,y1+trackHeight-1,screenWidth-x1,1);
  }
  if(menus && menuIsActive){
    activeMenu.displayMenu(true,true);
  }

  //anim offset (for the pram)
  if(!menuIsActive){
    animOffset++;
    if(animOffset>100)
      animOffset = 0;
  }

  if(playing || recording)
    updateLEDs();
}


//draws dashed note (MAKE THIS FASTER!)
void drawNote(uint16_t id, uint8_t track, unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned short int height, unsigned short int shade, bool isSelected, bool mute){
  //makes drawing zoomed out notes faster
  if(len>=3){
    if(!mute){
      if(shade != 1){//so it does this faster
        // display.fillRect(x1,y1,len,height,SSD1306_BLACK);//clearing out the note area
        display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_BLACK);//clearing out the note area
        for(int j = 1; j<height-2; j++){//shading the note...
          for(int i = x1+1;i+j%shade<x1+len-1; i+=shade){
            display.drawPixel(i+j%shade,y1+j,SSD1306_WHITE);
          }
        }
        display.drawRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      }
      else{//filling note
        display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      }
      //line at the end, if there's something at the end
      if(lookupData[track][seqData[track][id].endPos] != 0)
        display.drawFastVLine(x1+len,y1+1,height-2,SSD1306_BLACK);
    }
    //if it's muted
    else{
      //body
      display.fillRect(x1+1, y1+1, len-1, height-2, SSD1306_BLACK);
      display.drawRect(x1+1, y1+1, len-1, height-2, SSD1306_WHITE);
      display.drawLine(x1+1,y1+1, x1+len-1, y1+height-2,SSD1306_WHITE);
      display.drawLine(x1+1,y1+height-2,x1+len-1,y1+1,SSD1306_WHITE);
      display.drawFastVLine(x1+len,y1+1,height-2,SSD1306_BLACK);
    }
    if(isSelected){
      display.drawRect(x1,y1,len+1,trackHeight,SSD1306_WHITE);
      display.drawRect(x1,y1+1,len,trackHeight-2,SSD1306_BLACK);
      display.drawRect(x1+2,y1+2,len-3,trackHeight-4,SSD1306_WHITE);
    }
  }
  else{
    display.fillRect(x1, y1+1, len+2, height-2, SSD1306_WHITE);
  }
}

void drawNote_vel(uint16_t id, uint8_t track, unsigned short int xStart, unsigned short int yStart, unsigned short int length, unsigned short int height, unsigned short int vel, bool isSelected, bool isMuted){
  if(vel>125)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,isMuted);
  else if(vel>110)
    drawNote(id, track, xStart,yStart,length,height,2,isSelected,isMuted);
  else if(vel>100)
    drawNote(id, track, xStart,yStart,length,height,3,isSelected,isMuted);
  else if(vel>90)
    drawNote(id, track, xStart,yStart,length,height,4,isSelected,isMuted);
  else if(vel>80)
    drawNote(id, track, xStart,yStart,length,height,5,isSelected,isMuted);
  else if(vel>70)
    drawNote(id, track, xStart,yStart,length,height,6,isSelected,isMuted);
  else if(vel>60)
    drawNote(id, track, xStart,yStart,length,height,7,isSelected,isMuted);
  else if(vel>50)
    drawNote(id, track, xStart,yStart,length,height,8,isSelected,isMuted);
  else if(vel>40)
    drawNote(id, track, xStart,yStart,length,height,9,isSelected,isMuted);
  else if(vel>30)
    drawNote(id, track, xStart,yStart,length,height,10,isSelected,isMuted);
  else if(vel>20)
    drawNote(id, track, xStart,yStart,length,height,11,isSelected,isMuted);
  else if(vel>10)
    drawNote(id, track, xStart,yStart,length,height,12,isSelected,isMuted);
  else if(vel>0)
    drawNote(id, track, xStart,yStart,length,height,13,isSelected,isMuted);
  else if(vel == 0)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,true);
}

void drawNote_chance(uint16_t id, uint8_t track, unsigned short int xStart, unsigned short int yStart, unsigned short int length, unsigned short int height, unsigned short int chance, bool isSelected, bool isMuted){
  if(chance>=100)
    drawNote(id, track, xStart,yStart,length,height,1,isSelected,isMuted);
  else if(chance>90)
    drawNote(id, track, xStart,yStart,length,height,2,isSelected,isMuted);
  else if(chance>80)
    drawNote(id, track, xStart,yStart,length,height,3,isSelected,isMuted);
  else if(chance>70)
    drawNote(id, track, xStart,yStart,length,height,4,isSelected,isMuted);
  else if(chance>60)
    drawNote(id, track, xStart,yStart,length,height,5,isSelected,isMuted);
  else if(chance>50)
    drawNote(id, track, xStart,yStart,length,height,6,isSelected,isMuted);
  else if(chance>40)
    drawNote(id, track, xStart,yStart,length,height,7,isSelected,isMuted);
  else if(chance>30)
    drawNote(id, track, xStart,yStart,length,height,8,isSelected,isMuted);
  else if(chance>20)
    drawNote(id, track, xStart,yStart,length,height,9,isSelected,isMuted);
  else if(chance>10)
    drawNote(id, track, xStart,yStart,length,height,10,isSelected,isMuted);
  else if(chance>0)
    drawNote(id, track, xStart,yStart,length,height,11,isSelected,isMuted);
  else if(chance == 0)
    drawNote(id, track, xStart,yStart,length,height,12,isSelected,true);
}

void fillSquareVertically(uint8_t x0, uint8_t y0, uint8_t width, uint8_t fillAmount){
  display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  uint8_t maxLine = float(fillAmount)/float(100)*(width-4);
  for(uint8_t line = 0; line<maxLine; line++){
    display.drawFastHLine(x0+2,y0+width-3-line,width-4,1);
  }
}
//fill amount is a percent
void fillSquareDiagonally(uint8_t x0, uint8_t y0, uint8_t width,uint8_t fillAmount){
  display.drawRect(x0,y0,width,width,SSD1306_WHITE);
  //fillAmount = lines/width
  uint8_t maxLine = float(fillAmount)/float(100)*width*sqrt(2);
  // if(fillAmount == 1)
  //   maxLine++;
  for(uint8_t line = 0; line<maxLine; line++){
    //bottom right
    int8_t x1 = x0+2+line;
    int8_t y1 = y0+width-3;
    if(x1>x0+width-2)
      x1=x0+width-2;

    //top left
    int8_t x2 = x0+2;
    int8_t y2 = y0+width-3-line;

    if(x1>=x0+width-2){
      x1 = x0+width-3;
      y1 = y0+width-3-(line-width+5);
    }
    if(y2<y0+2){
      y2 = y0+2;
      x2 = x0+2+(line-width+5);
    }
    if(y1<y0+2)
      y1 = y0+2;
    if(x2>x0+width-2)
      x2 = x0+width-2;
    // //Serial.println("x1:"+stringify(x1));
    // //Serial.println("y1:"+stringify(y1));
    // //Serial.println("x2:"+stringify(x2));
    // //Serial.println("y2:"+stringify(y2));
    display.drawLine(x1,y1,x2,y2,SSD1306_WHITE);
    // //Serial.println("line:"+stringify(line));
    // //Serial.println("w:"+stringify(width));
  }
}


//prints pitch with a small # and either a large or small Octave number
void printTrackPitch(uint8_t xCoord, uint8_t yCoord, uint8_t trackID,bool bigOct, bool channel, uint16_t c){
  String s = getTrackPitchOctave(trackID)+stringify(trackData[trackID].channel)+((trackData[trackID].noteLastSent != 255)?"$":"");
  uint8_t offset = printPitch(xCoord, yCoord, s, bigOct, channel, c);
  offset+=4;
  //if you want to show the track channel
  if(shift || (menuIsActive && activeMenu.menuTitle == "TRK")){
    String sx = ":";
    sx += stringify(trackData[trackID].channel);
    // if(trackData[trackID].isLatched){
    //   sx += "<";
    // }
    // display.drawBitmap(xCoord+offset+2,yCoord,ch_tiny,6,3,1);
    printSmall(xCoord+offset,yCoord,sx,1);
    offset+=sx.length()*4;
  }
  //if you want to show the track "primed" status for recording
  if(recording && trackData[trackID].isPrimed){
    if((millis()+trackID*10)%1000>500){
      display.fillCircle(trackDisplay-5,yCoord+1,2,1);
    }
    else{
      display.drawCircle(trackDisplay-5,yCoord+1,2,1);
    }
  }
}

//prints a formatted pitch and returns the pixel length of the printed pitch
uint8_t printPitch(uint8_t xCoord, uint8_t yCoord, String pitch, bool bigOct, bool channel, uint16_t c){
  uint8_t offset = 0;
  display.setCursor(xCoord,yCoord);
  display.print(pitch.charAt(0));
  offset+=6;
  // printChunky(xCoord,yCoord,stringify(pitch.charAt(0)),c);
  //if it's a sharp
  if(pitch.charAt(1) == '#'){
    printSmall(xCoord+offset,yCoord,pitch.charAt(1),c);
    offset+=6;
    printSmall(xCoord+offset,yCoord,pitch.charAt(2),c);
    if(pitch.charAt(2) == '-'){
      offset+=4;
      printSmall(xCoord+offset,yCoord,pitch.charAt(3),c);
    }
  }
  //if it's not a sharp
  else{
    printSmall(xCoord+offset,yCoord,pitch.charAt(1),c);
    if(pitch.charAt(1) == '-'){
      offset+=4;
      printSmall(xCoord+offset,yCoord,pitch.charAt(2),c);
    }
  }
  if(pitch.charAt(pitch.length()-1) == '$'){
    offset+=4;
    printSmall(xCoord+offset,1+yCoord+sin(millis()/50),"$",c);
  }
  return offset;
}

void drawBox(uint8_t cornerX, uint8_t cornerY, uint8_t width, uint8_t height, uint8_t depth, int8_t xSlant, uint8_t fill){
  // if(cornerX+width>screenWidth || cornerY+height>screenHeight){
  //   return;
  // }
  int16_t point[4][2] = {{cornerX,cornerY},
                         {int16_t(cornerX+width),cornerY},
                         {int16_t(cornerX+width+xSlant),int16_t(cornerY+depth)},
                         {int16_t(cornerX+xSlant),int16_t(cornerY+depth)}};
  switch(fill){
    //transparent box (wireframe)
    case 0:
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);
      //draw bottom face
      display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      break;
    //for a solid self-occluding box
    case 1:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_WHITE);

      if(xSlant>=0){
        //draw bottom face
        display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
        //draw vertical edges
        display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      else if(xSlant<0){
        //draw bottom face
        display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
        //draw vertical edges
        display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_WHITE);
      }
      break;
    //for a solid,filled box
    //draw top face again and again
    case 2:
      for(int i = 0; i<height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      break;
    //for a solid box, with black edges
    case 3:
      for(int i = 0; i<height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);

      //draw bottom face
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;

    //for a box with a white border, but no internal lines
    case 4:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_BLACK);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_BLACK);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_BLACK);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_BLACK);
      }
      //draw top face (just the back two lines)
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_WHITE);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_WHITE);
      //draw bottom face (just the front two lines)
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_WHITE);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_WHITE);
      //draw vertical edges (just the two edges)
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_WHITE);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_WHITE);
      break;
    //this one is obscure, but basically a solid white box with a black edge (helps it stand out over white things)
    case 5:
      //clearing out background
      for(int i = 0; i<=height; i ++){
        display.drawLine(point[0][0],point[0][1]+i,point[1][0],point[1][1]+i,SSD1306_WHITE);
        display.drawLine(point[1][0],point[1][1]+i,point[2][0],point[2][1]+i,SSD1306_WHITE);
        display.drawLine(point[2][0],point[2][1]+i,point[3][0],point[3][1]+i,SSD1306_WHITE);
        display.drawLine(point[3][0],point[3][1]+i,point[0][0],point[0][1]+i,SSD1306_WHITE);
      }
      //draw top face (just the back two lines)
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],SSD1306_BLACK);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],SSD1306_BLACK);
      //draw bottom face (just the front two lines)
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,SSD1306_BLACK);
      //draw vertical edges (just the two edges)
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,SSD1306_BLACK);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,SSD1306_BLACK);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 6:
      //draw top face
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],SSD1306_BLACK);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],SSD1306_BLACK);
      //draw bottom face
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,SSD1306_BLACK);
      break;
    //transparent box (wireframe) with inverted color
    case 7:
      //draw top face
      display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],2);
      display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],2);
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      display.drawLine(point[0][0],point[0][1]+height,point[1][0],point[1][1]+height,2);
      display.drawLine(point[1][0],point[1][1]+height,point[2][0],point[2][1]+height,2);
      display.drawLine(point[2][0],point[2][1]+height,point[3][0],point[3][1]+height,2);
      display.drawLine(point[3][0],point[3][1]+height,point[0][0],point[0][1]+height,2);
      //draw vertical edges
      display.drawLine(point[0][0],point[0][1],point[0][0],point[0][1]+height,2);
      display.drawLine(point[1][0],point[1][1],point[1][0],point[1][1]+height,2);
      display.drawLine(point[2][0],point[2][1],point[2][0],point[2][1]+height,2);
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
    //a black wireframe box, with only the internal 3 wires (used for quantcubes)
    case 8:
      //draw top face
      display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],2);
      display.drawLine(point[3][0],point[3][1],point[0][0],point[0][1],2);
      //draw bottom face
      display.drawLine(point[3][0],point[3][1],point[3][0],point[3][1]+height,2);
      break;
  }
}

void drawEllipse(uint8_t h, uint8_t k, int a, int b, uint16_t c) {
  //centerX = h
  //centerY = k
  //horizontal radius = a
  //vertical radius = b
  int y1;
  //if the ellipse is secretely a circle
  if (a == b) {
    display.drawCircle(h, k, a, c);
  }
  //for every x that falls along the length of the ellipse, get a y and draw a point
  else if (a > 0 && b > 0) {
    for (int x1 = h - a; x1 <= h + a; x1++) {
      if (x1 < screenWidth && x1 >= 0) {
        int root = b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        y1 = k + root;
        if (y1 < screenHeight) {
          display.drawPixel(x1, y1, c);
        }
        y1 = k - root;
        if (y1 >= 0) {
          display.drawPixel(x1, y1, c);
        }
      }
    }
  }
}


void drawBracket(uint8_t h, uint8_t k, uint8_t height, uint8_t width, uint8_t style, uint16_t c){
  int16_t point[4][2] = 
  {{int16_t(h-width/2),int16_t(k-height)},
   {int16_t(h-width/2),k},
   {int16_t(h+width/2),k},
   {int16_t(h+width/2),int16_t(k-height)}};
  //horizontal bracket facing up
  if(style == 0){
   //left side
   display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //bottom
   display.drawLine(point[1][0],point[1][1],point[2][0],point[2][1],c);
   //right side
    display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
  //horizontal bracket facing down
  if(style == 1){
   //left side
   display.drawLine(point[0][0],point[0][1],point[1][0],point[1][1],c);
   //top
   display.drawLine(point[0][0],point[0][1],point[3][0],point[3][1],c);
   //right side
    display.drawLine(point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}
void drawTetra(uint8_t h, uint8_t k, uint8_t height, uint8_t width, int xDepth, int yDepth, uint8_t style, uint16_t c){
  //upside down, and transparent
  if(style == 0){
    int point[4][2] =
   {{h,k+height/2},
    {h-width/2,k-height/2},
    {h+width/2,k-height/2},
    {h-xDepth,k-yDepth}};
    for(int i = 0; i<4; i++){
      while(point[i][0]<0){
        point[i][0]++;
      }
      while(point[i][1]<0){
        point[i][1]++;
      }
    }
    //draw front triangle
    display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[2][0],point[2][1],c);
    //draw left triangle
    display.drawTriangle(point[0][0],point[0][1],point[1][0],point[1][1],point[3][0],point[3][1],c);
    //draw right triangle
    display.drawTriangle(point[0][0],point[0][1],point[2][0],point[2][1],point[3][0],point[3][1],c);
  }
}

void fillAroundCircle(float x, float y, float r, uint16_t c){
  for(float i = 0; i<(x-r+10); i++){
    display.drawCircle(x,y,r+i,c);
  }
}

void drawMoon(int phase, bool forward){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  // phase*=scale;
  int r = screenHeight/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase<0)
    phase = 0;
  //controls whether or not the moon is waxing or waning
  if(forward){
    if(phase>0)
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    fillEllipse(screenWidth-2*r+phase/2,yPos,r,r+phase,SSD1306_BLACK);//filling earth's shadow
    if(phase>58 && phase < 88)
      fillEllipse(xPos,yPos,phase-58,r,SSD1306_WHITE);//filling earth's shadow
    if(phase>=88){
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);
    }
    else{
      fillAroundCircle(63,32,r+1,SSD1306_BLACK);
    }
    display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
  else if(!forward){
    if(phase == 0)
      display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
    else{
      if(phase<88)
        fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
      if(phase>58 && phase<88)
        fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
      if(phase>=88){
        display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
      }
      else{
        fillAroundCircle(63,31,r,SSD1306_BLACK);
        fillAroundCircle(63,32,r+1,SSD1306_BLACK);
      }
      display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
    }
  }
}
void drawMoon_reverse(int phase){
  //so it's scaled correctly
  // float scale = float(88)/float(100);
  //  phase*=scale;
  int r = screenHeight/2-1;
  int xPos = 63;
  int yPos = 31;
  if(phase>88)
    phase = 88;
  if(phase == 0)
    display.fillCircle(xPos,yPos,r,SSD1306_WHITE);//filling the moon
  else{
    if(phase<88)
      fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
    if(phase>58)
      fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
    if(phase>=88){
      display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
    }
    else{
      fillAroundCircle(63,31,r,SSD1306_BLACK);
    }
    display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
}

void printChannel(uint8_t xPos, uint8_t yPos, uint8_t channel, bool withBox){
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_WHITE);
  }
  printSmall(xPos,yPos,"Channel:"+stringify(channel),SSD1306_WHITE);
}

void drawProgBar(String text, float progress){
  display.setCursor(screenWidth-text.length()*10,screenHeight/2-8);
  display.setTextSize(2);
  display.print(text);
  display.setTextSize(1);
  // display.fillRect(32,screenHeight/2+10,64,8,SSD1306_BLACK);//clearing out the progress bar
  display.drawRect(32,screenHeight/2+10,64,8,SSD1306_WHITE);//drawing outline
  display.fillRect(32,screenHeight/2+10,64*progress,8,SSD1306_WHITE);//filling it
  display.display();
}

void drawCurlyBracket(uint8_t x1, uint8_t y1, uint8_t length, uint8_t height, bool start, bool end, uint8_t rotation){
  switch(rotation){
    //down
    case 0:
      drawCurlyBracket(x1,y1,length,height,start,end);
      break;
    //up
    case 1:
      display.setRotation(UPSIDEDOWN);
      drawCurlyBracket(screenWidth-x1,screenHeight-y1,length,height,start,end);
      display.setRotation(UPRIGHT);
      break;
    //right
    case 2:
      display.setRotation(1);
      drawCurlyBracket(y1,x1,height,length,start,end);
      display.setRotation(UPRIGHT);
      break;
  }
}

void drawCurlyBracket(int16_t x1, uint8_t y1, uint8_t length,uint8_t height,bool start, bool end){
  //left leg
  if(start){
    display.drawFastVLine(x1,y1-height,height,SSD1306_WHITE);
    display.drawPixel(x1+1,y1-height-1,SSD1306_WHITE);
    //left top
    display.drawFastHLine(x1+2,y1-height-2,length/2-3,SSD1306_WHITE);
  }
  //if start is out of view
  else{
     display.drawFastHLine(trackDisplay,y1-height-2,length/2-3-(viewStart-x1),SSD1306_WHITE);
  }
  //middle
  display.drawPixel(x1+length/2-1,y1-height-1,SSD1306_WHITE);
  display.drawFastVLine(x1+length/2,y1-height-4,3,SSD1306_WHITE);
  display.drawPixel(x1+length/2+1,y1-height-1,SSD1306_WHITE);
  //right top
  display.drawFastHLine(x1+length/2+2,y1-height-2,length/2-3,SSD1306_WHITE);
  //right leg
  display.drawPixel(x1+length-1,y1-height-1,SSD1306_WHITE);
  display.drawFastVLine(x1+length,y1-height,height,SSD1306_WHITE);
}


void bootscreen(){
  uint16_t frameCount = 0;
  display.setTextColor(SSD1306_WHITE);
  char child[5] = {'c','h','i','l','d'};
  int16_t xCoord;
  int16_t yCoord;

  uint8_t xOffset = 30;
  uint8_t yOffset = 15;
  WireFrame pram = makePram();
  pram.yPos = 40;
  pram.xPos = 64;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<60){
    display.clearDisplay();
    display.setTextSize(2);
    pram.render();
    //CHILD
    for(uint8_t letter = 0; letter<5; letter++){
      xCoord = 20+letter*8;
      yCoord = screenHeight-frameCount*10+letter*10;
      if(yCoord<0)
        yCoord = 0;
      yCoord+=yOffset;
      display.setCursor(xCoord,yCoord);
      printItalic(xCoord,yCoord,child[letter],1);
    }
    //OS
    if(frameCount>20){
      display.setTextSize(1);
      display.setFont(&FreeSerifItalic12pt7b);
      display.setCursor(xCoord+10,yCoord+5);
      display.print("OS");
      display.setFont();

      drawStar(xOffset+68,yOffset-8,3,7,5);
    }
    printSmall(0,58,"v0.1",1);
    display.display();
    pram.rotate(5,1);
    writeLEDs(uint8_t(0),uint8_t(2*frameCount/15));
    frameCount+=4;
  }
  turnOffLEDs();
}

void helloChild_1(){
  display.clearDisplay();
  display.setCursor(50, 20);
  // display.setTextSize(3.5);
  display.setTextColor(SSD1306_WHITE);
  // display.setFont(&FreeSerifBoldItalic18pt7b);
  display.print("hello");
  display.display();
  delay(1000);
  display.setCursor(10, 42);
  display.setFont();
  display.print("child.");
  display.invertDisplay(true);
  display.display();
  delay(1000);
  display.invertDisplay(false);

  display.setTextColor(SSD1306_WHITE);
  display.setCursor(50, 20);
  // display.setFont(&FreeSerifBoldItalic18pt7b);
  display.print("hello");
  display.setCursor(10, 42);
  display.setFont();
  display.print("child.");

  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1500);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
  // for(int i = 0; i<20; i++){
  //   display.drawBitmap(16+i*10,i*10,carriage_bmp,14,15,SSD1306_BLACK);
  //   display.display();
  //   delay(30);
  // }
}
void helloChild_2(){
  display.clearDisplay();
  display.invertDisplay(true);
  display.display();
  delay(1500);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  display.invertDisplay(false);
  delay(1500);
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.fillRect(0,0,128,64,SSD1306_BLACK);
  display.drawBitmap(50,25,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
}
void helloChild_3(){
  display.clearDisplay();
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1500);
}
void helloChild_4(){
  display.clearDisplay();
  display.drawBitmap(0,16,eyes_bmp,128,38,SSD1306_WHITE);
  display.display();
  delay(1000);
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_BLACK);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(500);
  for(float i = 0; i<20; i++){
    display.clearDisplay();
    display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(1000);
}
void helloChild_5(){
  display.clearDisplay();
  display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
  display.display();
  delay(1000);
  for(float i = 0; i<20; i++){
    display.clearDisplay();
    display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    display.display();
    delay(10);
  }
  delay(500);
}
void drawLogo(uint8_t x1, uint8_t y1){
  display.setTextSize(2);
  printItalic(x1,y1,"child",1);
  x1 += 42;
  y1 += 5;
  display.setTextSize(1);
  display.setFont(&FreeSerifItalic12pt7b);
  display.setCursor(x1,y1);
  display.print("OS");
  display.setFont();
}
void bootscreen_2(){
  uint16_t frameCount = 0;
  display.setTextColor(SSD1306_WHITE);
  int16_t xCoord;
  int16_t yCoord;

  uint8_t xOffset = 30;
  uint8_t yOffset = 15;
  //each letter pops in and swings into place with a x^2 parabolic motion
  while(frameCount<50){
    display.clearDisplay();
    display.setTextSize(2);
    xCoord = 20;
    yCoord = 20;
    printItalic(xCoord,yCoord,"child",1);
    xCoord = 52;
    display.setTextSize(1);
    display.setFont(&FreeSerifItalic12pt7b);
    display.setCursor(xCoord+10,yCoord+5);
    display.print("OS");
    display.setFont();
    // printCursive(xCoord,yCoord,"child",1);
    //OS
    if(frameCount>20){
      drawStar(xOffset+68,yOffset-8,3,7,5);
      uint8_t i = frameCount-21;
      display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
    }
    display.display();
    writeLEDs(uint8_t(0),uint8_t(2*frameCount/15));
    frameCount+=2;
  }
  turnOffLEDs();
}

void drawOSScreen(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  clearButtons();
  lastTime = millis();
  while(true){
    if(itsbeen(200)&&anyActiveInputs()){
      clearButtons();
      lastTime = millis();
      return;
    }
  }
}