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
      controls.readButtons();
      controls.readJoystick();
      if(utils.itsbeen(100)){
        if(controls.UP() && cursor < NUMBER_OF_APPLICATIONS-1){
          if(cursor<2)
            cursor++;
          else if(menuStart < (NUMBER_OF_APPLICATIONS-3)){
            menuStart++;
          }
          lastTime = millis();
        }
        if(controls.DOWN()){
          if(cursor > 0){
            cursor--;
          }
          else if(menuStart>0){
            menuStart--;
          }
          lastTime = millis();
        }
      }
      if(utils.itsbeen(200)){
        if(controls.MENU()){
          lastTime = millis();
          return false;
        }
        if(controls.SELECT() ){
          lastTime = millis();
          slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
          instrumentApplicationFunctions[cursor+menuStart]();
          slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
        }
      }
      return true;
    }
    void displayMenu(){
      display.clearDisplay();
      settings.drawSteps = coords.start.y>headerHeight;
      drawSeq(settings);
      //drawing menu box (+16 so the title is transparent)
      display.fillRect(coords.start.x,coords.start.y+13, coords.end.x-coords.start.x, coords.end.y-coords.start.y, SSD1306_BLACK);
      display.drawRect(coords.start.x,coords.start.y+12, coords.end.x-coords.start.x, coords.end.y-coords.start.y-12, SSD1306_WHITE);

      //if the title will be on screen
      if(coords.start.x+coords.start.y-1<screenWidth){
        display.setCursor(coords.start.x+coords.start.y-1,5);
        display.setFont(&FreeSerifItalic9pt7b);
        display.print("Apps");
        display.setFont();
      }
      
      String text = instrumentApplicationTitles[cursor+menuStart];
      printChunky(coords.start.x+coords.start.y+31,17,text,SSD1306_WHITE);

      //say if it's a tool or generator
      display.drawFastHLine(coords.start.x+coords.start.y+21,20,8,1);
      if((menuStart+cursor)>NUMBER_OF_GENERATIVE_INSTRUMENTS){
        display.drawBitmap(coords.start.x+coords.start.y+48,4,tool_bmp,19,6,1);
      }
      else{
        display.drawBitmap(coords.start.x+coords.start.y+48,4,generator_bmp,44,6,1);
      }

      //printing arrows
      if(menuStart>0)
        graphics.drawArrow(coords.start.x+coords.start.y+38,1+(millis()/400+1)%2,2,2,true);
      if(menuStart<NUMBER_OF_APPLICATIONS - 3)
        graphics.drawArrow(coords.start.x+coords.start.y+38,10+(millis()/400)%2,2,3,true);

      //frame for the info text
      display.drawRoundRect(coords.start.x+19,coords.start.y+14,82,47,3,1);
      text = instrumentApplicationInfo[cursor+menuStart];
      printSmall_overflow(coords.start.x+23,coords.start.y+24,4,text,1);
      const uint8_t width = 16;
      for(uint8_t i = 0; i<3; i++){
        display.drawBitmap(coords.start.x+4,coords.start.y+i*(width-1)+17+(millis()/400+i)%2,instrumentApplicationIcons[i+menuStart],12,12,SSD1306_WHITE);
        if(i == cursor){
          display.drawRoundRect(coords.start.x+2,coords.start.y+i*(width-1)+15+(millis()/400+i)%2,width,width,3,SSD1306_WHITE);
        }
        if(i==cursor)
          graphics.drawArrow(coords.start.x+21,coords.start.y+i*(width-1)+23+(millis()/400+i)%2,3,ARROW_RIGHT,true);
          // display.fillRoundRect(coords.start.x+18,coords.start.y+i*(width-1)+15,84,width,3,1);
      }
      display.display();
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
