#include "Stepchild.h"

#include "stringPatch.h"


;

void gridAnimation(bool in){
  //draws box, growing out from top left and bottom right corners
  //draws cursor, sliding in from top and left
  const uint8_t framerate = 12;
  if(in){
    for(int i = 0; i<=64; i+=framerate){
      stepchild.display.clearDisplay();
      //top
      stepchild.display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      stepchild.display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      stepchild.display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      stepchild.display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      stepchild.display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      stepchild.display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      graphics.drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      graphics.drawDottedLineH(64-i/2,64+i/2,32,3);

      stepchild.display.display();
    }
  }
  else{
    for(int i = 64; i>=0; i-=framerate){
      stepchild.display.clearDisplay();
      //top
      stepchild.display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      stepchild.display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      stepchild.display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      stepchild.display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      stepchild.display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      stepchild.display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      graphics.drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      graphics.drawDottedLineH(64-i/2,64+i/2,32,3);

      stepchild.display.display();
    }
  }
}

bool xyGrid(){
  int16_t xCoord = stepchild.buttons.getJoyX();
  int16_t yCoord = stepchild.buttons.getJoyY();

  int8_t offsetX = 0;
  int8_t offsetY = 0;

  uint8_t controlX = 1;
  uint8_t controlY = 2;

  uint8_t channelX = 1;
  uint8_t channelY = 1;

  bool pauseX = false;
  bool pauseY = false;
  gridAnimation(true);
  while(true){
    //a little averaging for smoother motion
    if(!pauseY)
      yCoord = (yCoord+stepchild.buttons.getJoyY())/2+offsetY;
      // yCoord = (yCoord+abs((analogRead(JOYSTICK_Y) - 5) * scaleF))/2;
    if(!pauseX)
      // xCoord = (xCoord+abs(128 - (analogRead(JOYSTICK_Y) - 5) * scaleF))/2;
      xCoord = (xCoord+stepchild.buttons.getJoyX())/2+offsetX;

    stepchild.display.clearDisplay();
    //rec/play icon
    if(stepchild.recording()){
      if(stepchild.clock.clockSource == EXTERNAL_CLOCK && !stepchild.clock.receivedClockMessage){
        if(stepchild.idlingUntilNoteReceived){
          if(millis()%1000>500){
            stepchild.display.drawCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          stepchild.display.drawCircle(3,3,3,SSD1306_WHITE);
      }
      else if((stepchild.clock.clockSource == EXTERNAL_CLOCK && stepchild.clock.receivedClockMessage) || stepchild.clock.clockSource == INTERNAL_CLOCK){
        if(stepchild.idlingUntilNoteReceived){
          if(millis()%1000>500){
            stepchild.display.fillCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          stepchild.display.fillCircle(3,3,3,SSD1306_WHITE);
      }
    }
    else if(stepchild.playing()){
      if(stepchild.clock.clockSource == EXTERNAL_CLOCK && !stepchild.clock.receivedClockMessage){
        stepchild.display.drawTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
      else{
        stepchild.display.fillTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
    }
    stepchild.display.drawRect(32,0,64,64,SSD1306_WHITE);
    //axes
    if(!pauseY)
      graphics.drawDottedLineV(64,0,64,3);
    if(!pauseX)
      graphics.drawDottedLineH(32,96,32,3);
    
    //drawing the cursor
    // stepchild.display.drawCircle(xCoord/2+34,64 - yCoord/2,5,SSD1306_WHITE);
    // stepchild.display.fillCircle(xCoord/2+34,64 - yCoord/2,3,SSD1306_WHITE);
    stepchild.display.drawFastHLine(xCoord/2+31,64 - yCoord/2,2,1);
    stepchild.display.drawFastHLine(xCoord/2+36,64 - yCoord/2,2,1);
    stepchild.display.drawFastVLine(xCoord/2+34,61 - yCoord/2,2,1);
    stepchild.display.drawFastVLine(xCoord/2+34,66 - yCoord/2,2,1);

    //printing values
    graphics.printSmall(40,57,"<"+stringify(channelX)+">",SSD1306_WHITE);
    stepchild.display.setRotation(1);
    graphics.printSmall(0,10,"[Sh] to lock axis",1);
    graphics.printSmall(7,34,"<"+stringify(channelY)+">",SSD1306_WHITE);
    stepchild.display.drawBitmap(11,25,ch_tiny,6,3,1);
    stepchild.display.setRotation(DISPLAY_UPRIGHT);
    stepchild.display.fillCircle(26,59,4,1);
    graphics.printSmall(25,57,"A",0);

    //printing controller numbers
    graphics.printSmall(81-stringify(controlX).length()*4,2,"<"+stringify(controlX)+">",SSD1306_WHITE);
    stepchild.display.setRotation(3);
    stepchild.display.drawBitmap(12,25,cc_tiny,5,3,1);
    graphics.printSmall(7,34,"<"+stringify(controlY)+">",SSD1306_WHITE);
    stepchild.display.setRotation(DISPLAY_UPRIGHT);
    stepchild.display.fillCircle(101,5,4,1);
    graphics.printSmall(100,3,"B",0);


    //x and y
    stepchild.display.setFont(&FreeSerifItalic24pt7b);
    stepchild.display.setCursor(35,22);
    if(!pauseX)
      stepchild.display.print("x");
    stepchild.display.setRotation(1);
    stepchild.display.setCursor(1,94);
    if(!pauseY)
      stepchild.display.print("y");
    stepchild.display.setRotation(DISPLAY_UPRIGHT);
    stepchild.display.setFont();

    //bar graphs
    graphics.drawBarGraphV(0,0,8,64,float(xCoord)/float(127));
    graphics.drawBarGraphV(120,0,8,64,float(yCoord)/float(127));

    if(stepchild.playing())
      graphics.drawPlayIcon(10+(millis()/200)%2,2);

    stepchild.display.display();

    //if you're not keybinding, keep sending vals
    if(!stepchild.buttons.SHIFT()){
      stepchild.midi.sendCC(controlX,xCoord,channelX);
      stepchild.midi.sendCC(controlY,yCoord,channelY);
      pauseX = false;
      pauseY = false;
    }

    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();

    //sending CC vals for keybinding
    if(stepchild.buttons.SHIFT()){
      if(stepchild.buttons.joystickX != 0){
        stepchild.midi.sendCC(controlX,xCoord,channelX);
        pauseY = true;
        pauseX = false;
      }
      else if(stepchild.buttons.joystickY != 0){
        stepchild.midi.sendCC(controlY,yCoord,channelY);
        pauseY = false;
        pauseX = true;
      }
    }

    while(stepchild.buttons.counterA != 0){
      //if shift is held, change channel
      if(stepchild.buttons.SHIFT()){
        if(stepchild.buttons.counterA >= 1 && channelY<16)
          channelY++;
        else if(stepchild.buttons.counterA <= -1 && channelY>0)
          channelY--;
      }
      //if not, change which CC val is sent
      else{
        if(stepchild.buttons.counterA >= 1 && controlY<127)
          controlY++;
        else if(stepchild.buttons.counterA <= -1 && controlY>0)
          controlY--;
      }
      stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;;
    }
    while(stepchild.buttons.counterB != 0){
      //if shift is held, change channel
      if(stepchild.buttons.SHIFT()){
        if(stepchild.buttons.counterB >= 1 && channelX<16)
          channelX++;
        else if(stepchild.buttons.counterB <= -1 && channelX>0)
          channelX--;
      }
      else{
        if(stepchild.buttons.counterB >= 1 && controlX<127)
          controlX++;
        else if(stepchild.buttons.counterB <= -1 && controlX>0)
          controlX--;
      }
      stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
    }
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        gridAnimation(false);
        return true;
      }
      //set offsets
      if(stepchild.buttons.LOOP()){
        stepchild.lastTime = millis();
        offsetX = 64-xCoord;
        offsetY = 64-yCoord;
      }
      if(stepchild.buttons.PLAY()){
        stepchild.togglePlay();
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.A()){
        pauseY = !pauseY;
        if(pauseY)
          pauseX = false;
        stepchild.lastTime = millis();
      }
      if(stepchild.buttons.B()){
        pauseX = !pauseX;
        if(pauseX)
          pauseY = false;
        stepchild.lastTime = millis();
      }
    }
  }
  return true;
}