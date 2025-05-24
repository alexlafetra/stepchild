void applySwingToSequence(){
  if(!selectNotes("swing")){
    return;
  }
  vector<NoteID> noteIDs = getSelectedNoteIDs();

  //get actual note objects from IDs, so you can delete all the notes
  vector<NoteTrackPair> notes;
  for(NoteID n:noteIDs){
    notes.push_back(NoteTrackPair(n.getNote(),n.track));
  }
  //delete all the notes
  sequence.deleteNotes_byID(noteIDs);

  //iterate over selected tracks
  for(NoteTrackPair note:notes){
    uint16_t startPos = note.note.startPos;
    uint16_t endPos = note.note.endPos;
    float timingOffset = sequenceClock.swingOffset(startPos);
    int32_t stepOffset = timingOffset/float(sequenceClock.uSecPerStep);
    //if the offset is big enough to register
    if(!(abs(stepOffset) < 1 || (stepOffset + int(startPos)) < 0)){
      //set the new note positions
      note.note.startPos = int32_t(startPos)+(stepOffset);
      note.note.endPos = int32_t(endPos)+(stepOffset);
    }
    
    //create the note
    sequence.makeNote(note.note,note.trackID,false);
  }
}


void tapBpm(){

  long t1 = 0;

  uint16_t lastButtonState = 0;
  uint16_t lastStepButtonState = 0;

  bool atLeastOnce = false;
  bool started = false;
  bool buttonsReleasedSinceTimeStarted = false;
  lastTime = millis();

  while(true){

    display.clearDisplay();
    display.drawFastHLine(0,16,128,1);
    display.drawCircle(78,32,9,1);

    if(t1){
      display.fillCircle(50,32,9,1);
      printSmall_centered(64,43,stringify(millis()-t1)+"ms",1);
    }
    else{
      display.drawCircle(50,32,9,1);
      printSmall_centered(64,43,"press any button to set tempo",1);
    }

    display.drawFastHLine(0,49,128,1);
    display.display();
    if(!utils.itsbeen(1000) && !atLeastOnce)
      continue;
    controls.readButtons();
    controls.readJoystick();
    if(controls.MENU()){
      lastTime = millis();
      break;
    }
//    if(started && !controls.mainButtonState && !controls.stepButtonState){
//      buttonsReleasedSinceTimeStarted = true;
//    }
//    if((controls.mainButtonState || controls.stepButtonState)){
//      if(!started){
//        started = true;
//        t1 = millis();
//        atLeastOnce = true;
//        buttonsReleasedSinceTimeStarted = false;
//      }
//      else{
//        sequenceClock.setBPM(float(60000)/float(millis()-t1));
//        t1 = 0;
//        buttonsReleasedSinceTimeStarted = false;
//        started = false;
//      }
//    }
  }
}

class ClockMenu:public StepchildMenu{
  public:
    float tVal = 0;
    float angle;
    bool internalMenuActive = false;
    uint8_t internalMenuCursor = 0;
    uint16_t bpmMetronomeState = 0;
    SequenceRenderSettings settings;
    ClockMenu(){
      tVal = micros();
      angle = 0;
      coords = CoordinatePair(0,0,35,64);
    }
    void displayMenu();
    void drawSmallStepchild(uint8_t, uint8_t);
    void drawSwingCurve(uint8_t,uint8_t,uint8_t,uint8_t);
    void updatePendulum();
    bool clockMenuControls();
    void updateStepButtons();
    void toggleClockSource(){
      if(sequenceClock.clockSource == INTERNAL_CLOCK)
        sequenceClock.clockSource = EXTERNAL_CLOCK;
      else if(sequenceClock.clockSource == EXTERNAL_CLOCK)
        sequenceClock.clockSource = INTERNAL_CLOCK;
    }
};


void ClockMenu::updatePendulum(){
  //for the clock pendulum
  if(sequenceClock.isSwinging)
    angle += 4.0*(micros()-tVal)/(sequenceClock.uSecPerStep)+(sequenceClock.swingCurve.amplitude/2000)*sin(PI*int(angle)/45);
  else
    angle += 4.0*(micros()-tVal)/(sequenceClock.uSecPerStep);
  tVal = micros();
}

void ClockMenu::drawSwingCurve(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
  int16_t oldY = h - mapVal(long(sequenceClock.swingOffset(0)),-long(sequenceClock.uSecPerStep),sequenceClock.uSecPerStep,0,h);
  for(uint8_t i = 1; i<w; i++){
    int16_t y1 = h - mapVal(long(sequenceClock.swingOffset(i*2)),-long(sequenceClock.uSecPerStep),sequenceClock.uSecPerStep,0,h);
    display.drawLine(i-1+x,oldY+y,i+x,y1+y,1);
    oldY = y1;
  }
  display.drawFastVLine(x+(sequence.playheadPos/2)%w,y,h,1);
}

void ClockMenu::updateStepButtons(){
  uint16_t LEDs = 1<<((millis()/sequenceClock.mSecPerStep()/24)%16);
  bpmMetronomeState = (millis()/sequenceClock.mSecPerStep()/24)%4;
  controls.writeLEDs(LEDs);
}

//Draws a small graphic of the Stepchild with the top left corner at (x1, y1)
void ClockMenu::drawSmallStepchild(uint8_t x1, uint8_t y1){
  //aspect ratio is ~11:9
  display.fillRoundRect(x1, y1, 22, 18, 3, SSD1306_WHITE);
  //screen
  display.fillRect(x1+5,y1+3,12,8,SSD1306_BLACK);
  //buttons
  for(uint8_t i = 0; i<4; i++){
    display.drawPixel(x1+2,y1+5+i*2,SSD1306_BLACK);
  }
  for(uint8_t i = 0; i<4; i++){
    display.drawPixel(x1+5+i*2,y1+15,SSD1306_BLACK);
  }
  //joystick
  display.fillCircle(x1+17,y1+14,2,SSD1306_BLACK);
  //encoders
  display.fillCircle(x1+19,y1+5,1,SSD1306_BLACK);
  display.fillCircle(x1+19,y1+9,1,SSD1306_BLACK);
}

/*
BPM submenu has:
  - bpm indicator
  - x2 & /2 buttons
  - instructions for how to change bpm
  - info on what it means, "Clock message is sent every 1/24th of a beat"
Swing submenu has:
  - swing on/off button
  - swing curve display
  - changing swing amplitude/period/phase/curve type
clock source submenu has
  - internal/external toggle
  - send/receive clock source on X port, or any
*/

void ClockMenu::displayMenu(){
  updatePendulum();
  updateStepButtons();
  String helptext;
  display.clearDisplay();
  //draw the sequence, if it's visible
  if(coords.start.x>32){
    drawSeq(settings);
  }

  //buttons
  graphics.drawButton(coords.start.x+28,coords.start.y+18,"BPM",cursor == 0);
  graphics.drawButton(coords.start.x+28,coords.start.y+27,"Swing",cursor == 1);
  graphics.drawButton(coords.start.x+28,coords.start.y+36,"src",cursor == 2);
  display.drawRoundRect(coords.start.x+52,coords.start.y+18,76,45,3,1);

  if(internalMenuActive){
    switch(cursor){
      //bpm
      case 0:
        switch(internalMenuCursor){
          case 0:
            helptext = "tap buttons to set tempo";
            break;
          case 1:
            helptext = "halve bpm";
            break;
          case 2:
            helptext = "double bpm";
            break;
        }
        break;
      case 1:
        switch(internalMenuCursor){
          case 0:
            helptext = stringify(sequenceClock.isSwinging?"disable":"enable")+" swing";
            break;
          case 1:
            helptext = "curve type: "+getCurveTypeString(sequenceClock.swingCurve.type);
            break;
          case 2:
            helptext = "amplitude";
            break;
          case 3:
            helptext = "subdivision";
            break;
          case 4:
            helptext = "phase";
            break;
        }
        break;
      case 2:
        helptext = "clock src: "+stringify((sequenceClock.clockSource == INTERNAL_CLOCK)?"stepchild":"external midi");
        break;
    }
  }

  //submenu info
  switch(cursor){
    //BPM
    case 0:{
      if(!internalMenuActive)
        helptext = "tempo";
      //bpm number
      String beatsPerMin = stringify(sequenceClock.BPM);
      while(beatsPerMin.length()<3){
        beatsPerMin = "0"+beatsPerMin;
      }
      print7SegString(coords.start.x+66,coords.start.y+28,beatsPerMin,false);
      printSmall(coords.start.x+54,coords.start.y+55,"set",1);
      graphics.drawButton(coords.start.x+66,coords.start.y+54,"A/B",true);
      graphics.drawArrow(coords.start.x+84,coords.start.y+54+(millis()/400)%2,2,ARROW_UP,true);

      //x2 & /2 buttons
      graphics.drawButton(coords.start.x+54,coords.start.y+20,"tap bpm",internalMenuCursor==0 && internalMenuActive);
      graphics.drawButton(coords.start.x+54,coords.start.y+29,"/2",internalMenuCursor==1 && internalMenuActive);
      graphics.drawButton(coords.start.x+54,coords.start.y+38,"x2",internalMenuCursor==2 && internalMenuActive);

      //metronome indicator
      for(uint8_t i = 0; i<4; i++){
        if(bpmMetronomeState == i){
          display.fillCircle(coords.start.x+119,coords.start.y+25+i*10,3,1);
        }
        else{
          display.drawCircle(coords.start.x+119,coords.start.y+25+i*10,3,1);
        }
      }
      break;
    }
    //swing
    case 1:
      if(!internalMenuActive)
        helptext = "swing properties";
      //swing curve
      printSmall(coords.start.x+80,56,"[N] to apply",1);
      graphics.drawDottedRect(coords.start.x+74,21,51,32,2);
      drawSwingCurve(coords.start.x+75,coords.start.y+21,50,32);

      //on/off
      graphics.drawButton(coords.start.x+54,coords.start.y+20,sequenceClock.isSwinging?"on":"off",internalMenuCursor==0 && internalMenuActive);
      //type
      graphics.drawButton(coords.start.x+54,coords.start.y+28,"type",internalMenuCursor==1 && internalMenuActive);
      //amplitude
      graphics.drawButton(coords.start.x+54,coords.start.y+37,stringify(float(sequenceClock.swingCurve.amplitude*100)/float(sequenceClock.uSecPerStep))+"%",internalMenuCursor==2 && internalMenuActive);
      //period
      graphics.drawButton(coords.start.x+54,coords.start.y+46,"$:",internalMenuCursor==3 && internalMenuActive);
      graphics.printFraction_small(coords.start.x+64,coords.start.y+47,stepsToMeasures(sequenceClock.swingCurve.period));
      //phase
      graphics.drawButton(coords.start.x+54,coords.start.y+54,"@:",internalMenuCursor==4 && internalMenuActive);
      printSmall(coords.start.x+64,coords.start.y+55,stringify(sequenceClock.swingCurve.phase),1);
      break;
    //source
    case 2:
      if(!internalMenuActive)
        helptext = "midi clock source";
      //src
      graphics.drawButton(coords.start.x+54,20,sequenceClock.clockSource==INTERNAL_CLOCK?"INTERNAL":"EXTERNAL",internalMenuCursor==0 && internalMenuActive);
      break;
  }

  //arrows
  if(internalMenuActive){
    switch(cursor){
      //bpm
      case 0:
        switch(internalMenuCursor){
          // tap bpm
          case 0:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+23,3,ARROW_RIGHT,false);
            break;
          // /2
          case 1:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+32,3,ARROW_RIGHT,false);
            break;
          // x2
          case 2:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+41,3,ARROW_RIGHT,false);
            break;
        }
        break;
      //swing
      case 1:
        switch(internalMenuCursor){
          case 0:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+23,3,ARROW_RIGHT,false);
            break;
          case 1:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+31,3,ARROW_RIGHT,false);
            break;
          case 2:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+40,3,ARROW_RIGHT,false);
            break;
          case 3:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+49,3,ARROW_RIGHT,false);
            break;
          case 4:
            graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+57,3,ARROW_RIGHT,false);
            break;
        }
        break;
      //src
      case 2:
        graphics.drawArrow(coords.start.x+54+(millis()/400)%2,coords.start.y+23,3,ARROW_RIGHT,false);
        break;
    }
  }
  else{
    switch(cursor){
      case 0:
        graphics.drawArrow(coords.start.x+42+(millis()/400)%2,coords.start.y+21,3,ARROW_LEFT,false);
        break;
      case 1:
        graphics.drawArrow(coords.start.x+50+(millis()/400)%2,coords.start.y+30,3,ARROW_LEFT,false);
        break;
      case 2:
        graphics.drawArrow(coords.start.x+42+(millis()/400)%2,coords.start.y+39,3,ARROW_LEFT,false);
        break;
    }

  }
  //title
  display.drawFastHLine(trackDisplay,headerHeight,screenWidth-trackDisplay,SSD1306_WHITE);
  display.setCursor(coords.start.x+30+coords.start.y-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Clock");
  display.setFont();
  //clock animation
  if(sequenceClock.clockSource == INTERNAL_CLOCK){
    graphics.drawPendulum(16,coords.start.y+18,26,angle);
    display.fillRect(10,coords.start.y+18,12,10,SSD1306_BLACK);
    display.drawBitmap(6,coords.start.y,clock_1_bmp,20,38,SSD1306_WHITE);
    
    //drawing hands on clock
    graphics.drawCircleRadian(15,coords.start.y+10,3,float(millis())/20.0,0);
  }
  else{
    graphics.drawPendulum(16,coords.start.y+18,26,90);
    display.fillRect(10,coords.start.y+18,12,10,SSD1306_BLACK);
    display.drawBitmap(6,coords.start.y,clock_1_bmp,20,38,SSD1306_WHITE);
    display.drawLine(15,coords.start.y+10,15,coords.start.y+13,SSD1306_BLACK);
  }
  printSmall_overflow(coords.start.x+70,coords.start.y,8,helptext,1);
  display.display();
}

bool ClockMenu::clockMenuControls(){
  controls.readButtons();
  controls.readJoystick();
  if(utils.itsbeen(200)){
    if(internalMenuActive){
      if(controls.MENU()){
        lastTime = millis();
        internalMenuActive = false;
        internalMenuCursor = 0;
      }
      if(controls.LEFT()){
        lastTime = millis();
        internalMenuActive = false;
        internalMenuCursor = 0;
      }
      switch(cursor){
        //BPM
        case 0:
          //changing cursor
          if(controls.DOWN() && internalMenuCursor){
            internalMenuCursor--;
            lastTime = millis();
          }
          if(controls.UP() && internalMenuCursor < 2){
            internalMenuCursor++;
            lastTime = millis();
          }
          while(controls.counterA != 0){
            if(controls.counterA > 0){
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM+10);
              else
                sequenceClock.setBPM(sequenceClock.BPM+1);
            }
            else{
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM-10);
              else
                sequenceClock.setBPM(sequenceClock.BPM-1);
            }
            controls.countDownA();
          }
          while(controls.counterB != 0){
            if(controls.counterB > 0){
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM+10);
              else
                sequenceClock.setBPM(sequenceClock.BPM+1);
            }
            else{
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM-10);
              else
                sequenceClock.setBPM(sequenceClock.BPM-1);
            }
            controls.countDownB();
          }
          if(controls.SELECT()){
            switch(internalMenuCursor){
              //tap bpm
              case 0:
                tapBpm();
                break;
              //div 2
              case 1:
                sequenceClock.setBPM(sequenceClock.BPM/2);
                break;
              //mult 2
              case 2:
                sequenceClock.setBPM(sequenceClock.BPM*2);
                break;
            }
            lastTime = millis();
          }
          break;
        //Swing
        case 1:
          if(controls.DOWN() && internalMenuCursor){
            internalMenuCursor--;
            lastTime = millis();
          }
          if(controls.UP() && internalMenuCursor < 4){
            internalMenuCursor++;
            lastTime = millis();
          }
          if(controls.NEW()){
            lastTime = millis();
            applySwingToSequence();
          }
          while(controls.counterA){
            switch(internalMenuCursor){
              //swing on/off
              case 0:
                sequenceClock.isSwinging = !sequenceClock.isSwinging;
                break;
              //type
              case 1:
                if(controls.counterA > 0){
                  sequenceClock.swingCurve.type++;
                }
                else{
                  sequenceClock.swingCurve.type--;
                }
                break;
              //amplitude
              case 2:
                if(controls.counterA > 0){
                  if(!controls.SHIFT()){
                    sequenceClock.swingCurve.amplitude+=sequenceClock.uSecPerStep/10;
                  }
                  else{
                    sequenceClock.swingCurve.amplitude+=sequenceClock.uSecPerStep/100;
                  }
                }
                else{
                  if(!controls.SHIFT()){
                    sequenceClock.swingCurve.amplitude-=sequenceClock.uSecPerStep/10;
                  }
                  else{
                    sequenceClock.swingCurve.amplitude-=sequenceClock.uSecPerStep/100;
                  }
                }
                if(abs(sequenceClock.swingCurve.amplitude)>sequenceClock.uSecPerStep)
                  sequenceClock.swingCurve.amplitude = sequenceClock.swingCurve.amplitude<0?-sequenceClock.uSecPerStep:sequenceClock.uSecPerStep;
                break;
              //subdiv
              case 3:
                if(controls.counterA > 0){
                  if(controls.SHIFT() && sequenceClock.swingCurve.period<768)
                    sequenceClock.swingCurve.period++;
                  else if(sequenceClock.swingCurve.period%24)
                    sequenceClock.swingCurve.period+=24-sequenceClock.swingCurve.period%24;
                  else if(sequenceClock.swingCurve.period<=384)
                    sequenceClock.swingCurve.period *= 2;
                }
                else{
                  if(controls.SHIFT() && sequenceClock.swingCurve.period>1)
                    sequenceClock.swingCurve.period--;
                  else if(sequenceClock.swingCurve.period%24){
                    sequenceClock.swingCurve.period-=sequenceClock.swingCurve.period%24;
                    if(sequenceClock.swingCurve.period == 0)
                      sequenceClock.swingCurve.period = 1;
                  }
                  else if(sequenceClock.swingCurve.period >= 2)
                    sequenceClock.swingCurve.period /= 2;
                }
                break;
              //phase
              case 4:
                if(controls.counterA > 0){
                  sequenceClock.swingCurve.phase++;
                }
                else if(sequenceClock.swingCurve.phase){
                  sequenceClock.swingCurve.phase--;
                }
                sequenceClock.swingCurve.phase%=sequenceClock.swingCurve.period;
                break;
            }
            controls.countDownA();
          }
          break;
        //Source
        case 2:
          //src
          if(controls.counterA && internalMenuCursor == 0){
            toggleClockSource();
            controls.countDownA();
          }
          if(controls.counterB && internalMenuCursor == 0){
            toggleClockSource();
            controls.countDownB();
          }
          break;
      }
    }
    //if you're not in a submenu
    else{
      if(cursor == 0){
          while(controls.counterA != 0){
            if(controls.counterA > 0){
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM+10);
              else
                sequenceClock.setBPM(sequenceClock.BPM+1);
            }
            else{
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM-10);
              else
                sequenceClock.setBPM(sequenceClock.BPM-1);
            }
            controls.countDownA();
          }
          while(controls.counterB != 0){
            if(controls.counterB > 0){
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM+10);
              else
                sequenceClock.setBPM(sequenceClock.BPM+1);
            }
            else{
              if(!controls.SHIFT())
                sequenceClock.setBPM(sequenceClock.BPM-10);
              else
                sequenceClock.setBPM(sequenceClock.BPM-1);
            }
            controls.countDownB();
          }      
      }
      if(controls.MENU()){
        lastTime = millis();
        return false;
      }
      if(controls.PLAY()){
        sequence.togglePlay();
        lastTime = millis();
      }
      //jumping into the internal menu
      if(controls.RIGHT()){
        internalMenuActive = true;
        internalMenuCursor = 0;
        lastTime = millis();
      }
      if(controls.DOWN() && cursor>0){
        cursor--;
        lastTime = millis();
      }
      else if(controls.UP()&& cursor<2){
        cursor++;
        lastTime = millis();
      }
      if(controls.SELECT() ){
        internalMenuActive = true;
        lastTime = millis();
      }
    }
  }
  return true;
}

void clockMenu(){
  ClockMenu clockMenu;
  clockMenu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  while(clockMenu.clockMenuControls()){
    clockMenu.displayMenu();
  }
  clockMenu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
}
