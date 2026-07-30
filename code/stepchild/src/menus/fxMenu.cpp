#include "classes/StepchildMenu.h"
#include "Stepchild.h"


;

extern bool (*const fxApplicationFunctions[])();
extern const String fxApplicationInfo[];
extern const unsigned char* fxApplicationIcons[];
extern const String fxApplicationTitles[];
extern const uint8_t NUMBER_OF_FX;

using namespace std;

/*

  FX Menu! A dropdown list allowing the user to select and use an FX function. Displays the FX icon, and short text describing it
  See applications.h for a list of the FX and the data that this menu pulls from
*/

class FxMenu:public StepchildMenu{
  public:
    uint8_t menuStart = 0;
    SequenceRenderSettings settings;
    bool usedAtLeastOneFx = false;
    FxMenu(){
      coords = CoordinatePair(25,1,128,64);
      settings.shrinkTopDisplay = false;
      settings.topLabels = false;
      settings.drawLoopFlags = false;
    }
    void drawFxLabel();
    MenuReturnValue fxMenuControls();
    void displayMenu();
};

MenuReturnValue FxMenu::fxMenuControls(){
  stepchild.buttons.readJoystick();
  stepchild.buttons.readButtons();
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickX != 0){
    }
    if(stepchild.buttons.joystickY != 0){
      if(stepchild.buttons.joystickY == 1){
        if(cursor<2)
          cursor++;
        else if(cursor == 2 && menuStart < (NUMBER_OF_FX-3)){
          menuStart++;
        }
        stepchild.lastTime = millis();
      }
      else if(stepchild.buttons.joystickY == -1){
        if(cursor > 0){
          cursor--;
        }
        else if(cursor == 0 && menuStart>0){
          menuStart--;
        }
        stepchild.lastTime = millis();
      }
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      if(usedAtLeastOneFx)
        return BACK_TO_MAIN_SEQUENCE;
      else
        return BACK_TO_MAIN_MENU;
    }
    if(stepchild.buttons.SELECT() ){
      usedAtLeastOneFx = true;
      stepchild.buttons.setSELECT(false);
      stepchild.lastTime = millis();
      slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
      if(fxApplicationFunctions[cursor+menuStart]()){
        usedAtLeastOneFx = true;
      }
      slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
      // stepchild.buttons.clearButtons();
    }
  }
  return NO_ACTION;
}

void FxMenu::drawFxLabel(){
  String text = fxApplicationTitles[cursor+menuStart];
  graphics.printChunky(coords.start.x+coords.start.y+38,4,text,SSD1306_WHITE);
}

void FxMenu::displayMenu(){
  stepchild.display.clearDisplay();
  settings.drawSteps = coords.start.y>stepchild.HEADER_HEIGHT;
  graphics.drawSeq(settings);
  //drawing menu box (+16 so the title is transparent)
  stepchild.display.fillRect(coords.start.x,coords.start.y+13, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
  stepchild.display.drawRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.start.x+coords.start.y-1<stepchild.SCREEN_WIDTH){
    stepchild.display.fillRect(coords.start.x+coords.start.y-1,0,18,13,0);
    stepchild.display.setCursor(coords.start.x+coords.start.y-1,5);
    stepchild.display.setFont(&FreeSerifItalic9pt7b);
    stepchild.display.print("Fx");
    stepchild.display.setFont();
  }
  
  drawFxLabel();
  //printing page number/arrows
  graphics.printSmall(coords.start.x+coords.start.y+18,2,stringify(cursor+menuStart+1),SSD1306_WHITE);
  if(menuStart>0)
    graphics.drawArrow(coords.start.x+coords.start.y+26,1+(millis()/400+1)%2,2,ARROW_UP,true);
  if(menuStart<NUMBER_OF_FX - 3)
    graphics.drawArrow(coords.start.x+coords.start.y+26,10+(millis()/400)%2,2,ARROW_DOWN,true);

  const uint8_t width = 16;
  for(uint8_t i = 0; i<3; i++){
    stepchild.display.drawBitmap(coords.start.x+4,coords.start.y+i*(width-1)+17+sin(millis()/200+i),fxApplicationIcons[i+menuStart],12,12,SSD1306_WHITE);
    if(i == cursor){
      stepchild.display.drawRoundRect(coords.start.x+2,coords.start.y+i*(width-1)+15+sin(millis()/200+i),width,width,3,SSD1306_WHITE);
    }
    if(i==cursor)
      stepchild.display.fillRoundRect(coords.start.x+18,coords.start.y+i*(width-1)+15,84,width,3,1);
    String text = fxApplicationInfo[i+menuStart];
    graphics.printSmall_overflow(coords.start.x+((i==cursor)?19:18),coords.start.y+i*(width-1)+17,2,text,2);
  }
  stepchild.display.display();
}

MenuReturnValue fxMenu(){
  FxMenu fxMenu;
  fxMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM_FAST);
  while(true){
    switch(fxMenu.fxMenuControls()){
      case NO_ACTION:
        break;
      case BACK_TO_MAIN_MENU:
        fxMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
        return BACK_TO_MAIN_MENU;
      case BACK_TO_MAIN_SEQUENCE:
        fxMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
        return BACK_TO_MAIN_SEQUENCE;
    }
    fxMenu.displayMenu();
  }
  return NO_ACTION;
}

