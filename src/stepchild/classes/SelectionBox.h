//Holds coordinates and a flag set when the SelectionBox has been started
class SelectionBox{
  public:
  uint16_t x1;
  uint16_t y1;
  uint16_t x2;
  uint16_t y2;
  bool begun;
  SelectionBox();
  void displaySelBox();
};

SelectionBox::SelectionBox(){
  x1 = 0;
  y1 = 0;
  x2 = 0;
  y2 = 0;
  begun = false;
}

void SelectionBox::displaySelBox(){
  x2 = cursorPos;
  y2 = activeTrack;

  unsigned short int startX;
  unsigned short int startY;
  unsigned short int len;
  unsigned short int height;

  unsigned short int X1;
  unsigned short int X2;
  unsigned short int Y1;
  unsigned short int Y2;

  if(x1>x2){
    X1 = x2;
    X2 = x1;
  }
  else{
    X1 = x1;
    X2 = x2;
  }
  if(y1>y2){
    Y1 = y2;
    Y2 = y1;
  }
  else{
    Y1 = y1;
    Y2 = y2;
  }

  startX = trackDisplay+(X1-viewStart)*scale;
  len = (X2-X1)*scale;

  //if box starts before view
  if(X1<viewStart){
    startX = trackDisplay;//box is drawn from beggining, to x2
    len = (X2-viewStart)*scale;
  }
  //if box ends past view
  if(X2>viewEnd){
    len = (viewEnd-X1)*scale;
  }

  //same, but for tracks
  uint8_t startHeight = (menuIsActive||maxTracksShown==5)?headerHeight:8;
  startY = (Y1-startTrack)*trackHeight+startHeight;
  height = ((Y2+1-startTrack)*trackHeight)+startHeight - startY;
  if(Y1<startTrack){
    startY = startHeight;
    height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
  }
  display.drawRect(startX, startY, len, height, SSD1306_WHITE);
  display.drawRect(startX+1, startY+1, len-2, height-2, SSD1306_WHITE);

  if(len>5 && height>=trackHeight){
    display.fillRect(startX+2,startY+2, len-4, height-4, SSD1306_BLACK);
    shadeArea(startX+2,startY+2, len-4, height-4,10);
  }
}

SelectionBox selBox;