//Holds all the graphics functions
//Accessed thru Stepchild.graphics.function();
class StepchildGraphics{
  public:
  StepchildGraphics(){}
  //Shades a rectangle
  void shadeArea(int16_t x1, int16_t y1, int16_t len, int16_t height, uint8_t shade){
    for(int j = 0; j<height; j++){
      for(int i = 0;(i+j%shade)<len; i+=shade){
        if((i+j%shade)>=0 && (i+j%shade)<screenWidth){
          display.drawPixel(x1+i+j%shade,y1+j,1);
        }
        else{
          display.drawPixel(x1+i+j%shade,y1+j,0);
        }
      }
    }
  }
  void drawDottedLineH(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t dot){
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
  void drawDottedLineV(uint8_t x1, uint8_t y1, uint8_t y2, uint8_t dot){
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
    this->shadeArea(x1,y1,len,height,shade);
  }

  void drawCenteredBracket(int x1, int y1, int length, int height){
    this->drawNoteBracket(x1-length/2,y1-height/2,length, height, false);
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
    this->drawDottedLineH(x1,x1+w,y1,dot);
    //bottom
    this->drawDottedLineH(x1,x1+w,y1+h,dot);
    //left
    this->drawDottedLineV(x1,y1,y1+h,dot);
    //right
    this->drawDottedLineV(x1+w,y1,y1+h,dot);
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
      this->printFraction(x1+whole.length()*6, y1-3, num, denom);
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
      this->printFraction(x2+whole.length()*6, y1-3, num, denom);
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
      this->printFraction(x1+whole.length()*4, y1-3, num, denom);
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
      this->printFraction(x1-length*2+whole.length()*4, y1-3, num, denom);
    }
    printSmall(x1-length*2,y1,whole,SSD1306_WHITE);
  }
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
      this->drawRadian(h,k,a,b,i,c);
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
  void drawStar(uint8_t centerX, uint8_t centerY, uint8_t r1, uint8_t r2, uint8_t points){
    uint8_t numberOfPoints = points*2;//the actual number of points (both convex and concave vertices)
    Coordinate coords[numberOfPoints];
    for(uint8_t pt = 0; pt<numberOfPoints; pt++){
      Coordinate pair;
      //if it's odd, it's a convex point
      if(!(pt%2))
        pair = utils.getRadian(centerX, centerY, r1, r1, pt*360/numberOfPoints);
      else
        pair = utils.getRadian(centerX, centerY, r2, r2, pt*360/numberOfPoints);
      coords[pt] = pair;
    }
    for(uint8_t pt = 0; pt<numberOfPoints; pt++){
      if(pt == numberOfPoints-1){
        display.drawLine(coords[pt].x,coords[pt].y,coords[0].x,coords[0].y,SSD1306_WHITE);
      }
      else{
        display.drawLine(coords[pt].x,coords[pt].y,coords[pt+1].x,coords[pt+1].y,SSD1306_WHITE);
      }
    }
  }
  void printChannel(uint8_t xPos, uint8_t yPos, uint8_t channel, bool withBox){
    if(withBox){
      display.fillRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_BLACK);
      display.drawRect(xPos-2,yPos-2,4*(8+stringify(channel).length())+3,9,SSD1306_WHITE);
    }
    printSmall(xPos,yPos,"Channel:"+stringify(channel),SSD1306_WHITE);
  }
  //draws a horizontal bar graph -- so far totally unused
  void drawBarGraphH(int xStart, int yStart, int thickness, int length, float progress){
    float percentage = progress*length;
    display.fillRect(xStart,yStart,length,thickness,SSD1306_BLACK);
    display.drawRect(xStart,yStart,length,thickness,SSD1306_WHITE);
    display.fillRect(xStart,yStart,percentage,thickness,SSD1306_WHITE);
    display.drawRect(xStart+1,yStart+1,length-2,thickness-2,SSD1306_BLACK);
  }
  void drawBarGraphV(uint8_t xStart, uint8_t yStart, uint8_t thickness, uint8_t height, float progress){
    float percentFull = progress*height;
    display.fillRect(xStart,yStart,thickness,height,0);
    display.drawRect(xStart,yStart,thickness,height,1);
    display.fillRect(xStart,yStart+height-percentFull,thickness,percentFull,1);
    display.drawRect(xStart+1,yStart+1,thickness-2,height-2,0);

  }
  //Draws percentage bar showing how full the sequence is
  void drawSequenceMemoryBar(uint8_t x1, uint8_t y1, uint8_t length){
    float free = rp2040.getFreeHeap();
    float total = rp2040.getTotalHeap();
    float percentageUsed = 1.0 - free/total;
    // display.fillRoundRect(x1-14,y1-2,length+16,9,3,0);
    // display.drawRoundRect(x1-14,y1-2,length+16,9,3,1);
    this->drawBarGraphH(x1,y1,5,length,percentageUsed);
    printSmall(x1-12,y1,"mem",1);
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
    this->drawArrow(pointX,pointY,size+2,direction,true);
    switch(direction){
      case RIGHT:
        this->drawArrow(pointX-1,pointY,size,direction,false);
        break;
      case LEFT:
        this->drawArrow(pointX+1,pointY,size,direction,false);
        break;
      case UP:
        this->drawArrow(pointX,pointY+1,size,direction,false);
        break;
      case DOWN:
        this->drawArrow(pointX,pointY-1,size,direction,false);
        break;
    }
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
    if(x1>=sequence.viewStart){
      //topL
      display.drawLine(x1-2-offset,y1-2-offset,x1+1-offset,y1-2-offset,SSD1306_WHITE);
      display.drawLine(x1-2-offset,y1-2-offset,x1-2-offset,y1+1-offset,SSD1306_WHITE);
      //bottomL
      display.drawLine(x1-2-offset,y1+height+2+offset,x1+1-offset,y1+height+2+offset,SSD1306_WHITE);
      display.drawLine(x1-2-offset,y1+height+2+offset,x1-2-offset,y1+height-1+offset,SSD1306_WHITE);
    }
    if(x1+length<=sequence.viewEnd){
      //topR
      display.drawLine(x1+length+2+offset,y1-2-offset,x1+length-1+offset,y1-2-offset,SSD1306_WHITE);
      display.drawLine(x1+length+2+offset,y1-2-offset,x1+length+2+offset,y1+1-offset,SSD1306_WHITE);
      //bottomR
      display.drawLine(x1+length+2+offset,y1+height+2+offset,x1+length-1+offset,y1+height+2+offset,SSD1306_WHITE);
      display.drawLine(x1+length+2+offset,y1+height+2+offset,x1+length+2+offset,y1+height-1+offset,SSD1306_WHITE);
    }
  }

  void drawNoteBracket(int x1, int y1, int length, int height){
    this->drawNoteBracket(x1, y1, length, height, true);
  }

  void drawNoteBracket(Note note, int track){
    this->drawNoteBracket(trackDisplay+(note.startPos-sequence.viewStart)*sequence.viewScale,headerHeight+(track-startTrack)*trackHeight,(note.endPos-note.startPos+1)*sequence.viewScale,trackHeight);
  }

  void drawSelectionBracket(){
    vector<uint16_t> bounds  = getSelectedNotesBoundingBox();
    //if the left side is in view
    if(bounds[0]>=sequence.viewStart){
      //if the top L corner is in view
      uint8_t x1 = (bounds[0]-sequence.viewStart)*sequence.viewScale+trackDisplay-((millis()/200)%2);
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
    if(bounds[2]<sequence.viewEnd){
      uint8_t x1 = (bounds[2]-sequence.viewStart)*sequence.viewScale+trackDisplay+((millis()/200)%2)+1;
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

  void drawBanner(int8_t x1, int8_t y1, String text){
    display.drawBitmap(x1-13,y1-4,bannerL_bmp,12,9,SSD1306_WHITE);
    // display.drawBitmap(x1+text.length()*4-countSpaces(text)*2,y1,bannerR_bmp,11,9,SSD1306_WHITE);
    display.setRotation(UPSIDEDOWN);
    display.drawBitmap(screenWidth-(x1+text.length()*4-countSpaces(text)*2+countChar(text,'#')*2)-12,screenHeight-y1-9,bannerL_bmp,12,9,SSD1306_WHITE);
    display.setRotation(UPRIGHT);
    display.fillRect(x1-1,y1-1,text.length()*4-countSpaces(text)*2+countChar(text,'#')*2+1,7,SSD1306_WHITE);
    printSmall(x1,y1,text,SSD1306_BLACK);
  }

  void drawCenteredBanner(int8_t x1, int8_t y1, String text){
    uint8_t len = text.length()*4-countSpaces(text)*2+countChar(text,'#')*2;
    x1-=len/2;
    this->drawBanner(x1,y1,text);
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
  //draws a pendulum where "val" is a the angle of the pendulum
  void drawPendulum(int16_t x2, int16_t y2, int8_t length, float val){
    this->drawPendulum(x2,y2,length,val,3);
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
      this->drawLabel_outline(x1,y1,text);
    }
  }
};

StepchildGraphics graphics;

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
  drawFullKeyBed(16,pressList,mask,activeKey,octave);
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
    //pixel indicating it's in the chord
    if(isInVector(wKeyPattern[i]+12*octave,pressList))
      display.drawPixel(i*(keyWidth+1)+2,y1+15,SSD1306_WHITE);
    //draw cursor indicator
    if(wKeyPattern[i]+12*octave == activeKey){
      graphics.drawArrow(i*(keyWidth+1)+2,y1+17+((millis()/200)%2),3,2,true);
      printSmall(i*(keyWidth+1)+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
    }

    //if there's no mask, or if the key is in the mask
    if(mask.size() == 0 || isInVector(wKeyPattern[i]+12*octave,mask)){
      //if it's pressed, draw it blinking
      if(isInVector(wKeyPattern[i]+12*octave,pressList)){
        if(millis()%800>400)
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        else
          display.fillRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey)
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
      }
      else{
        //if it's highlighted
        if(wKeyPattern[i]+12*octave == activeKey)
          display.drawRect(i*(keyWidth+1),y1-1,keyWidth,wKeyHeight+1,SSD1306_WHITE);
        else
          display.fillRect(i*(keyWidth+1),y1,keyWidth,wKeyHeight,SSD1306_WHITE);
      }
    }
  }
  //then draw black keys
  uint8_t xPos = 3;
  for(uint8_t i = 0; i<15; i++){
    //pixel indicating it's in the chord
    if(isInVector(bKeyPattern[i]+12*octave,pressList))
      display.drawPixel(xPos+2,y1+15,SSD1306_WHITE);
    //draw cursor indicator
    if(bKeyPattern[i]+12*octave == activeKey){
      graphics.drawArrow(xPos+2,y1+17+((millis()/200)%2),3,2,false);
      printSmall(xPos+2-text.length()*2,y1+22+((millis()/200)%2),text,SSD1306_WHITE);
    }

    if(mask.size() == 0 || isInVector(bKeyPattern[i]+12*octave,mask)){
      if(isInVector(bKeyPattern[i]+12*octave,pressList) || bKeyPattern[i]+12*octave == activeKey){
        display.fillRect(xPos,y1,keyWidth,bKeyHeight,SSD1306_BLACK);
        //if it's pressed
        if(isInVector(bKeyPattern[i]+12*octave,pressList)){
          if(millis()%800>400){
            display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
          }
        }
        else
          display.fillRect(xPos+1,y1,keyWidth-2,bKeyHeight-1,SSD1306_WHITE);
        display.drawRect(xPos-1,y1-1,keyWidth+2,bKeyHeight+2,SSD1306_WHITE);

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

void drawPram(uint8_t x1, uint8_t y1){
  if(sequenceClock.onBeat(2,30))
    display.drawBitmap(x1,y1+1,carriage_bmp,14,15,SSD1306_WHITE);
  else
    display.drawBitmap(x1,y1,carriage_bmp,14,15,SSD1306_WHITE);
}

void drawPlayIcon(int8_t x1, int8_t y1){
  if(clockSource == EXTERNAL_CLOCK && !gotClock){
    display.drawTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
  else{
    display.fillTriangle(x1,y1+6,x1,y1,x1+6,y1+3,SSD1306_WHITE);
  }
}

//draws/fills a triangle circumscribed by a circle w/ radius r
//super janky
void fillCircumscribedTriangle(int8_t x1, int8_t y1, uint8_t r){
  Coordinate A = Coordinate(x1,y1-r);
  uint8_t a = r/2;
  float b = r*0.866;
  Coordinate B = Coordinate(x1-b,y1+a);
  Coordinate C = Coordinate(x1+b,y1+a);
  display.fillTriangle(A.x,A.y,B.x,B.y,C.x,C.y,1);
}

void drawPower(uint8_t x1, uint8_t y1){
  //check if USB is plugged in
  bool usb = digitalRead(USB_PIN);
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
      if(sequence.lookupTable[track][sequence.noteData[track][id].endPos] != 0)
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
  String s = sequence.trackData[trackID].getPitchAndOctave()+stringify(sequence.trackData[trackID].channel)+((sequence.trackData[trackID].noteLastSent != 255)?"$":"");
  uint8_t offset = printPitch(xCoord, yCoord, s, bigOct, channel, c);
  offset+=4;
  //if you want to show the track channel
  if(controls.SHIFT() || (menuIsActive && activeMenu.menuTitle == "TRK")){
    String sx = ":";
    sx += stringify(sequence.trackData[trackID].channel);
    // if(sequence.trackData[trackID].isLatched){
    //   sx += "<";
    // }
    // display.drawBitmap(xCoord+offset+2,yCoord,ch_tiny,6,3,1);
    printSmall(xCoord+offset,yCoord,sx,1);
    offset+=sx.length()*4;
  }
  //if you want to show the track "primed" status for recording
  if(recording && sequence.trackData[trackID].isPrimed){
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
    graphics.fillEllipse(screenWidth-2*r+phase/2,yPos,r,r+phase,SSD1306_BLACK);//filling earth's shadow
    if(phase>58 && phase < 88)
      graphics.fillEllipse(xPos,yPos,phase-58,r,SSD1306_WHITE);//filling earth's shadow
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
        graphics.fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
      if(phase>58 && phase<88)
        graphics.fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
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
      graphics.fillEllipse(screenWidth-2*r+phase/2-4,yPos-2,r+1,r+phase+1,SSD1306_WHITE);//filling earth's shadow
    if(phase>58)
      graphics.fillEllipse(xPos,yPos,phase-58,r,SSD1306_BLACK);//filling earth's shadow
    if(phase>=88){
      display.fillCircle(xPos,yPos,r,SSD1306_BLACK);
    }
    else{
      fillAroundCircle(63,31,r,SSD1306_BLACK);
    }
    display.drawCircle(xPos,yPos,r,SSD1306_WHITE);//drawing the moon's outline
  }
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
  while(frameCount<64){
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

      graphics.drawStar(xOffset+68,yOffset-8,3,7,5);
    }
    printSmall(0,58,"v0.1",1);
    display.display();
    pram.rotate(5,1);
    // writeLEDs(uint8_t(0),uint8_t(2*frameCount/15));
    uint16_t ledData = 0b1111111111111111;
    ledData>>(16-frameCount/8);
    controls.writeLEDs(ledData);
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
  while(frameCount<128){
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
    if(frameCount>40){
      graphics.drawStar(xOffset+68,yOffset-8,3,7,5);
      uint8_t i = frameCount-42;
      display.drawBitmap(48-i/3*i/3,32+i/3*i/3-i/3,carriage_bmp,14,15,SSD1306_WHITE);
    }
    else{
      display.drawBitmap(48,32,carriage_bmp,14,15,SSD1306_WHITE);
    }
    display.display();
    // writeLEDs(uint8_t(0),uint8_t(2*frameCount/15));
    controls.writeLEDs(0b1111111111111111>>(16-(frameCount/8)));
    frameCount+=2;
  }
  turnOffLEDs();
}

void drawOSScreen(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  controls.clearButtons();
  lastTime = millis();
  while(true){
    if(utils.itsbeen(200)&&controls.anyActiveInputs()){
      controls.clearButtons();
      lastTime = millis();
      return;
    }
  }
}

