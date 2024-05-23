
void gridAnimation(bool in){
  //draws box, growing out from top left and bottom right corners
  //draws cursor, sliding in from top and left
  const uint8_t framerate = 12;
  if(in){
    for(int i = 0; i<=64; i+=framerate){
      display.clearDisplay();
      //top
      display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      graphics.drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      graphics.drawDottedLineH(64-i/2,64+i/2,32,3);

      display.display();
    }
  }
  else{
    for(int i = 64; i>=0; i-=framerate){
      display.clearDisplay();
      //top
      display.drawLine(32,0,32+i,0,SSD1306_WHITE);
      //left
      display.drawLine(32,0,32,0+i,SSD1306_WHITE);
      //bottom
      display.drawLine(96,63,96-i,63,SSD1306_WHITE);
      //right
      display.drawLine(96,63,96,63-i,SSD1306_WHITE);
      //inner circle
      display.fillCircle(0+i,32,3,SSD1306_WHITE);
      //outer circle
      display.drawCircle(64,0+i/2,5,SSD1306_WHITE);
      //dotted vert
      graphics.drawDottedLineV(64,32-i/2,32+i/2,3);
      //dotted horz
      graphics.drawDottedLineH(64-i/2,64+i/2,32,3);

      display.display();
    }
  }
}

//range is (5-1023) aka 0-1018
const float joystickScaleFactor = float(128)/float(1018);

int16_t getJoyX(){
  return abs(analogRead(JOYSTICK_X) * joystickScaleFactor);
}

int16_t getJoyY(){
  return abs(128 - constrain(analogRead(JOYSTICK_Y)*joystickScaleFactor,0,128));
}

void xyGrid(){
  int16_t xCoord = getJoyX();
  int16_t yCoord = getJoyY();

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
      yCoord = (yCoord+getJoyY())/2+offsetY;
      // yCoord = (yCoord+abs((analogRead(JOYSTICK_Y) - 5) * scaleF))/2;
    if(!pauseX)
      // xCoord = (xCoord+abs(128 - (analogRead(JOYSTICK_Y) - 5) * scaleF))/2;
      xCoord = (xCoord+getJoyX())/2+offsetX;

    display.clearDisplay();
    //rec/play icon
    if(recording){
      if(clockSource == EXTERNAL_CLOCK && !gotClock){
        if(waitingToReceiveANote){
          if(millis()%1000>500){
            display.drawCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          display.drawCircle(3,3,3,SSD1306_WHITE);
      }
      else if((clockSource == EXTERNAL_CLOCK && gotClock) || clockSource == INTERNAL_CLOCK){
        if(waitingToReceiveANote){
          if(millis()%1000>500){
            display.fillCircle(3,3,3,SSD1306_WHITE);
          }
        }
        else
          display.fillCircle(3,3,3,SSD1306_WHITE);
      }
    }
    else if(playing){
      if(clockSource == EXTERNAL_CLOCK && !gotClock){
        display.drawTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
      else{
        display.fillTriangle(0,6,0,0,6,3,SSD1306_WHITE);
      }
    }
    display.drawRect(32,0,64,64,SSD1306_WHITE);
    //axes
    if(!pauseY)
      graphics.drawDottedLineV(64,0,64,3);
    if(!pauseX)
      graphics.drawDottedLineH(32,96,32,3);
    
    //drawing the cursor
    // display.drawCircle(xCoord/2+34,64 - yCoord/2,5,SSD1306_WHITE);
    // display.fillCircle(xCoord/2+34,64 - yCoord/2,3,SSD1306_WHITE);
    display.drawFastHLine(xCoord/2+31,64 - yCoord/2,2,1);
    display.drawFastHLine(xCoord/2+36,64 - yCoord/2,2,1);
    display.drawFastVLine(xCoord/2+34,61 - yCoord/2,2,1);
    display.drawFastVLine(xCoord/2+34,66 - yCoord/2,2,1);

    //printing values
    printSmall(40,57,"<"+stringify(channelX)+">",SSD1306_WHITE);
    display.setRotation(1);
    printSmall(7,34,"<"+stringify(channelY)+">",SSD1306_WHITE);
    display.drawBitmap(8,26,ch_tiny,6,3,1);
    display.setRotation(UPRIGHT);

    //printing controller numbers
    printSmall(81-stringify(controlX).length()*4,2,"<"+stringify(controlX)+">",SSD1306_WHITE);
    display.setRotation(3);
    display.drawBitmap(8,26,cc_tiny,5,3,1);
    printSmall(7,34,"<"+stringify(controlY)+">",SSD1306_WHITE);
    display.setRotation(UPRIGHT);

    //x and y
    display.setFont(&FreeSerifItalic24pt7b);
    display.setCursor(35,22);
    if(!pauseX)
      display.print("x");
    display.setRotation(1);
    display.setCursor(1,94);
    if(!pauseY)
      display.print("y");
    display.setRotation(UPRIGHT);
    display.setFont();

    //bar graphs
    graphics.drawBarGraphV(0,0,8,64,float(xCoord)/float(127));
    graphics.drawBarGraphV(120,0,8,64,float(yCoord)/float(127));

    if(playing)
      drawPlayIcon(10+(millis()/200)%2,2);

    display.display();

    //if you're not keybinding, keep sending vals
    if(!controls.SHIFT()){
      MIDI.sendCC(controlX,xCoord,channelX);
      MIDI.sendCC(controlY,yCoord,channelY);
      pauseX = false;
      pauseY = false;
    }

    controls.readButtons();
    controls.readJoystick();

    //sending CC vals for keybinding
    if(controls.SHIFT()){
      if(controls.joystickX != 0){
        MIDI.sendCC(controlX,xCoord,channelX);
        pauseY = true;
        pauseX = false;
      }
      else if(controls.joystickY != 0){
        MIDI.sendCC(controlY,yCoord,channelY);
        pauseY = false;
        pauseX = true;
      }
    }

    while(controls.counterA != 0){
      //if shift is held, change channel
      if(controls.SHIFT()){
        if(controls.counterA >= 1 && channelY<16)
          channelY++;
        else if(controls.counterA <= -1 && channelY>0)
          channelY--;
      }
      //if not, change which CC val is sent
      else{
        if(controls.counterA >= 1 && controlY<127)
          controlY++;
        else if(controls.counterA <= -1 && controlY>0)
          controlY--;
      }
      controls.counterA += controls.counterA<0?1:-1;;
    }
    while(controls.counterB != 0){
      //if shift is held, change channel
      if(controls.SHIFT()){
        if(controls.counterB >= 1 && channelX<16)
          channelX++;
        else if(controls.counterB <= -1 && channelX>0)
          channelX--;
      }
      else{
        if(controls.counterB >= 1 && controlX<127)
          controlX++;
        else if(controls.counterB <= -1 && controlX>0)
          controlX--;
      }
      controls.counterB += controls.counterB<0?1:-1;;
    }
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        lastTime = millis();
        gridAnimation(false);
        return;
      }
      //set offsets
      if(controls.LOOP()){
        lastTime = millis();
        offsetX = 64-xCoord;
        offsetY = 64-yCoord;
      }
      if(controls.PLAY()){
        togglePlayMode();
        lastTime = millis();
      }
      if(controls.A()){
        pauseY = !pauseY;
        if(pauseY)
          pauseX = false;
        lastTime = millis();
      }
      if(controls.B()){
        pauseX = !pauseX;
        if(pauseX)
          pauseY = false;
        lastTime = millis();
      }
    }
  }
}