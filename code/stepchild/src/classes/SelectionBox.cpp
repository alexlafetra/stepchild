#include "Stepchild.h"
#include "StepchildGraphics.h"
extern StepchildGraphics graphics;
using namespace std;

void SelectionBox::render(SequenceRenderSettings& settings){
  coords.end = Coordinate(stepchild->cursorPos,stepchild->activeTrack);

  unsigned short int startX;
  unsigned short int startY;
  unsigned short int len;
  unsigned short int height;

  unsigned short int X1;
  unsigned short int X2;
  unsigned short int Y1;
  unsigned short int Y2;

  if(this->coords.start.x>this->coords.end.x){
    X1 = this->coords.end.x;
    X2 = this->coords.start.x;
  }
  else{
    X1 = this->coords.start.x;
    X2 = this->coords.end.x;
  }
  if(this->coords.start.y>this->coords.end.y){
    Y1 = this->coords.end.y;
    Y2 = this->coords.start.y;
  }
  else{
    Y1 = this->coords.start.y;
    Y2 = this->coords.end.y;
  }

  startX = stepchild->TRACK_LABEL_WIDTH+(X1-stepchild->viewStart)*stepchild->viewScale;
  len = (X2-X1)*stepchild->viewScale;

  //if box starts before view
  if(X1<stepchild->viewStart){
    startX = stepchild->TRACK_LABEL_WIDTH;//box is drawn from beggining, to this->coords.end.x
    len = (X2-stepchild->viewStart)*stepchild->viewScale;
  }
  //if box ends past view
  if(X2>stepchild->viewEnd){
    len = (stepchild->viewEnd-X1)*stepchild->viewScale;
  }

  //same, but for tracks
  startY = (Y1-stepchild->startTrack)*stepchild->trackHeight+settings.startHeight;
  height = ((Y2+1-stepchild->startTrack)*stepchild->trackHeight)+settings.startHeight - startY;
  if(Y1<stepchild->startTrack){
    startY = settings.startHeight;
    height = ((Y2 - stepchild->startTrack + 1)*stepchild->trackHeight - startY)%(stepchild->SCREEN_HEIGHT-settings.startHeight) + settings.startHeight;
  }
  stepchild->display.drawRect(startX, startY, len, height, SSD1306_WHITE);
  stepchild->display.drawRect(startX+1, startY+1, len-2, height-2, SSD1306_WHITE);

  if(len>5 && height>=stepchild->trackHeight){
    stepchild->display.fillRect(startX+2,startY+2, len-4, height-4, SSD1306_BLACK);
    graphics.shadeArea(startX+2,startY+2, len-4, height-4,5);
  }
}


void SelectionBox::select(){
  if(coords.start.x>coords.end.x){
    Timestep x1_old = coords.start.x;
    coords.start.x = coords.end.x;
    coords.end.x = x1_old;
  }
  if(coords.start.y>coords.end.y){
    Timestep y1_old = coords.start.y;
    coords.start.y = coords.end.y;
    coords.end.y = y1_old;
  }
  for(uint16_t track = coords.start.y; track<=coords.end.y; track++){
    // < and not <= so it doesn't grab trailing notes
    for(Timestep time = coords.start.x; time<coords.end.x; time++){
      if(stepchild->lookupTable[track][time] != 0){
        //this is a little inconsistent with how select usually works, but it allows whatever's in the box to DEFINITELY be selected.
        //it makes sense (a little) because it seems rare that you would ever need to deselect notes using the box
        stepchild->selectNote(track, stepchild->lookupTable[track][time]);
        time = stepchild->noteData[track][stepchild->lookupTable[track][time]].endPos-1;
      }
    }
  }
}