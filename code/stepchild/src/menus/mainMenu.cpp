#include "Stepchild.h"
#include "menus.h"
#include "graphics/bitmaps.h"

#include "graphics/WireFrame.h"

;
extern WireFrame genRandMenuObjects(uint8_t x1, uint8_t y1, uint8_t distance, float scale);
extern bool loadBackup();

using namespace std;

class MainMenu:public StepchildMenu{
  public:
    WireFrame icon;
    uint8_t wireFrameID;
    bool renderWireframes = false;
    //set this to false if a submenu needs to jump right back
    //to the main seq (used by FX menu after you use an FX)
    bool shouldSlideOut = true;
    SequenceRenderSettings settings;
    MainMenu(){
      coords = CoordinatePair(25,1,93,64);
      icon = makeGraphBox(5);
      icon.offset.x = 112;
      icon.offset.y = 16;
      graphAnimation(&icon);
      settings.trackLabels = true;
      settings.topLabels = false;
      settings.drawLoopPoints = true;
      settings.shrinkTopDisplay = false;
    }
    void displayMenu();
    bool mainMenuControls();
    void updateMainMenuWireFrame();
    void animateMainMenuWireFrame();
    void drawMainMenuLabel();
};

void MainMenu::updateMainMenuWireFrame(){
  if(wireFrameID == cursor)
    return;
  switch(cursor){
    //autotracks
    case 0:
      icon = makeGraphBox(5);
      break;
    //loops
    case 1:
      icon = makeMobius(float(millis())/800.0);
      break;
    //apps
    case 2:
      icon = makeHammer();
      break;
    //wrench
    case 3:
      icon = makeWrench();
      break;
    //quicksave
    case 4:
      icon = makeCD();
      break;
    //liveloop
    case 5:
      icon = makeCassette();
      icon.rotate(30,0);
      icon.rotate(15,2);
      break;
    //rec
    case 6:
      icon = genRandMenuObjects(0,0,10,1);
      break;
    //PC
    case 7:
      icon = makeMonitor();
      icon.rotate(-30,1);
      icon.rotate(-10,0);
      break;
    //midi
    case 8:
      icon = makeMIDI();
      break;
    //files
    case 9:
      icon = makeFolder(30);
      break;
    //clock
    case 10:
      icon = makeMetronome(0);
      break;
    //arp
    case 11:
      icon = makeArpBoxes(millis());
      break;
  }
  icon.offset.x = 112;
  if(cursor == 7)
    icon.offset.y = 11;
  else
    icon.offset.y = 16;
  wireFrameID = cursor;
}

void MainMenu::animateMainMenuWireFrame(){
  switch(cursor){
    //autotracks
    case 0:
      graphAnimation(&icon);
      icon.rotate(-2,0);
      icon.rotate(2,1);
      icon.rotate(2,2);
      break;
    //loop
    case 1:
      //cute idea to pause the loop icon, but you're never not looping!
      // if(stepchild.isLooping)
      icon = makeMobius(float(millis())/800.0);
      icon.offset.x = 112;
      icon.offset.y = 16;
      icon.rotate(float(millis())/50.0,0);
      icon.rotate(-float(millis())/75.0,1);
      break;
    //apps
    case 2:
    //settings/preferences
    case 3:
    //quicksave
    case 4:
    //liveloop
    case 5:
      icon.rotate(2,1);
      break;
    //random
    case 6:
      icon.rotate(2,0);
      icon.rotate(2,1);
      break;
    //Program Change
    case 7:
      animateMonitor(icon,3.0,14);
      break;
    //midi
    case 8:
      icon.rotate(2,1);
      break;
    //files
    case 9:
      folderAnimation(icon);
      icon.rotate(2,1);
      break;
    //clock
    case 10:
      metAnimation(&icon);
      icon.offset.y = 16 + 2.0*sin(float(millis())/400);
      icon.setRotation(20.0*sin(float(millis())/400.0),1);
      break;
    //arp
    case 11:
      icon = makeArpBoxes(millis()/1.5);
      icon.offset.y = 16;
      icon.offset.x = 112;
      break;
  }
}

bool MainMenu::mainMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickX != 0){
      if(stepchild.buttons.joystickX == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((cursor+1)%4)
          cursor++;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickX == 1){
        if((cursor)%4)
          cursor--;
        stepchild.lastTime = millis();
      }
    }
    if(stepchild.buttons.joystickY != 0){
      if(stepchild.buttons.joystickY == 1){
        //first row
        if(cursor < 8)
          cursor += 4;
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickY == -1){
        //second row
        if(cursor > 3)
          cursor -= 4;
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      renderWireframes = false;
      return false;
    }
    if(stepchild.buttons.LOOP()){
      stepchild.lastTime = millis();
      renderWireframes = false;
      slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
      loopMenu();
    }
    if(stepchild.buttons.NEW()){
      stepchild.lastTime = millis();
      renderWireframes = false;
      slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
      return fxMenu();
    }
    if(stepchild.buttons.SELECT() ){
      stepchild.buttons.setSELECT(false);
      stepchild.lastTime = millis();
      switch(cursor){
        //autotracks
        case 0:
          renderWireframes = false;
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          autotrackMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          renderWireframes = true;
          break;
        //loop
        case 1:
          renderWireframes = false;
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          loopMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          renderWireframes = true;
          break;
        //keys
        case 2:
          renderWireframes = false;
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          instrumentMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          renderWireframes = true;
          break;
        //settings
        case 3:
          renderWireframes = false;
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          settingsMenu();
          return false;
        //quicksave
        case 4:
          //if you're shifting, load most recent backup
          if(stepchild.buttons.SHIFT()){
            loadBackup();
            return false;
          }
          else{
            stepchild.filesystem.quickSave();
            stepchild.tooltipText = "saved \'"+stepchild.filesystem.currentFilename+"\'";
            return false;
          }
          break;
        //fx
        case 5:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          liveLoopMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
          // renderWireframes = false;
          // slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          // switch(fxMenu()){
          //   case BACK_TO_MAIN_SEQUENCE:
          //     shouldSlideOut = false;
          //     return false;
          //   default:
          //     renderWireframes = true;
          //     slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          //     return true;
          // }
        //rec
        case 6:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          randomMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
        //console
        case 7:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          PCEditor();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
        //midi
        case 8:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          midiMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
        //files
        case 9:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          //if you load a file, exit
          if(fileMenu()){
            return false;
          }
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
        //clock
        case 10:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          clockMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
        //arp
        case 11:
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          arpMenu();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          break;
      }
    }
  }
  return true;
}

void MainMenu::drawMainMenuLabel(){
  String text;
  switch(cursor){
    //DT
    case 0:
      text = "AUTO";
      break;
    //Loop
    case 1:
      text = "LOOPS";
      break;
    //instruments
    case 2:
      text = "APPS";
      break;
    //settings
    case 3:
      text = "PREFS";
      break;
    //quicksave
    case 4:
      text = "QIKSAVE";
      break;
    //clock
    case 5:
      text = "LIVELOOP";
      break;
    //rec
    case 6:
      text = "RANDOM";
      break;
    //heart
    case 7:
      text = "PRGCHG";
      break;
    //midi
    case 8:
      text = "MIDI";
      break;
    //files
    case 9:
      text = "FILES";
      break;
    //clock
    case 10:
      text = "CLOCK";
      break;
    //arp menu
    case 11:
      text = "ARP";
      break;
  }
  graphics.drawLabel(112,34,text,true);
}

void MainMenu::displayMenu(){
  stepchild.display.clearDisplay();
  settings.updateView();
  graphics.drawSeq(settings);

  graphics.ditherBackground(32,16,128,64);

  if(renderWireframes){
    stepchild.display.fillCircle(111,15,23,0);
    stepchild.display.drawCircle(111,15,23,1);
  }

  //drawing menu box (+16 so the title is transparent)
  stepchild.display.fillRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  stepchild.display.drawRoundRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, 3, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.start.x+coords.start.y-1<stepchild.SCREEN_WIDTH){
    stepchild.display.fillRect(coords.start.x+coords.start.y-1,0,40,13,0);
    stepchild.display.setCursor(coords.start.x+coords.start.y-1,5);
    stepchild.display.setFont(&FreeSerifItalic9pt7b);
    stepchild.display.print("Menu");
    stepchild.display.setFont();
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      //animate clock hands
     if(count == 10){
        stepchild.display.drawBitmap(coords.start.x+4+width*i,coords.start.y+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
        graphics.drawCircleRadian(coords.start.x+4+width*i+6,coords.start.y+j*(width-1)+17+sin(millis()/200+count)+6,4,millis()/10,0);
        graphics.drawCircleRadian(coords.start.x+4+width*i+6,coords.start.y+j*(width-1)+17+sin(millis()/200+count)+6,3,millis()/100,0);
      }
      else
        stepchild.display.drawBitmap(coords.start.x+4+width*i,coords.start.y+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      if(i+4*j == cursor){
        stepchild.display.drawRoundRect(coords.start.x+2+width*i,coords.start.y+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
  if(renderWireframes){
    animateMainMenuWireFrame();
    if(cursor == 6)
      icon.renderDie();
    else
      icon.render();
    drawMainMenuLabel();
  }
  stepchild.display.display();
}

void mainMenu(){
  MainMenu mainMenu;
  mainMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  mainMenu.renderWireframes = true;
  while(mainMenu.mainMenuControls()){
    mainMenu.updateMainMenuWireFrame();
    mainMenu.displayMenu();
  }
  if(mainMenu.shouldSlideOut)
    mainMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  // mainMenu.renderWireframes = false;
}
