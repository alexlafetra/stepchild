#include "Stepchild.h"
#include "classes/StepchildMenu.h"
#include "StepchildGraphics.h"

extern StepchildGraphics graphics;
extern Stepchild stepchild;
extern bool (*const instrumentApplicationFunctions[])();
extern const String instrumentApplicationInfo[];
extern const unsigned char* instrumentApplicationIcons[];
extern const String instrumentApplicationTitles[];
extern const uint8_t NUMBER_OF_GENERATIVE_INSTRUMENTS;
extern const uint8_t NUMBER_OF_APPLICATIONS;

using namespace std;

/*

two kinds of instruments: tools and generative
generative instruments (more) passively send midi data according to parameters
tools let you use the stepchild's controls to control other instruments

*/

class InstrumentMenu:public StepchildMenu{
  public:
    uint8_t menuStart = 0;
    SequenceRenderSettings settings;
    InstrumentMenu(){
      coords = CoordinatePair(25,1,128,64);
      settings.shrinkTopDisplay = false;
      settings.topLabels = false;
      settings.drawLoopFlags = false;
    }
    bool instrumentMenuControls(){
      stepchild.buttons.readButtons();
      stepchild.buttons.readJoystick();
      if(stepchild.itsbeen(100)){
        if(stepchild.buttons.UP() && cursor < NUMBER_OF_APPLICATIONS-1){
          if(cursor<2)
            cursor++;
          else if(menuStart < (NUMBER_OF_APPLICATIONS-3)){
            menuStart++;
          }
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.DOWN()){
          if(cursor > 0){
            cursor--;
          }
          else if(menuStart>0){
            menuStart--;
          }
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
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          instrumentApplicationFunctions[cursor+menuStart]();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
        }
      }
      return true;
    }
    void displayMenu(){
      stepchild.display.clearDisplay();
      settings.drawSteps = coords.start.y>stepchild.HEADER_HEIGHT;
      graphics.drawSeq(settings);
      //drawing menu box (+16 so the title is transparent)
      stepchild.display.fillRect(coords.start.x,coords.start.y+13, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
      stepchild.display.drawRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, SSD1306_WHITE);

      //if the title will be on screen
      if(coords.start.x+coords.start.y-1<stepchild.SCREEN_WIDTH){
        stepchild.display.setCursor(coords.start.x+coords.start.y-1,5);
        stepchild.display.setFont(&FreeSerifItalic9pt7b);
        stepchild.display.print("Apps");
        stepchild.display.setFont();
      }
      
      String text = instrumentApplicationTitles[cursor+menuStart];
      graphics.printChunky(coords.start.x+coords.start.y+31,17,text,SSD1306_WHITE);

      //say if it's a tool or generator
      stepchild.display.drawFastHLine(coords.start.x+coords.start.y+21,20,8,1);
      if((menuStart+cursor)>NUMBER_OF_GENERATIVE_INSTRUMENTS){
        stepchild.display.drawBitmap(coords.start.x+coords.start.y+48,4,tool_bmp,19,6,1);
      }
      else{
        stepchild.display.drawBitmap(coords.start.x+coords.start.y+48,4,generator_bmp,44,6,1);
      }

      //printing arrows
      if(menuStart>0)
        graphics.drawArrow(coords.start.x+coords.start.y+38,1+(millis()/400+1)%2,2,ARROW_UP,true);
      if(menuStart<NUMBER_OF_APPLICATIONS - 3)
        graphics.drawArrow(coords.start.x+coords.start.y+38,10+(millis()/400)%2,2,ARROW_DOWN,true);

      //frame for the info text
      stepchild.display.drawRoundRect(coords.start.x+19,coords.start.y+14,82,47,3,1);
      text = instrumentApplicationInfo[cursor+menuStart];
      graphics.printSmall_overflow(coords.start.x+23,coords.start.y+24,4,text,1);
      const uint8_t width = 16;
      for(uint8_t i = 0; i<3; i++){
        stepchild.display.drawBitmap(coords.start.x+4,coords.start.y+i*(width-1)+17+(millis()/400+i)%2,instrumentApplicationIcons[i+menuStart],12,12,SSD1306_WHITE);
        if(i == cursor){
          stepchild.display.drawRoundRect(coords.start.x+2,coords.start.y+i*(width-1)+15+(millis()/400+i)%2,width,width,3,SSD1306_WHITE);
        }
        if(i==cursor)
          graphics.drawArrow(coords.start.x+21,coords.start.y+i*(width-1)+23+(millis()/400+i)%2,3,ARROW_RIGHT,true);
          // stepchild.display.fillRoundRect(coords.start.x+18,coords.start.y+i*(width-1)+15,84,width,3,1);
      }
      stepchild.display.display();
    }
};

void instrumentMenu(){
  InstrumentMenu menu;
  menu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  while(menu.instrumentMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
}
