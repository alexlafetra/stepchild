/*

  Code for the random menu, and the genRandom() & selectArea_random() functions

*/

CoordinatePair selectArea_random(){
  CoordinatePair coords;
  coords.start.x = 0;
  coords.end.x = 0;
  coords.start.y = 0;
  coords.end.y = 0;
  WireFrame dieModel = genRandMenuObjects(12,0,10,0.5);
  while(true){
    controls.readJoystick();
    controls.readButtons();
    defaultEncoderControls();
    if(controls.SELECT()  && !selBox.begun && (controls.joystickX != 0 || controls.joystickY != 0)){
      selBox.begun = true;
      selBox.coords.start.x = sequence.cursorPos;
      selBox.coords.start.y = sequence.activeTrack;
      coords.start.x = sequence.cursorPos;
      coords.start.y = sequence.activeTrack;
    }
    //if controls.SELECT()  is released, and there's a selection box
    if(!controls.SELECT()  && selBox.begun){
      selBox.coords.end.x = sequence.cursorPos;
      selBox.coords.end.y = sequence.activeTrack;
      selBox.begun = false;
      coords.end.x = sequence.cursorPos;
      coords.end.y = sequence.activeTrack;
    }
    if(utils.itsbeen(200)){
      if(controls.NEW()){
        lastTime = millis();
        return coords;
      }
      if(controls.MENU()){
        coords.start.x = 0;
        coords.end.x = 0;
        coords.start.y = 0;
        coords.end.y = 0;
        lastTime = millis();
        return coords;
      }
    }
    if (utils.itsbeen(100)) {
      if (controls.joystickX == 1 && !controls.SHIFT()) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(sequence.cursorPos%sequence.subDivision){
          sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          sequence.moveCursor(-sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickX == -1 && !controls.SHIFT()) {
        if(sequence.cursorPos%sequence.subDivision){
          sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
          lastTime = millis();
        }
        else{
          sequence.moveCursor(sequence.subDivision);
          lastTime = millis();
        }
      }
      if (controls.joystickY == 1) {
        if(sequence.recording())
          sequence.setActiveTrack(sequence.activeTrack + 1, false);
        else
          sequence.setActiveTrack(sequence.activeTrack + 1, true);
        lastTime = millis();
      }
      if (controls.joystickY == -1) {
        if(sequence.recording())
          sequence.setActiveTrack(sequence.activeTrack - 1, false);
        else
          sequence.setActiveTrack(sequence.activeTrack - 1, true);
        lastTime = millis();
      }
    }
    if (utils.itsbeen(50)) {
      if (controls.joystickX == 1 && controls.SHIFT()) {
        sequence.moveCursor(-1);
        lastTime = millis();
      }
      if (controls.joystickX == -1 && controls.SHIFT()) {
        sequence.moveCursor(1);
        lastTime = millis();
      }
    }
    dieModel.rotate(1,0);
    dieModel.rotate(1,1);
    display.clearDisplay();
    SequenceRenderSettings settings;
    settings.drawPram = false;
    settings.topLabels = false;
    drawSeq(settings);
    drawCoordinateBox(coords,settings);
    if(sequence.shrinkTopDisplay){
      dieModel.scale = 0.25;
      dieModel.yPos = 4;
      dieModel.drawDots = false;
      dieModel.renderDie();
    }
    else{
      dieModel.scale = 0.5;
      dieModel.yPos = 8;
      dieModel.drawDots = true;
      dieModel.renderDie();
    }
    if(coords.start.x == 0 && coords.end.x == 0 && coords.start.y == 0 && coords.end.y == 0){
      printSmall(trackDisplay,0,"select an area!",1);
    }
    else{
      printSmall(trackDisplay,0,"[n] to randomize",1);
    }
    display.display();
  }
}

void genRandom(RandomData randData){
  while(true){
    CoordinatePair coords = selectArea_random();
    if(coords.start.x == coords.end.x && coords.start.y == coords.end.y){
      break;
    }
    if(coords.start.x>coords.end.x){
      uint16_t temp = coords.start.x;
      coords.start.x = coords.end.x;
      coords.end.x = temp;
    }
    if(coords.start.y>coords.end.y){
      uint8_t temp = coords.start.y;
      coords.start.y = coords.end.y;
      coords.end.y = temp;
    }
    //iterate over the tracks (inclusively)
    for(uint8_t t = coords.start.y; t<=coords.end.y; t++){
      for(uint16_t step = coords.start.x; step<coords.end.x; step+=randData.everyNSteps){
        //if the step isn't a start pos
        if(sequence.lookupTable[t][step] == 0 || sequence.noteAt(t,step).startPos != t){
          if(random(0,100)<randData.odds){
            uint8_t chance = random(randData.minChance,randData.maxChance+1);
            uint16_t length = random(randData.minLength,randData.maxLength+1);
            uint8_t vel = random(randData.minVel,randData.maxVel+1);
            //making sure notes won't run off the end
            if(step+length>coords.end.x){
              length = coords.end.x-step;
            }
            Note newNote = Note(step,step+length,vel,chance,false,false);
            sequence.makeNote(newNote, t);
          }
        }
      }
    }
  }
}
/*
  Note options: vel, chance, length
  placement options: odds of a note being placed, where the note is placed
*/
class RandomMenu:public StepchildMenu{
  public:
    WireFrame icon;
    SequenceRenderSettings settings;
    RandomMenu(){
      settings.topLabels = false;
      coords = CoordinatePair(44,0,128,64);
      icon = genRandMenuObjects(112,13,10,1);
      icon.scale = 0.8;
    }
    void displayMenu(){
      icon.rotate(1,0);
      icon.rotate(1,1);
      display.clearDisplay();
      drawSeq(settings);
      printSmall(32,0,"[N] to add notes",1);
      display.fillRoundRect(coords.start.x-2,12,coords.end.x-coords.start.x+2,coords.end.y-12,5,0);
      display.drawRoundRect(coords.start.x-2,12,coords.end.x-coords.start.x+5,coords.end.y-12,5,1);
      display.drawFastHLine(coords.start.x+51,12,77-coords.start.x,0);
      display.drawFastVLine(coords.start.x+51,0,13,1);
      icon.renderDie();

      //min/max chart
      const uint8_t x = 54;
      const uint8_t y = coords.start.y+28;
      printSmall(coords.start.x+x,y,"min",1);
      display.fillCircle(coords.start.x+x+5,y+10,4,1);
      printSmall(coords.start.x+x+4,y+8,"B",0);

      //dividing line on chart
      display.drawFastVLine(coords.start.x+x+13,y,64-y,1);
      //max
      printSmall(coords.start.x+x+16,y,"max",1);
      display.fillCircle(coords.start.x+x+21,y+10,4,1);
      printSmall(coords.start.x+x+20,y+8,"A",0);

      //placement
      printChunky(coords.start.x,coords.start.y+16,"placement",1);
      printSmall(coords.start.x+7,coords.start.y+23,"odds:",1);
      printSmall(coords.start.x+30,coords.start.y+23,stringify(randomData.odds)+"%",1);
      if(cursor == 0){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+25,2,ARROW_RIGHT,true);
      }
      printSmall(coords.start.x+7,coords.start.y+29,"every:",1);
      printSmall(coords.start.x+30,coords.start.y+29,stepsToMeasures(randomData.everyNSteps),1);
      if(cursor == 1){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+31,2,ARROW_RIGHT,true);
      }

      //note options
      printChunky(coords.start.x,coords.start.y+37,"notes",1);

      //vel
      printSmall(coords.start.x+7,coords.start.y+44,"velocity:",1);
      String s  = stringify(randomData.minVel);
      printSmall(coords.start.x+x+12-getSmallTextLength(s),coords.start.y+44,s,1);
      s = stringify(randomData.maxVel);
      printSmall(coords.start.x+x+15,coords.start.y+44,s,1);
      if(cursor == 2){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+46,2,ARROW_RIGHT,true);
      }

      //length
      printSmall(coords.start.x+7,coords.start.y+50,"length:",1);
      s = stepsToMeasures(randomData.minLength);
      printSmall(coords.start.x+x+12-getSmallTextLength(s),coords.start.y+50,s,1);
      s = stepsToMeasures(randomData.maxLength);
      printSmall(coords.start.x+x+15,coords.start.y+50,s,1);
      if(cursor == 3){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+52,2,ARROW_RIGHT,true);
      }

      //chance
      printSmall(coords.start.x+7,coords.start.y+57,"chance:",1);
      s  = stringify(randomData.minChance);
      printSmall(coords.start.x+x+12-getSmallTextLength(s),coords.start.y+57,s,1);
      s = stringify(randomData.maxChance);
      printSmall(coords.start.x+x+15,coords.start.y+57,s,1);
      if(cursor == 4){
        graphics.drawArrow(coords.start.x+4+(millis()/300)%2,coords.start.y+59,2,ARROW_RIGHT,true);
      }
      display.display();
    }
    bool randomMenuControls(){
      controls.readButtons();
      controls.readJoystick();
      if(utils.itsbeen(200)){
        if(controls.MENU()){
          lastTime = millis();
          return false;
        }
        if(controls.DOWN() && cursor > 0){
          cursor--;
          lastTime = millis();
        }
        if(controls.UP() && cursor < 4){
          cursor++;
          lastTime = millis();
        }
        if(controls.NEW()){
          slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
          lastTime = millis();
          genRandom(randomData);
          slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
        }
        while(controls.counterB){
          switch(cursor){
            //odds of a note appearing
            case 0:{
              int8_t temp = randomData.odds;
              int8_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterB<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                controls.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterB = 0;
              }
              randomData.odds = temp;
            }
              break;
            //place each note is attempted to be placed ('every n steps')
            case 1:
              if(controls.SHIFT()){
                randomData.everyNSteps = toggleTriplets(randomData.everyNSteps);
              }
              randomData.everyNSteps = changeSubDiv(controls.counterB>0,randomData.everyNSteps,false);
              break;
            //velocity
            case 2:{
              int16_t temp = randomData.maxVel;
              int16_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterB<0?-increment:increment; 
              if(temp > 127){
                temp = 127;
                controls.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterB = 0;
              }
              randomData.maxVel = temp;
            }
              break;
            //length
            case 3:
              if(controls.SHIFT()){
                randomData.maxLength = toggleTriplets(randomData.maxLength);
              }
              randomData.maxLength = changeSubDiv(controls.counterB>0,randomData.maxLength,false);
              break;
            //chance
            case 4:{
              int8_t temp = randomData.maxChance;
              int8_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterB<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                controls.counterB = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterB = 0;
              }
              randomData.maxChance = temp;
            }
              break;
          }
          controls.countDownB();
        }
        while(controls.counterA){
          switch(cursor){
            //odds of a note appearing
            case 0:{
              int8_t temp = randomData.odds;
              int8_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterA<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                controls.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterA = 0;
              }
                randomData.odds = temp;
            }
              break;
            case 1:
              if(controls.SHIFT()){
                randomData.everyNSteps = toggleTriplets(randomData.everyNSteps);
              }
              randomData.everyNSteps = changeSubDiv(controls.counterA>0,randomData.everyNSteps,false);
              break;
            case 2:{
              int16_t temp = randomData.minVel;
              int16_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterA<0?-increment:increment; 
              if(temp > 127){
                temp = 127;
                controls.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterA = 0;
              }
              randomData.minVel = temp;
            }
              break;
            case 3:
              if(controls.SHIFT()){
                randomData.minLength = toggleTriplets(randomData.minLength);
              }
              randomData.minLength = changeSubDiv(controls.counterA>0,randomData.minLength,false);
              break;
            case 4:{
              int8_t temp = randomData.minChance;
              int8_t increment = 10;
              if(controls.SHIFT()){
                increment = 1;
              }
              temp += controls.counterA<0?-increment:increment; 
              if(temp > 100){
                temp = 100;
                controls.counterA = 0;
              }
              if(temp < 0){
                temp = 0;
                controls.counterA = 0;
              }
              randomData.minChance = temp;
            }
              break;
          }
          controls.countDownA();
        }
      }
      return true;
    }
    
};

bool randomMenu(){
  RandomMenu menu;
  menu.slideIn(IN_FROM_RIGHT,MENU_SLIDE_FAST);
  while(menu.randomMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_RIGHT,MENU_SLIDE_FAST);
  return false;
}
