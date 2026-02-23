#include "Stepchild.h"
#include "graphics/SequenceRenderSettings.h"
#include "StepchildGraphics.h"
#include "guiUtilities.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;
extern void drawSeq();

using namespace std;

//Functions that all 3 track menus use:
void drawTrackMenuTopInfo(uint8_t topL);

vector<uint8_t> selectMultipleTracks(String text){
  //clearing out the selected tracks
  for(uint8_t i = 0; i<stepchild.trackData.size(); i++){
    stepchild.trackData[i].setSelected(false);
  }
  //to hold the id's and return
  vector<uint8_t> selection = {};
  while(true){
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.trackSelection = true;
    settings.drawPram = false;
    settings.shrinkTopDisplay = true;
    settings.topLabels = false;
    graphics.drawSeq(settings);
    graphics.printSmall(stepchild.SCREEN_WIDTH-text.length()*4,0,text,1);

    graphics.printSmall(2,2,"Trk:"+stringify(stepchild.activeTrack+1),1);

    stepchild.display.display();
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickY == 1){
        stepchild.setActiveTrack(stepchild.activeTrack+1,true);
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == -1){
        stepchild.setActiveTrack(stepchild.activeTrack-1,true);
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.SELECT()  && !stepchild.buttons.SHIFT()){
        stepchild.buttons.setSELECT(false);
        stepchild.lastTime = millis();
        stepchild.trackData[stepchild.activeTrack].setSelected(!stepchild.trackData[stepchild.activeTrack].isSelected());
      }
      //toggle the selection on all of them
      if(stepchild.buttons.SHIFT() && stepchild.buttons.SELECT() ){
        stepchild.buttons.setNEW(false);
        stepchild.lastTime = millis();
        stepchild.trackData[stepchild.activeTrack].setSelected(!stepchild.trackData[stepchild.activeTrack].isSelected());
        for(int i = 0; i<stepchild.trackData.size(); i++){
          stepchild.trackData[i].setSelected(stepchild.trackData[stepchild.activeTrack].isSelected());
        }
      }
      if(stepchild.buttons.NEW() && !stepchild.buttons.SHIFT()){
        stepchild.buttons.setNEW(false);
        stepchild.lastTime = millis();
        //adding all the selected tracks to the list
        for(int i = 0; i<stepchild.trackData.size(); i++){
          if(stepchild.trackData[i].isSelected()){
            selection.push_back(i);
            stepchild.trackData[i].setSelected(false);
          }
        }
        return selection;
      }
      if(stepchild.buttons.DELETE() || stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
          stepchild.trackData[track].setSelected(false);
        }
        selection.clear();
        break;
      }
    }
  }
  return selection;
}

vector<uint8_t> selectMultipleTracks(){
  return selectMultipleTracks("");
}

void tuneTracksToScale(){
  selectKeysAnimation(true);
  vector<uint8_t> pitches = selectKeys(0);
  selectKeysAnimation(false);
  if(pitches.size() == 0){
    return;
  }
  vector<uint8_t> tracks = selectMultipleTracks("select tracks to tune");
  if(!tracks.size())
    return;
  bool allowDuplicates = binarySelectionBox(64,32,"no","ye","allow duplicate pitches?",drawSeq);
  for(uint8_t track = 0; track < tracks.size(); track++){
    stepchild.setTrackToNearestPitch(pitches, tracks[track], allowDuplicates);
  }
}


void sortTracks(){
  //0 = ascending, 1 = descending
  uint8_t sortType = 0;
  //0 = track pitch, 1 = track channel, 2 =  number of notes (wip)
  int8_t sortTarget = 0;
  Coordinate coords(stepchild.SCREEN_WIDTH-44,8);
  uint8_t cursor = 0;
  stepchild.tooltipText = "sort tracks";
  while(true){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        break;
      }
      if(stepchild.buttons.NEW() || (stepchild.buttons.SELECT() && cursor == 2)){
        stepchild.sortTrackData(sortType,sortTarget);
        stepchild.tooltipText = "sorted!";
        stepchild.lastTime = millis();
        return;
      }
      if(stepchild.buttons.joystickY == 1 && cursor<3){
        cursor++;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == -1 && cursor>0){
        cursor--;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickX){
        switch(cursor){
          case 0:
            sortTarget+=stepchild.buttons.joystickX;
            if(sortTarget<0)
              sortTarget = 2;
            if(sortTarget == 3)
              sortTarget = 0;
            break;
          case 1:
            sortType = !sortType;
            break;
        }
        stepchild.lastTime = millis();
      }
    }
    while(stepchild.buttons.counterA != 0){
      switch(cursor){
        case 0:
          sortTarget+=stepchild.buttons.counterA<0?-1:1;
          if(sortTarget<0)
            sortTarget = 2;
          if(sortTarget == 3)
            sortTarget = 0;
          break;
        case 1:
          sortType = !sortType;
          break;
      }
      stepchild.buttons.counterA+=stepchild.buttons.counterA<0?1:-1;
    }
    while(stepchild.buttons.counterB != 0){
      switch(cursor){
        case 0:
          sortTarget+=stepchild.buttons.counterB<0?-1:1;
          if(sortTarget<0)
            sortTarget = 2;
          if(sortTarget == 3)
            sortTarget = 0;
          break;
        case 1:
          sortType = !sortType;
          break;
      }
      stepchild.buttons.counterB+=stepchild.buttons.counterB<0?1:-1;
    }
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawTrackChannel = true;
    settings.shrinkTopDisplay = false;
    graphics.drawSeq(settings);
    stepchild.display.fillRoundRect(coords.x,coords.y,46,40,3,SSD1306_BLACK);
    stepchild.display.drawRoundRect(coords.x,coords.y,46,40,3,SSD1306_WHITE);
    // stepchild.display.fillRoundRect(x1,y1,85,43,3,0);
    // stepchild.display.drawRoundRect(x1,y1,85,43,3,1);
    // graphics.printSmall(x1+42,y1+3,"sort by:",1,ALIGN_CENTER);
    String target;
    switch(sortTarget){
      case 0:
        target = "pitch";
        break;
      case 1:
        target = "channel";
        break;
      case 2:
        target = "note count";
        break;
    }

    //sort by
    graphics.printSmall(coords.x+2,coords.y+2,"sort by:",1);
    if(cursor == 0){
      stepchild.display.fillRoundRect(coords.x+1,coords.y+9,graphics.getSmallTextLength(target)+2,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2,coords.y+12,3,ARROW_RIGHT,0);
    }
    graphics.printSmall(coords.x+2,coords.y+10,target,2);

    //order
    graphics.printSmall(coords.x+2,coords.y+17,"order:",1);
    String order = sortType?"descending":"ascending";
    if(cursor == 1){
      stepchild.display.fillRoundRect(coords.x+1,coords.y+24,graphics.getSmallTextLength(order)+2,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2,coords.y+27,3,ARROW_RIGHT,0);
    }
    graphics.printSmall(coords.x+2,coords.y+25,order,2);

    //commit
    if(cursor == 2){
      stepchild.display.fillRoundRect(coords.x+11,coords.y+31,22,7,2,1);
      graphics.drawArrow(coords.x+millis()/200%2+10,coords.y+34,3,ARROW_RIGHT,0);
    }
    graphics.printSmall(coords.x+12,coords.y+32,"sort!",2);
    stepchild.display.display();
  }
}
void swapTracks(){
  // slideMenuOut(1,7);
  unsigned short int track1 = stepchild.activeTrack;
  stepchild.buttons.setSELECT(false);
  while(true){
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickY == 1){
        stepchild.swapTracks(track1,stepchild.activeTrack+1);
        stepchild.setActiveTrack(stepchild.activeTrack+1,true);
        track1 = stepchild.activeTrack;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == -1){
        stepchild.swapTracks(track1,stepchild.activeTrack-1);
        stepchild.setActiveTrack(stepchild.activeTrack-1,true);
        track1 = stepchild.activeTrack;
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.NEW()){
        stepchild.lastTime = millis();
        sortTracks();
      }
      if(stepchild.buttons.SELECT()  || stepchild.buttons.MENU()){
        stepchild.buttons.setSELECT(false);
        stepchild.buttons.setMENU(false) ;
        stepchild.lastTime = millis();
        break;
      }
    }
    stepchild.buttons.readJoystick();
    stepchild.buttons.readButtons();
    stepchild.display.clearDisplay();
    SequenceRenderSettings settings;
    settings.topLabels = false;
    settings.drawLoopPoints = false;
    graphics.drawSeq(settings);
    stepchild.display.fillRect(70,0,16,16,0);
    if(millis()%1000 >= 500){
      stepchild.display.drawBitmap(70,0,arrow_1_bmp,16,16,SSD1306_WHITE);
    }
    else{
      stepchild.display.drawBitmap(70,0,arrow_3_bmp,16,16,SSD1306_WHITE);
    }
    stepchild.display.fillRoundRect(86,-2,44,9,3,0);
    stepchild.display.drawRoundRect(86,-2,44,9,3,1);
    graphics.printSmall(89,0,"[n] to sort",1);
    stepchild.display.display();
  }
}

/*
--------------------------------
      Track Menu
--------------------------------
*/

class TrackMenu:public StepchildMenu{
  public:
    uint8_t xCursor = 0;
    TrackMenu(){
      coords = CoordinatePair(94,0,129,65);
    }
    bool trackMenuControls();
    void displayMenu();
    void displayMenuSidebar();
    String getTooltipText();
};

void trackUtils(TrackMenu* t);

bool TrackMenu::trackMenuControls(){
  stepchild.buttons.readButtons();
  stepchild.buttons.readJoystick();
  //moving menu cursor
  if(!stepchild.buttons.SHIFT()){
    if(stepchild.itsbeen(100)){
      if(stepchild.buttons.joystickY != 0){
        if(stepchild.buttons.joystickY == 1){
          if(cursor<9 && cursor != 6){
            cursor++;
            stepchild.lastTime = millis();
          }
        }
        if(stepchild.buttons.joystickY == -1){
          if(cursor == 7){
            cursor = 3;
            stepchild.lastTime = millis();
          }
          else if(cursor>0){
            cursor--;
            stepchild.lastTime = millis();
          }
        }
      }
    }
  }
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.SHIFT() && stepchild.buttons.joystickY != 0){
      if(stepchild.buttons.joystickY == -1){
        stepchild.setActiveTrack(stepchild.activeTrack-1,true);
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.joystickY == 1){
        stepchild.setActiveTrack(stepchild.activeTrack+1,true);
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.PLAY()){
      stepchild.trackData[stepchild.activeTrack].setPrimed(!stepchild.trackData[stepchild.activeTrack].isPrimed());
      stepchild.lastTime = millis();
    }
    if(stepchild.buttons.SELECT() ){
      stepchild.lastTime = millis();
      switch(cursor){
        //tune
        case 0:
          tuneTracksToScale();
          stepchild.lastTime = millis();
          break;
        //midi route/channel
        case 2:
//          routeMenu();
          stepchild.lastTime = millis();
          break;
        case 3:
          stepchild.trackData[stepchild.activeTrack].setPrimed(!stepchild.trackData[stepchild.activeTrack].isPrimed());
          if(stepchild.buttons.SHIFT()){
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              stepchild.trackData[track].setPrimed(stepchild.trackData[stepchild.activeTrack].isPrimed());
            }
          }
          break;
        //edit
        case 4:
          trackUtils(this);
          stepchild.lastTime = millis();
          break;
        //mute
        case 5:
          if(stepchild.buttons.SHIFT()){
            stepchild.muteMultipleTracks(selectMultipleTracks(stringify("Mute which?")));
          }
          else{
           stepchild.toggleMute(stepchild.activeTrack);
          }
          stepchild.lastTime = millis();
          break;
        //move
        case 6:
          swapTracks();
          stepchild.lastTime = millis();
          break;
        //erase
        case 7:
          if(stepchild.buttons.SHIFT()){
            stepchild.eraseMultipleTracks(selectMultipleTracks(stringify("Erase which?")));
          }
          else{
            stepchild.eraseTrack(stepchild.activeTrack);
          }
          stepchild.lastTime = millis();
          break;
        //solo
        case 8:
        stepchild.toggleSolo(stepchild.activeTrack);
          stepchild.lastTime = millis();
          break;
        //latch
        case 9:
          stepchild.trackData[stepchild.activeTrack].setLatched(!stepchild.trackData[stepchild.activeTrack].isLatched());
          //latch all tracks
          if(stepchild.buttons.SHIFT()){
            for(int track = 0; track<stepchild.trackData.size(); track++){
              stepchild.trackData[track].setLatched(stepchild.trackData[stepchild.activeTrack].isLatched());
            }
          }
          stepchild.lastTime = millis();
          break;
      }
    }
    if(stepchild.buttons.DELETE()){
      if(!stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        stepchild.deleteTrack(stepchild.activeTrack);
      }
      else if(stepchild.buttons.SHIFT()){
        stepchild.lastTime = millis();
        stepchild.deleteEmptyTracks();
      }
    }
    if(stepchild.buttons.NEW()){
      if(stepchild.buttons.SHIFT())
        stepchild.dupeTrack(stepchild.activeTrack);
      else
        stepchild.addTrack(stepchild.trackData[stepchild.activeTrack].pitch+1,stepchild.defaultChannel,true);
      stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.A() || stepchild.buttons.MENU() || stepchild.buttons.B()){
      stepchild.lastTime = millis();
      return false;
    }
  }
  //menu navigation/incrementing values
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickX == -1){
      if(cursor >= 4 && cursor <=6){
        cursor+=3;
        stepchild.lastTime = millis();
      }
    }
    else if(stepchild.buttons.joystickX == 1){
      if(cursor >= 7 && cursor <= 9){
        cursor-=3;
        stepchild.lastTime = millis();
      }
    }
    if(cursor<3){
      while(stepchild.buttons.counterA != 0){
        if(stepchild.buttons.counterA >= 1 || stepchild.buttons.joystickX == -1){
          //pitch
          if(cursor == 0 && stepchild.trackData[stepchild.activeTrack].pitch<120){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllPitches(1);
            else
              stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch+1,true);
            stepchild.lastTime = millis();
          }
          //octave
          else if(cursor == 1 && stepchild.trackData[stepchild.activeTrack].pitch<108){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllPitches(12);
            else
              stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch+12,true);
            stepchild.lastTime = millis();
          }
          //channel
          else if(cursor == 2 && stepchild.trackData[stepchild.activeTrack].channel<16){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllChannels(1);
            else
              stepchild.setTrackChannel(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].channel+1,true);
              stepchild.lastTime = millis();
          }
          stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
        }
        if(stepchild.buttons.counterA <= -1 || stepchild.buttons.joystickX == 1){
          //pitch
          if(cursor == 0 && stepchild.trackData[stepchild.activeTrack].pitch>0){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllPitches(-1);
            else
              stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch-1,true);
            stepchild.lastTime = millis();
          }
          //octave
          else if(cursor == 1 && stepchild.trackData[stepchild.activeTrack].pitch>11){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllPitches(-12);
            else
              stepchild.setTrackPitch(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].pitch-12,true);
            stepchild.lastTime = millis();
          }
          //channel
          else if(cursor == 2 && stepchild.trackData[stepchild.activeTrack].channel>1){
            if(stepchild.buttons.SHIFT())
              stepchild.transposeAllChannels(-1);
            else
              stepchild.setTrackChannel(stepchild.activeTrack,stepchild.trackData[stepchild.activeTrack].channel-1,true);
            stepchild.lastTime = millis();
          }
          stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
        }
      }
    }
    while(stepchild.buttons.counterB != 0){
      if(stepchild.buttons.counterB <= -1){
        stepchild.setActiveTrack(stepchild.activeTrack+1,true);
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.counterB >= 1){
        stepchild.setActiveTrack(stepchild.activeTrack-1,true);
        stepchild.lastTime = millis();
      }
      stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
    }
  }
  return true;
}

void TrackMenu::displayMenu(){
  SequenceRenderSettings settings;
  settings.shrinkTopDisplay = false;
  settings.drawLoopFlags = false;
  settings.drawPram = false;
  settings.topLabels = false;
  settings.drawTrackChannel = true;
  stepchild.display.clearDisplay();
  graphics.drawSeq(settings);
  displayMenuSidebar();
  stepchild.display.display();
}

String TrackMenu::getTooltipText(){
  const vector<String> texts = {"change pitch","change octave","change channel","prime 4 rec","edit","mute","move","erase","solo","latch"};
  switch(cursor){
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
  }
  return texts[cursor];
}

void TrackMenu::displayMenuSidebar(){

  //title
  stepchild.display.setCursor((stepchild.SCREEN_WIDTH-coords.start.x)-34,7);
  stepchild.display.setFont(&FreeSerifItalic9pt7b);
  stepchild.display.setTextColor(SSD1306_WHITE);
  stepchild.display.print("Trk");
  stepchild.display.print(stringify(stepchild.activeTrack+1));
  stepchild.display.setFont();

  //drawing menu box
  stepchild.display.fillRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_BLACK);
  stepchild.display.drawRoundRect(coords.start.x,coords.start.y+12,coords.end.x-coords.start.x+1,coords.end.y-coords.start.y-11,3,SSD1306_WHITE);
  //top labels 
  String text = getTooltipText();
  // graphics.printChunky(coords.end.x-text.length()*6,coords.start.y+5,text,1);
  // graphics.printChunky(coords.end.x-6,coords.start.y+5,stringify(cursor),1);
  graphics.printSmall(coords.end.x-text.length()*4,coords.start.y+6,text,1);
  graphics.printSmall(coords.end.x-60,coords.start.y,"[n]->new|[del]->del",1);
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu options, and the highlight
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = stepchild.pitchToString(stepchild.trackData[stepchild.activeTrack].pitch,false,true);
  if(cursor == 0){
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    stepchild.display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    stepchild.display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  graphics.printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  graphics.printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(stepchild.getOctave(stepchild.trackData[stepchild.activeTrack].pitch));
  if(cursor == 1)
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  graphics.printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(stepchild.trackData[stepchild.activeTrack].channel);
  if(cursor == 2)
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  graphics.printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // graphics.printSmall(coords.start.x+col1X,37,"rec",1);
  stepchild.display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(stepchild.trackData[stepchild.activeTrack].isPrimed()){
    // graphics.printSmall(coords.start.x+2,37,"prime",1);
    if(millis()%1000<500)
      stepchild.display.fillCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
    else
      stepchild.display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    stepchild.display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  
  //edit
  stepchild.display.drawBitmap(coords.start.x+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  stepchild.display.drawBitmap(coords.start.x+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(stepchild.trackData[stepchild.activeTrack].isMuted())
    stepchild.display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    stepchild.display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  stepchild.display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  stepchild.display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(stepchild.trackData[stepchild.activeTrack].isLatched())
    stepchild.display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    stepchild.display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(cursor == 3)
    graphics.drawArrow(coords.start.x+25+((millis()/200)%2),39,2,ARROW_LEFT,true);
  //first column
  else if(cursor>=4 && cursor<=6)
    graphics.drawRectWithMissingCorners(coords.start.x+3,34+7*(cursor-3),15,9,SSD1306_WHITE);
  else if(cursor>=7)
    graphics.drawRectWithMissingCorners(coords.start.x+17,34+7*(cursor-6),15,9,SSD1306_WHITE);
}

void trackMenu(){
  TrackMenu trackMenu;
  trackMenu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_SLOW);
  while(trackMenu.trackMenuControls()){
    trackMenu.displayMenu();
  }
  trackMenu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_SLOW);
}

/*
--------------------------------
      Track Edit Menu
--------------------------------
*/

class TrackEditMenu:public StepchildMenu{
  public:
    //for which param to edit
    uint8_t xCursor = 0;
    //params are: track select, note, oct, channel, prime, latch, mute group
    //for which track to edit
    uint8_t yCursor = 0;
    TrackEditMenu(){
      coords = CoordinatePair(94,0,129,65);
    }
    void displayMenu();
    void drawTrackInfo();
    bool trackEditMenuControls();
};

void TrackEditMenu::drawTrackInfo(){
  const uint8_t sideWidth = 18;
  //track scrolling
  stepchild.endTrack = stepchild.startTrack + stepchild.trackData.size();
  stepchild.trackHeight = (stepchild.SCREEN_HEIGHT - stepchild.HEADER_HEIGHT) / stepchild.trackData.size();
  if(stepchild.trackData.size()>5){
    stepchild.endTrack = stepchild.startTrack + 5;
    stepchild.trackHeight = (stepchild.SCREEN_HEIGHT-stepchild.HEADER_HEIGHT)/5;
  }
  while(stepchild.activeTrack>stepchild.endTrack-1 && stepchild.trackData.size()>5){
    stepchild.startTrack++;
    stepchild.endTrack++;
  }
  while(stepchild.activeTrack<stepchild.startTrack && stepchild.trackData.size()>5){
    stepchild.startTrack--;
    stepchild.endTrack--;
  }
  stepchild.display.setTextSize(1);
  stepchild.display.setTextColor(SSD1306_WHITE);

  //sideWidth border
  graphics.drawDottedLineV2(sideWidth,stepchild.HEADER_HEIGHT+1,64,6);

  //top and bottom bounds
  stepchild.display.drawFastHLine(0,stepchild.HEADER_HEIGHT-1,stepchild.SCREEN_WIDTH,SSD1306_WHITE);

  //tracks
  for(uint8_t track = stepchild.startTrack; track<stepchild.startTrack+5; track++){
    unsigned short int y1 = (track-stepchild.startTrack) * stepchild.trackHeight + stepchild.HEADER_HEIGHT-1;
    if(stepchild.trackData[track].isSelected()){
      //double digit
      if((track+1)>=10){
        stepchild.display.setCursor(1-((millis()/200)%2), y1+4);
        stepchild.display.print("{");
        stepchild.display.setCursor(13+((millis()/200)%2), y1+4);
        stepchild.display.print("}");
      }
      else{
        stepchild.display.setCursor(3-((millis()/200)%2), y1+4);
        stepchild.display.print("{");
        stepchild.display.setCursor(13+((millis()/200)%2), y1+4);
        stepchild.display.print("}");
      }
    }
    //track info display
    //single digit
    if((track+1)<10){
      graphics.printSmall(9,y1+5,stringify(track+1),SSD1306_WHITE);
    }
    else{
      graphics.printSmall(17-stringify(track+1).length()*6,y1+5,stringify(track+1),SSD1306_WHITE);
    }
    //track cursor
    if(track == stepchild.activeTrack){
      //track
      if(xCursor == 0)
        graphics.drawArrow(3+((millis()/200)%2),y1+7,2,ARROW_RIGHT,true);
      //note
      else if(xCursor == 1){
        if(stepchild.trackData[track].getPitch().length()>1)
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 15, stepchild.trackHeight+2, 3, SSD1306_WHITE);
        else
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 10, stepchild.trackHeight+2, 3, SSD1306_WHITE);
      }
      //oct
      else if(xCursor == 2){
        if(stepchild.trackData[track].pitch>=120){
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, stepchild.trackHeight+2, 3, SSD1306_WHITE);
        }
        else
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, stepchild.trackHeight+2, 3, SSD1306_WHITE);
      }
      //channel
      else if(xCursor == 3){
        if(stepchild.trackData[track].channel>=10)
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+3, y1+2, 13, stepchild.trackHeight+2, 3, SSD1306_WHITE);
        else
          stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+2, 10, stepchild.trackHeight+2, 3, SSD1306_WHITE);
      }
      //rec
      else if(xCursor == 4){
        stepchild.display.drawCircle(sideWidth+(xCursor-1)*10+10, y1+7, 5, SSD1306_WHITE);
      }
      //mute group
      else if(xCursor == 6){
        if(stepchild.trackData[track].muteGroup == 0)
          stepchild.display.drawRoundRect(sideWidth+56, y1+2, 19, stepchild.trackHeight+2, 3, SSD1306_WHITE);
        else
          stepchild.display.drawRoundRect(sideWidth+56, y1+2, 5+4*stringify(stepchild.trackData[track].muteGroup).length(), stepchild.trackHeight+2, 3, SSD1306_WHITE);
      }
      else if(xCursor != 5)
        stepchild.display.drawRoundRect(sideWidth+(xCursor-1)*10+6, y1+1, 10, stepchild.trackHeight+2, 3, SSD1306_WHITE);
    }

    //all the track info...
    //pitch, oct, and channel
    graphics.printSmall(sideWidth+6,  y1+5, stepchild.trackData[track].getPitch(), SSD1306_WHITE);//pitch

    if(stepchild.getOctave(stepchild.trackData[track].pitch)>=10)
      graphics.printSmall(sideWidth+16, y1+5, stringify(stepchild.getOctave(stepchild.trackData[track].pitch)), SSD1306_WHITE);//octave
    else
      graphics.printSmall(sideWidth+20, y1+5, stringify(stepchild.getOctave(stepchild.trackData[track].pitch)), SSD1306_WHITE);//octave

    if(stepchild.trackData[track].channel>=10)
        graphics.printSmall(sideWidth+26, y1+5, stringify(stepchild.trackData[track].channel), SSD1306_WHITE);//channel
    else
      graphics.printSmall(sideWidth+30, y1+5, stringify(stepchild.trackData[track].channel), SSD1306_WHITE);//channel
    
    //primed rec symbol
    if(stepchild.trackData[track].isPrimed()){
      if(millis()%1000>500){
        stepchild.display.drawCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
      else{
        stepchild.display.fillCircle(sideWidth+40, y1+7, 3, SSD1306_WHITE);
      }
    }

    //latch
    graphics.drawCheckbox(sideWidth+48,y1+4,stepchild.trackData[track].isLatched(),xCursor == 5 && stepchild.activeTrack == track);

    //mute group
    if(stepchild.trackData[track].muteGroup == 0){
      graphics.printSmall(sideWidth+58, y1+5, "none",SSD1306_WHITE);
    }
    else{
      graphics.printSmall(sideWidth+59, y1+5, stringify(stepchild.trackData[track].muteGroup),SSD1306_WHITE);
    }
  }
}
void TrackEditMenu::displayMenu(){
  stepchild.display.clearDisplay();
  drawTrackInfo();
  //corner logo
  stepchild.display.setCursor((stepchild.SCREEN_WIDTH-coords.start.x)-34,7);
  stepchild.display.setFont(&FreeSerifItalic9pt7b);
  stepchild.display.setTextColor(SSD1306_WHITE);
  stepchild.display.print("Trk");
  stepchild.display.print(stringify(stepchild.activeTrack+1));
  stepchild.display.setFont();

  //track info
  drawTrackMenuTopInfo(coords.start.x);

  //drawing menu box
  stepchild.display.fillRect(coords.start.x,coords.start.y, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  stepchild.display.drawRoundRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x+1, coords.end.y-coords.start.y-11, 3,SSD1306_WHITE);
  String s;
  switch(cursor){
    case 0:
      s = "pitch";
      break;
    case 1:
      s = "octave";
      break;
    case 2:
      s = "chnnl";
      break;
    case 3:
      s = "prime";
      break;
    case 4:
      s = "utils";
      break;
    case 5:
      s = "mute";
      break;
    case 6:
      s = "move";
      break;
    case 7:
      s = "erase";
      break;
    case 8:
      s = "solo";
      break;
    case 9:
      s = "latch";
      break;
  }
  graphics.printChunky(coords.end.x-s.length()*6,coords.start.y+5,s,SSD1306_WHITE);

  //drawing menu options, and the highlight
  const uint8_t textHeight = 8;
  const uint8_t yPos = 12;
  //printing out the menu
  //pitch
  String p = stepchild.pitchToString(stepchild.trackData[stepchild.activeTrack].pitch,false,true);
  if(cursor == 0){
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2,8+p.length()*4+12,9,4,SSD1306_WHITE);
    stepchild.display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_BLACK,SSD1306_WHITE,1);
  }
  else
    stepchild.display.drawChar(coords.start.x+6,yPos+coords.start.y+3,0x0E,SSD1306_WHITE,SSD1306_BLACK,1);
  graphics.printSmall(coords.start.x+12,yPos+coords.start.y+3,"$",2);//printing all the text inverse, so it turns black when highlighted
  graphics.printSmall(coords.start.x+14,yPos+coords.start.y+4," = "+p,2);
  //oct
  p = stringify(stepchild.getOctave(stepchild.trackData[stepchild.activeTrack].pitch));
  if(cursor == 1)
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+3+textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  graphics.printSmall(coords.start.x+6,yPos+coords.start.y+4+textHeight,"/8 = "+p,2);
  //channel
  p = stringify(stepchild.trackData[stepchild.activeTrack].channel);
  if(cursor == 2)
    stepchild.display.fillRoundRect(coords.start.x+4,yPos+coords.start.y+2+2*textHeight,19+p.length()*4,7,4,SSD1306_WHITE);
  graphics.printSmall(coords.start.x+6,yPos+coords.start.y+3+2*textHeight,"ch = "+p,2);
  
  const uint8_t col1X = 5;
  const uint8_t col2X = 19;

  //prime icon
  // graphics.printSmall(coords.start.x+col1X,37,"rec",1);
  stepchild.display.drawBitmap(coords.start.x+col1X,38,rec_tiny,11,4,1,0);
  if(stepchild.trackData[stepchild.activeTrack].isPrimed()){
    // graphics.printSmall(coords.start.x+2,37,"prime",1);
    if(millis()%1000<500)
      stepchild.display.fillCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
    else
      stepchild.display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  else{
    stepchild.display.drawCircle(coords.start.x+col2X+4,39,2,SSD1306_WHITE);
  }
  //edit
  stepchild.display.drawBitmap(coords.start.x+col1X,43,track_pencil,11,5,SSD1306_WHITE);
  //erase
  stepchild.display.drawBitmap(coords.start.x+col2X,43,track_eraser,11,5,SSD1306_WHITE);
  //mute
  if(stepchild.trackData[stepchild.activeTrack].isMuted())
    stepchild.display.drawBitmap(coords.start.x+col1X,50,track_mute,11,5,SSD1306_WHITE);
  else
    stepchild.display.drawBitmap(coords.start.x+col1X,50,track_unmute,11,5,SSD1306_WHITE);
  //solo
  stepchild.display.drawBitmap(coords.start.x+col2X,50,track_solo,11,5,SSD1306_WHITE);
  //move
  stepchild.display.drawBitmap(coords.start.x+col1X,57,track_arrows,11,5,SSD1306_WHITE);
  //latch
  if(stepchild.trackData[stepchild.activeTrack].isLatched())
    stepchild.display.drawBitmap(coords.start.x+col2X+3,57,track_latch,5,5,SSD1306_WHITE);
  else
    stepchild.display.drawBitmap(coords.start.x+col2X+3,57,track_unlatch,5,5,SSD1306_WHITE);
  
  //menu highlight
  if(cursor == 3)
    graphics.drawArrow(coords.start.x+28+((millis()/200)%2),39,2,ARROW_LEFT,true);
  //first column
  else if(cursor>=4 && cursor<=6){
    graphics.drawRectWithMissingCorners(coords.start.x+3,34+7*(cursor-3),15,9,SSD1306_WHITE);
  }
  else if(cursor>=7){
    graphics.drawRectWithMissingCorners(coords.start.x+17,34+7*(cursor-6),15,9,SSD1306_WHITE);
  }
  stepchild.display.display();
}

bool TrackEditMenu::trackEditMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(stepchild.itsbeen(150)){
    //moving thru tracks
    if(stepchild.buttons.joystickY != 0){
      if (stepchild.buttons.joystickY == 1) {
        stepchild.setActiveTrack(stepchild.activeTrack + 1, true);
        stepchild.lastTime = millis();
      }
      if (stepchild.buttons.joystickY == -1) {
        stepchild.setActiveTrack(stepchild.activeTrack - 1, true);
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    //moving thru params
    if(stepchild.buttons.joystickX != 0){
      //changing params
      if(stepchild.buttons.SHIFT()){
        if(stepchild.buttons.joystickX == 1){
          switch(xCursor){
            //note
            case 1:
              if(stepchild.trackData[stepchild.activeTrack].pitch>0){
                stepchild.trackData[stepchild.activeTrack].pitch--;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch>0)
                    stepchild.trackData[track].pitch--;
                }
              }
              break;
            //octave
            case 2:
              if(stepchild.trackData[stepchild.activeTrack].pitch>11){
                stepchild.trackData[stepchild.activeTrack].pitch-=12;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch>11)
                    stepchild.trackData[track].pitch-=12;
                }
              }
              break;
            //channel
            case 3:
              if(stepchild.trackData[stepchild.activeTrack].channel>1){
                stepchild.trackData[stepchild.activeTrack].channel--;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].channel>0)
                    stepchild.trackData[track].channel--;
                }
              }
              break;
            //mute group
            case 6:
              if(stepchild.trackData[stepchild.activeTrack].muteGroup>0){
                stepchild.trackData[stepchild.activeTrack].muteGroup--;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].muteGroup>0)
                    stepchild.trackData[track].muteGroup--;
                }
              }
              break;
          }
        }
        else if(stepchild.buttons.joystickX == -1){
          switch(xCursor){
            //note
            case 1:
              if(stepchild.trackData[stepchild.activeTrack].pitch<127){
                stepchild.trackData[stepchild.activeTrack].pitch++;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch<127)
                    stepchild.trackData[track].pitch++;
                }
              }
              break;
            //octave
            case 2:
              if(stepchild.trackData[stepchild.activeTrack].pitch<=115){
                stepchild.trackData[stepchild.activeTrack].pitch+=12;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch<=115)
                    stepchild.trackData[track].pitch+=12;
                }
              }
              break;
            //channel
            case 3:
              if(stepchild.trackData[stepchild.activeTrack].channel<16){
                stepchild.trackData[stepchild.activeTrack].channel++;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].channel<16)
                    stepchild.trackData[track].channel++;
                }
              }
              break;
            //mute group
            case 6:
              if(stepchild.trackData[stepchild.activeTrack].muteGroup<127){
                stepchild.trackData[stepchild.activeTrack].muteGroup++;
                stepchild.lastTime = millis();
                //checking all selected tracks
                for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                  if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].muteGroup<127)
                    stepchild.trackData[track].muteGroup++;
                }
              }
              break;
          }
        }
      }
      else{
        if(stepchild.buttons.joystickX == 1 && xCursor > 0){
          xCursor--;
          stepchild.lastTime = millis();
        }
        else if(stepchild.buttons.joystickX == -1 && xCursor<6){
          xCursor++;
          stepchild.lastTime = millis();
        }
      }
    }
    if(stepchild.buttons.NEW()){
      stepchild.addTrack(stepchild.trackData[stepchild.activeTrack].pitch,stepchild.defaultChannel,false);
      stepchild.lastTime = millis();
    }
    while(stepchild.buttons.counterA != 0){
      if(stepchild.buttons.counterA >= 1){
        switch(xCursor){
          //note
          case 1:
            if(stepchild.trackData[stepchild.activeTrack].pitch<127){
              stepchild.trackData[stepchild.activeTrack].pitch++;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch<127)
                  stepchild.trackData[track].pitch++;
              }
            }
            break;
          //octave
          case 2:
            if(stepchild.trackData[stepchild.activeTrack].pitch<=115){
              stepchild.trackData[stepchild.activeTrack].pitch+=12;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch<=115)
                  stepchild.trackData[track].pitch+=12;
              }
            }
            break;
          //channel
          case 3:
            if(stepchild.trackData[stepchild.activeTrack].channel<16){
              stepchild.trackData[stepchild.activeTrack].channel++;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].channel<16)
                  stepchild.trackData[track].channel++;
              }
            }
            break;
          //mute group
          case 6:
            if(stepchild.trackData[stepchild.activeTrack].muteGroup<127){
              stepchild.trackData[stepchild.activeTrack].muteGroup++;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].muteGroup<127)
                  stepchild.trackData[track].muteGroup++;
              }
            }
            break;
        }
      }
      else{
        switch(xCursor){
          //note
          case 1:
            if(stepchild.trackData[stepchild.activeTrack].pitch>0){
              stepchild.trackData[stepchild.activeTrack].pitch--;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch>0)
                  stepchild.trackData[track].pitch--;
              }
            }
            break;
          //octave
          case 2:
            if(stepchild.trackData[stepchild.activeTrack].pitch>11){
              stepchild.trackData[stepchild.activeTrack].pitch-=12;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].pitch>11)
                  stepchild.trackData[track].pitch-=12;
              }
            }
            break;
          //channel
          case 3:
            if(stepchild.trackData[stepchild.activeTrack].channel>1){
              stepchild.trackData[stepchild.activeTrack].channel--;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].channel>0)
                  stepchild.trackData[track].channel--;
              }
            }
            break;
          //mute group
          case 6:
            if(stepchild.trackData[stepchild.activeTrack].muteGroup>0){
              stepchild.trackData[stepchild.activeTrack].muteGroup--;
              stepchild.lastTime = millis();
              //checking all selected tracks
              for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
                if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected() && stepchild.trackData[track].muteGroup>0)
                  stepchild.trackData[track].muteGroup--;
              }
            }
            break;
        }
      }
      stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
    }
    if(stepchild.buttons.SELECT() ){
      switch(xCursor){
        //if it's on track, select
        case 0:
          //toggles selection on all
          if(stepchild.buttons.SHIFT()){
            stepchild.trackData[stepchild.activeTrack].setSelected(!stepchild.trackData[stepchild.activeTrack].isSelected());
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              stepchild.trackData[track].setSelected(stepchild.trackData[stepchild.activeTrack].isSelected());
            }
          }
          //normal selection toggle
          else
            stepchild.trackData[stepchild.activeTrack].setSelected(!stepchild.trackData[stepchild.activeTrack].isSelected());
          
          stepchild.lastTime = millis();
          break;
        //if it's on primed
        case 4:
          //toggles prime on all
          if(stepchild.buttons.SHIFT()){
            stepchild.trackData[stepchild.activeTrack].setPrimed(!stepchild.trackData[stepchild.activeTrack].isPrimed());
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              stepchild.trackData[track].setPrimed(stepchild.trackData[stepchild.activeTrack].isPrimed());
            }
          }
          //normal selection toggle
          else{
            stepchild.trackData[stepchild.activeTrack].setPrimed(!stepchild.trackData[stepchild.activeTrack].isPrimed());
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected())
                stepchild.trackData[track].setPrimed(stepchild.trackData[stepchild.activeTrack].isPrimed());
            }
          }
          stepchild.lastTime = millis();
          break;
        //if it's on latch
        case 5:
          //toggles prime on all
          if(stepchild.buttons.SHIFT()){
            stepchild.trackData[stepchild.activeTrack].setLatched(!stepchild.trackData[stepchild.activeTrack].isLatched());
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              stepchild.trackData[track].setLatched(stepchild.trackData[stepchild.activeTrack].isLatched());
            }
          }
          //normal selection toggle
          else{
            stepchild.trackData[stepchild.activeTrack].setLatched(!stepchild.trackData[stepchild.activeTrack].isLatched());
            for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
              if(track != stepchild.activeTrack && stepchild.trackData[track].isSelected())
                stepchild.trackData[track].setLatched(stepchild.trackData[stepchild.activeTrack].isLatched());
            }
          }
          stepchild.lastTime = millis();
          break;
      }
    }
    if(stepchild.buttons.MENU() || stepchild.buttons.A()){
      for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
        stepchild.trackData[track].setSelected(false);
      }
      stepchild.lastTime = millis();
      stepchild.buttons.setMENU(false) ;
      stepchild.buttons.setA(false);;
      return false;
    }
  }
  return true;
}

void trackEditMenu(){
  //deselecting all the tracks
  for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
    stepchild.trackData[track].setSelected(false);
  }
  TrackEditMenu trackEditMenu;
  while(trackEditMenu.trackEditMenuControls()){
    trackEditMenu.displayMenu();
  }
  //deselecting all the tracks
  for(uint8_t track = 0; track<stepchild.trackData.size(); track++){
    stepchild.trackData[track].setSelected(false);
  }
}

/*
--------------------------------
      Track Utility Menu
--------------------------------
*/

class TrackUtilMenu:public StepchildMenu{
  public:
    //storing a ref to the track menu so you can call the display method
    TrackMenu* trackMenu;
    const vector<String> options = {"group edit tracks","tune 2 scale","delete empty tracks","disarm tracks w notes","sort tracks"};
    TrackUtilMenu(TrackMenu* t){trackMenu = t;
                                coords = CoordinatePair(0,16,128,64);}
    void drawTrackUtils();
    bool trackUtilMenuControls();
    void displayMenu();
};

void TrackUtilMenu::displayMenu(){
  stepchild.display.clearDisplay();
  if(coords.start.y>16){
  SequenceRenderSettings settings;
    settings.shrinkTopDisplay = false;
    settings.drawLoopFlags = false;
    settings.drawPram = false;
    settings.topLabels = false;
    settings.drawTrackChannel = true;
    graphics.drawSeq(settings);
  }
  trackMenu->displayMenuSidebar();
  drawTrackUtils();
  stepchild.display.display();
}

//shows options for edit, tune tracks, del empty tracks, and disarm tracks w/ notes
void TrackUtilMenu::drawTrackUtils(){
  const uint8_t length = 95;
  stepchild.display.fillRect(coords.start.x,coords.start.y,length,stepchild.SCREEN_HEIGHT-coords.start.y,0);
  stepchild.display.drawRect(coords.start.x,coords.start.y,length,stepchild.SCREEN_HEIGHT-coords.start.y,1);
  for(uint8_t i = 0; i<options.size(); i++){
    if(i == cursor){
      stepchild.display.fillRect(coords.start.x+1,coords.start.y+i*6+1,length-2,7,1);
    }
    graphics.printSmall(coords.start.x+2,coords.start.y+i*6+2,options[i],2);
  }
}

bool TrackUtilMenu::trackUtilMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickY == -1 && cursor>0){
      cursor--;
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.joystickY == 1 && cursor<(options.size()-1)){
      cursor++;
      stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      return false;
    }
    if(stepchild.buttons.SELECT() ){
      stepchild.lastTime = millis();
      switch(cursor){
        case 0:
          trackEditMenu();
          break;
        case 1:
          tuneTracksToScale();
          break;
        case 2:
          stepchild.deleteEmptyTracks();
          break;
        case 3:
          stepchild.disarmTracksWithNotes();
          break;
        case 4:
          sortTracks();
          break;
      }
    }
  }
  return true;
}

void trackUtils(TrackMenu* t){
  TrackUtilMenu trackUtilMenu(t);
  trackUtilMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_FAST);
  while(trackUtilMenu.trackUtilMenuControls()){
    trackUtilMenu.displayMenu();
  }
  trackUtilMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_FAST);
}

/*
    Common drawing functions:
*/


//primed, pitch, latch, mute info
void drawTrackMenuTopInfo(uint8_t topL){
  //track info
  uint8_t x1 = 3+(stepchild.SCREEN_WIDTH-topL)+uint8_t(log10(stepchild.activeTrack+1))*9;
  String p = stepchild.pitchToString(stepchild.trackData[stepchild.activeTrack].pitch,true,true);
  graphics.printSmall(x1,0,p,1);
  graphics.printSmall(x1+4+p.length()*4,0,"("+stringify(stepchild.trackData[stepchild.activeTrack].pitch)+")",1);
  //primed
  if(stepchild.trackData[stepchild.activeTrack].isPrimed()){
    if(millis()%1000>500){
      stepchild.display.drawCircle(x1+3,10,3,1);
    }
    else{
      stepchild.display.fillCircle(x1+3,10,3,1);
    }
  }
  //latch
  if(stepchild.trackData[stepchild.activeTrack].isLatched()){
    stepchild.display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
    stepchild.display.fillRect(x1+10,9,5,4,1);
  }
  else
    stepchild.display.drawBitmap(x1+9,7,latch_big,7,7,1,0);
  //mute
  stepchild.display.drawBitmap(x1+18,7,stepchild.trackData[stepchild.activeTrack].isMuted()?muted:unmuted,7,7,1,0);
  //solo'd
  if(stepchild.trackData[stepchild.activeTrack].isSolo())
    graphics.printItalic(x1+27,7,"S",1);
  //drum icon
  graphics.drawDrumIcon(x1+35,2,stepchild.trackData[stepchild.activeTrack].pitch);
}
