//Specialized functions for drawing geometries

void shadeArea(unsigned short int x1, unsigned short int y1, unsigned short int len, unsigned char height, unsigned char shade){
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
#ifndef HEADLESS
  uint16_t num = numerator.toInt();
  uint16_t denom = denominator.toInt();
#else
    uint16_t num = stoi(numerator);
    uint16_t denom = stoi(denominator);
#endif
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
#ifndef HEADLESS
  uint16_t num = numerator.toInt();
  uint16_t denom = denominator.toInt();
#else
    uint16_t num = stoi(numerator);
    uint16_t denom = stoi(denominator);
#endif

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
    
//#ifndef HEADLESS
  uint16_t num = toInt(numerator);
  uint16_t denom = toInt(denominator);
//#else
//    uint16_t num = stoi(numerator);
//    uint16_t denom = stoi(denominator);
//#endif
    
  length = whole.length();
  //if there's actually a numerator and a denominator
  if(num != 0 && denom != 0){
    length+=numerator.length()+denominator.length();
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
    offset = sin(millis()/200);
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
  drawNoteBracket(trackDisplay+(note.startPos-viewStart)*scale,debugHeight+(track-startTrack)*trackHeight,(note.endPos-note.startPos+1)*scale,trackHeight);
}

void drawSelectionBracket(){
  vector<uint16_t> bounds  = getSelectionBounds2();
  //if the left side is in view
  if(bounds[0]>=viewStart){
    //if the top L corner is in view
    uint8_t x1 = (bounds[0]-viewStart)*scale+trackDisplay-sin(millis()/100);
    if(bounds[1]>=startTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[1]-startTrack)*trackHeight+debugHeight-sin(millis()/100);
      display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1-1,x1+5,y1-1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      display.drawLine(x1-1,y1,x1-1,y1+5,SSD1306_WHITE);
    }
    //if the bottom L corner is in view
    if(bounds[3]<=endTrack){
      //y coord relative to the view
      uint8_t y1 = (bounds[3]-startTrack+1)*trackHeight+debugHeight+sin(millis()/100);
      display.drawLine(x1,y1,x1+5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1+1,x1+5,y1+1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      display.drawLine(x1-1,y1,x1-1,y1-5,SSD1306_WHITE);
    }
  }
  //if the right corner is in view
  if(bounds[2]<viewEnd){
    uint8_t x1 = (bounds[2]-viewStart)*scale+trackDisplay+sin(millis()/100)+1;
    //top R corner
    if(bounds[1]>=startTrack){
      uint8_t y1 = (bounds[1]-startTrack)*trackHeight+debugHeight-sin(millis()/100);
      display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1-1,x1-5,y1-1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1+5,SSD1306_WHITE);
      display.drawLine(x1+1,y1,x1+1,y1+5,SSD1306_WHITE);
    }
    //bottom R corner
    if(bounds[3]<=endTrack){
      uint8_t y1 = (bounds[3]-startTrack+1)*trackHeight+debugHeight+sin(millis()/100);
      display.drawLine(x1,y1,x1-5,y1,SSD1306_WHITE);
      display.drawLine(x1,y1+1,x1-5,y1+1,SSD1306_WHITE);
      display.drawLine(x1,y1,x1,y1-5,SSD1306_WHITE);
      display.drawLine(x1+1,y1,x1+1,y1-5,SSD1306_WHITE);
    }
  }
}
