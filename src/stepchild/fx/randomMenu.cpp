WireFrame genRandMenuObjects(uint8_t x1, uint8_t y1, uint8_t distance, float scale);


//Random Menu
void drawRandMenuOptions(uint8_t which,RandomData randData){
  switch(which){
  //length
  case 0:
    {
    const uint8_t x1 = 46;
    const uint8_t y1 = 10;
    printSmall(x1-8,y1,"length of notes",1);
    printSmall(x1-2,y1+24,"max",1);
    String frac = stepsToMeasures(randData.maxLength);
    graphics.printFraction_small(x1-(frac.length()>=4?5:0),y1+15,frac);
    printSmall(x1+29,y1+14,"min",1);
    frac = stepsToMeasures(randData.minLength);
    graphics.printFraction_small(x1+29,y1+23,frac);
    int8_t offset1 = 4*((millis()/200)%2);
    int8_t offset2 = 4*cos(millis()/100);
    display.fillRect(x1+12,y1+24-randData.maxLength/6-offset1/2,6,randData.maxLength/3+offset1,1);
    display.fillRect(x1+20,y1+24-randData.minLength/6-offset2/2,6,randData.minLength/3+offset2,1);
    }
    break;
    //odds
    case 1:
      {
      const uint8_t x1 = 46;
      const uint8_t y1 = 10;
      printSmall(x1,y1,"odds of a",1);
      printSmall(x1-10,y1+7,"note appearing",1);
      //formatting the string
      String val = stringify(randData.odds);
      while(val.length()<3){
        val = "0" + val;
      }
      print7SegItalic_wiggly(x1-10,y1+18,val);
      printSmall(x1+40,y1+14,"%",1);
      printSmall(x1-12,56,"[n] to randomize",1);
      }
      break;
  //probability
  case 2:
    {
    const uint8_t x1 = 36;
    const uint8_t y1 = 14;
    display.drawCircle(x1+28,y1+20,randData.maxChance/6,1);
    display.fillCircle(x1+28,y1+20,randData.minChance/6,1);
    uint8_t diff = ceil(float(randData.maxChance)/8.0-float(randData.minChance)/8.0);
    if(!diff){
        diff = 1;
    }   
    display.drawCircle(x1+28,y1+20,randData.minChance/8+(millis()/50)%diff+1,1);
    printSmall(x1,y1+((millis()/400)%2),"max",1);
    printSmall(x1,y1+6+((millis()/400)%2),stringify(randData.maxChance),1);
    printSmall(x1+45,y1+34+cos(millis()/200),"min",1);
    printSmall(x1+45,y1+40+cos(millis()/200),stringify(randData.minChance),1);
    printSmall(x1,y1+40,"chance",1);
    }
    break;
  //target
  case 3:
    {
    const uint8_t x1 = 36;
    const uint8_t y1 = 14;
    printSmall(x1+8,y1,"randomize...",1);
      if(randData.target){
        printArp_wiggly(x1+8,y1+16,"Notes",1);
        printSmall_centered(64,y1+32,"(alter existing notes)",1);
      }
      else{
        printArp_wiggly(x1+8,y1+16,"Empty",1);
        printSmall_centered(64,y1+32,"(fill blank spaces)",1);
      }
    }
    break;
    //velocity
    case 4:
      {
      const uint8_t x1 = 36;
      const uint8_t y1 = 14;
      display.drawCircle(x1+28,y1+20,randData.maxVel/8,1);
      display.fillCircle(x1+28,y1+20,randData.minVel/8,1);
      uint8_t diff = randData.maxVel/8-randData.minVel/8;
      for(uint8_t i = (randData.minVel/8)+1; i<(randData.maxVel/8-diff*abs(sin(float(millis())/float(200))))-1; i+=2){
        display.drawCircle(x1+28,y1+20,i,1);
      }
      printSmall(x1,y1+((millis()/400)%2),"max",1);
      printSmall(x1,y1+6+((millis()/400)%2),stringify(randData.maxVel),1);
      printSmall(x1+45,y1+34+cos(millis()/200),"min",1);
      printSmall(x1+45,y1+40+cos(millis()/200),stringify(randData.minVel),1);
      printSmall(x1,y1+40,"velocity",1);
      }
      break;
    //grid
    case 5:
      {
      const uint8_t x1 = 44;
      const uint8_t y1 = 10;
      if(randData.onlyOnGrid){
        printArp_wiggly(x1,y1+10,"On Grid",1);
        printSmall_centered(64,y1+25,"locked to grid:",1);
        graphics.printFraction_small_centered(62,y1+35,stepsToMeasures(subDivInt));
      }
      else{
        printArp_wiggly(x1-4,y1+10,"Off Grid",1);
        printSmall_centered(x1+24,y1+25,"create notes anywhere!",1);
      }
      }
      break;
  }
}

void drawCoordinateBox(CoordinatePair coords){
  if(!selBox.begun && (coords.x1 != coords.x2)){
    //correcting bounds for view
    unsigned short int X1;
    unsigned short int X2;
    unsigned short int Y1;
    unsigned short int Y2;

    if(coords.x1>coords.x2){
      X1 = coords.x2;
      X2 = coords.x1;
    }
    else{
      X1 = coords.x1;
      X2 = coords.x2;
    }
    if(coords.y1>coords.y2){
      Y1 = coords.y2;
      Y2 = coords.y1;
    }
    else{
      Y1 = coords.y1;
      Y2 = coords.y2;
    }

    //if it's offscreen, return
    if(X2<=viewStart || X1>=viewEnd || Y1 > startTrack+maxTracksShown || Y2<startTrack){
      return;
    }

    if(X1<viewStart){
      X1 = viewStart;
    }
    if(X2>viewEnd){
      X2 = viewEnd;
    }
    if(Y1<startTrack){
      Y1 = startTrack;
    }
    if(Y2>(startTrack+maxTracksShown)){
      Y2 = startTrack+maxTracksShown;
    }
    uint8_t startX = trackDisplay+(X1-viewStart)*scale;
    uint8_t length = (X2-X1)*scale;
    // uint8_t startHeight = maxTracksShown==5?headerHeight:8;
    uint8_t startHeight = headerHeight;
    uint8_t startY = (Y1-startTrack)*trackHeight+startHeight;
    uint8_t height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
   
   if((millis())%400>200){
      graphics.shadeRect(startX,startY,length,height,3);
    }
    else{
      display.drawRect(startX,startY,length,height,1);
    }
  }
}

CoordinatePair selectArea_random(){
  CoordinatePair coords;
  coords.x1 = 0;
  coords.x2 = 0;
  coords.y1 = 0;
  coords.y2 = 0;
  WireFrame graphics = genRandMenuObjects(16,8,10,0.5);
  while(true){
    readJoystick();
    readButtons();
    defaultEncoderControls();
    if(sel && !selBox.begun && (x != 0 || y != 0)){
      selBox.begun = true;
      selBox.x1 = cursorPos;
      selBox.y1 = activeTrack;
      coords.x1 = cursorPos;
      coords.y1 = activeTrack;
    }
    //if sel is released, and there's a selection box
    if(!sel && selBox.begun){
      selBox.x2 = cursorPos;
      selBox.y2 = activeTrack;
      selBox.begun = false;
      coords.x2 = cursorPos;
      coords.y2 = activeTrack;
    }
    if(itsbeen(200)){
      if(n){
        lastTime = millis();
        return coords;
      }
      if(menu_Press){
        coords.x1 = 0;
        coords.x2 = 0;
        coords.y1 = 0;
        coords.y2 = 0;
        lastTime = millis();
        return coords;
      }
    }
    if (itsbeen(100)) {
      if (x == 1 && !shift) {
        //if cursor isn't on a measure marker, move it to the nearest one
        if(cursorPos%subDivInt){
          moveCursor(-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(-subDivInt);
          lastTime = millis();
        }
      }
      if (x == -1 && !shift) {
        if(cursorPos%subDivInt){
          moveCursor(subDivInt-cursorPos%subDivInt);
          lastTime = millis();
        }
        else{
          moveCursor(subDivInt);
          lastTime = millis();
        }
      }
      if (y == 1) {
        if(recording)
          setActiveTrack(activeTrack + 1, false);
        else
          setActiveTrack(activeTrack + 1, true);
        lastTime = millis();
      }
      if (y == -1) {
        if(recording)
          setActiveTrack(activeTrack - 1, false);
        else
          setActiveTrack(activeTrack - 1, true);
        lastTime = millis();
      }
    }
    if (itsbeen(50)) {
      if (x == 1 && shift) {
        moveCursor(-1);
        lastTime = millis();
      }
      if (x == -1 && shift) {
        moveCursor(1);
        lastTime = millis();
      }
    }
    graphics.rotate(1,0);
    graphics.rotate(1,1);
    display.clearDisplay();
    drawSeq(true, false, true, false, false, false, viewStart, viewEnd);
    drawCoordinateBox(coords);
    graphics.renderDie();
    if(coords.x1 == 0 && coords.x2 == 0 && coords.y1 == 0 && coords.y2 == 0){
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
    if(coords.x1 == coords.x2 && coords.y1 == coords.y2){
      break;
    }
    if(coords.x1>coords.x2){
      uint16_t temp = coords.x1;
      coords.x1 = coords.x2;
      coords.x2 = temp;
    }
    if(coords.y1>coords.y2){
      uint8_t temp = coords.y1;
      coords.y1 = coords.y2;
      coords.y2 = temp;
    }
    //if you're targeting empty spaces
    if(!randData.target){
      //iterate over the tracks (inclusively)
      for(uint8_t t = coords.y1; t<=coords.y2; t++){
        for(uint16_t step = coords.x1; step<coords.x2; step++){
          //if it's not only making notes on the grid, or if the step is on a subDiv
          if((!randData.onlyOnGrid || !(step%subDivInt)) && lookupData[t][step] == 0){
            if(random(0,100)<randData.odds){
              uint8_t chance = random(randData.minChance,randData.maxChance+1);
              uint16_t length = random(randData.minLength,randData.maxLength+1);
              uint8_t vel = random(randData.minVel,randData.maxVel+1);
              //making sure notes won't run off the end
              if(step+length>coords.x2){
                length = coords.x2-step;
              }
              Note newNote = Note(step,step+length,vel,chance,false,false);
              makeNote(newNote, t);
              step = newNote.endPos-1;
              //if it's not on grid, move a random amount forward
            }
            if(!randData.onlyOnGrid){
              step+=random(0,randData.maxLength*(100-randData.odds)/100);
            }
          }
        }
      }
    }
    //if you're targeting existing notes
    else{
      //iterate over the tracks (inclusively)
      for(uint8_t t = coords.y1; t<=coords.y2; t++){
        for(uint16_t step = coords.x1; step<coords.x2; step++){
          //ignore grid for this
          if(lookupData[t][step] != 0){
            if(random(0,100)<randData.odds){
              uint8_t chance = random(randData.minChance,randData.maxChance+1);
              uint16_t length = random(randData.minLength,randData.maxLength+1);
              uint8_t vel = random(randData.minVel,randData.maxVel+1);
              //making sure notes won't run off the end
              if(step+length>coords.x2){
                length = coords.x2-step;
              }
              deleteNote_byID(t,lookupData[t][step]);
              Note newNote = Note(step,step+length,vel,chance,false,false);
              makeNote(newNote, t);
              step = newNote.endPos-1;
            }
          }
        }
      }
    }
  }
}


//six parameters:
//odds - chance a note will be placed

//spacing - the min/max distance between notes XX
//min/max chance - probability property of the placed notes

//length - the min/max length of a note
//vel - the min/max velocity

//looping - if the randomSeq regens after each loop XX
//grid - on/off grid, grid size

//tracks - which tracks get randomized (all by default)
void drawRandMenu(uint8_t whichTab){
    const int8_t y1 = 4;
    const int8_t y2 = 28;
    const int8_t y3 = 52;
    //length
    if(whichTab == 0){
      display.fillRoundRect(-3,y1,30,9,3,1);
      graphics.drawArrow(18,y1+12+((millis()/200)%2),2,2,false);
    }
    else{
      display.fillRoundRect(-3,y1,30,9,3,0);
      display.drawRoundRect(-3,y1,30,9,3,1);
    }
    printSmall(1,y1+2,"length",2);
    
    //odds
    if(whichTab == 1){
      display.fillRoundRect(-3,y2,22,9,3,1);
      graphics.drawArrow(21+((millis()/200)%2),y2+4,2,1,false);
    }
    else{
      display.fillRoundRect(-3,y2,22,9,3,0);
      display.drawRoundRect(-3,y2,22,9,3,1);
    }
    printSmall(1,y2+2,"odds",2);
    
    //chance
    if(whichTab == 2){
      display.fillRoundRect(-3,y3,30,9,3,1);
      graphics.drawArrow(18,y3-3+((millis()/200)%2),2,3,false);
    }
    else{
      display.fillRoundRect(-3,y3,30,9,3,0);
      display.drawRoundRect(-3,y3,30,9,3,1);
    }
    printSmall(1,y3+2,"chance",2);
    
    //target
    if(whichTab == 3){
      display.fillRoundRect(101,y1,32,9,3,1);
      graphics.drawArrow(109,y1+12+((millis()/200)%2),2,2,false);
    }
    else{
      display.fillRoundRect(101,y1,32,9,3,0);
      display.drawRoundRect(101,y1,32,9,3,1);
    }
    printSmall(104,y1+2,"target",2);
    
    //vel
    if(whichTab == 4){
      display.fillRoundRect(109,y2,21,9,3,1);
      graphics.drawArrow(107-((millis()/200)%2),y2+4,2,0,false);
    }
    else{
      display.fillRoundRect(109,y2,21,9,3,0);
      display.drawRoundRect(109,y2,21,9,3,1);
    }
    printSmall(112,y2+2,"velo",2);
    
    //target
    if(whichTab == 5){
      display.fillRoundRect(101,y3,32,9,3,1);
      graphics.drawArrow(109,y3-3+((millis()/200)%2),2,3,false);
    }
    else{
      display.fillRoundRect(101,y3,32,9,3,0);
      display.drawRoundRect(101,y3,32,9,3,1);
    }
    printSmall(104,y3+2,"timing",2);
}

bool randMenuControls(uint8_t * whichTab){
    readJoystick();
    readButtons();
    if(itsbeen(200)){
        if(x != 0){
            if(x == -1 && (*whichTab) < 3){
                (*whichTab) += 3;
                lastTime = millis();
            }
            else if(x == 1 && (*whichTab)>2){
                (*whichTab) -= 3;
                lastTime = millis();
            }
        }
        if(y != 0){
            if(y == 1 && (*whichTab) != 5 && (*whichTab) != 2){
                (*whichTab)++;
                lastTime = millis();
            }
            else if(y == -1 && (*whichTab) != 0 && (*whichTab) != 3){
                (*whichTab)--;
                lastTime = millis();
            }
        }
        if(menu_Press){
            lastTime = millis();
            return false;
        }
      if(n){
        lastTime = millis();
        genRandom(randomData);
      }
    }
    
    while(counterA != 0){
      switch(*whichTab){
        //length
        case 0:
          if(counterA >= 1){
            if(shift)
              (randomData.minLength)++;
            else{
              randomData.minLength = changeSubDiv(true,randomData.minLength,false);
            }
            if(randomData.minLength>96)
              randomData.minLength = 96;
          }
          else if(counterA <= -1){
            if(shift && randomData.minLength>1)
              (randomData.minLength)--;
            else
              randomData.minLength = changeSubDiv(false,randomData.minLength,false);
          }
          if(randomData.minLength>randomData.maxLength)
            randomData.maxLength=randomData.minLength;
          counterA += counterA<0?1:-1;
          break;
        //odds
        case 1:
          if(counterA >= 1){
            if(shift)
              (randomData.odds)++;
            else
              (randomData.odds)+=10;
            if(randomData.odds>100)
              randomData.odds = 100;
          }
          else if(counterA <= -1){
            if(shift)
              (randomData.odds)--;
            else
              (randomData.odds)-=10;
            if(randomData.odds<0)
              (randomData.odds) = 0;
          }
          counterA += counterA<0?1:-1;
          break;
      //chance
        case 2:
          if(counterA >= 1){
            if(shift)
              (randomData.minChance)++;
            else{
              if(randomData.minChance == 1)
                randomData.minChance = 10;
              else
                (randomData.minChance) += 10;
            }
            if(randomData.minChance>100)
              randomData.minChance = 100;
          }
          else if(counterA <= -1){
            if(shift && randomData.minChance>1)
              (randomData.minChance)--;
            else if(randomData.minChance>16)
              (randomData.minChance) -= 16;
            else
              (randomData.minChance) = 1;
            
            if(randomData.minChance>randomData.maxChance)
              randomData.maxChance = randomData.minChance;
          }
          counterA += counterA<0?1:-1;
          break;
        //randomize selection/everything
        case 3:
          randomData.target = !randomData.target;
          counterA += counterA<0?1:-1;
          break;
        //vel
        case 4:
          if(counterA >= 1){
            if(shift)
              (randomData.minVel)++;
            else{
              if(randomData.minVel == 1)
                randomData.minVel = 16;
              else
                (randomData.minVel) += 16;
            }
            if(randomData.minVel>127)
              randomData.minVel = 127;
          }
          else if(counterA <= -1){
            if(shift && randomData.minVel>1)
              (randomData.minVel)--;
            else if(randomData.minVel>16)
             (randomData.minVel) -= 16;
            else
              (randomData.minVel) = 1;
            
            if(randomData.minVel>randomData.maxVel)
              randomData.maxVel = randomData.minVel;
          }
          counterA += counterA<0?1:-1;
          break;
        //grid on/off
        case 5:
          randomData.onlyOnGrid = !randomData.onlyOnGrid;
          counterA += counterA<0?1:-1;
          break;
      }
    }

    while(counterB != 0){
      switch(*whichTab){
        //length
        case 0:
        {
          if(counterB >= 1){
            if(shift)
              (randomData.maxLength)++;
            else{
              randomData.maxLength = changeSubDiv(true,randomData.maxLength,false);
            }
            if(randomData.maxLength>96)
              randomData.maxLength = 96;
          }
          else if(counterB <= -1){
            if(shift && randomData.maxLength>1)
              (randomData.maxLength)--;
            else
              randomData.maxLength = changeSubDiv(false,randomData.maxLength,false);
          }
          if((randomData.maxLength)<(randomData.minLength))
            randomData.minLength = randomData.maxLength;
          counterB += counterB<0?1:-1;
        }
        break;
        //odds
        case 1:
          if(counterB >= 1){
            if(shift)
              (randomData.odds)++;
            else
              (randomData.odds)+=10;
            if(randomData.odds>100)
              randomData.odds = 100;
          }
          else if(counterB <= -1){
            if(shift)
              (randomData.odds)--;
            else
              (randomData.odds)-=10;
            if(randomData.odds<0)
              (randomData.odds) = 0;
          }
          counterB += counterB<0?1:-1;
          break;
        //chance
        case 2:
          if(counterB >= 1){
            if(shift)
              (randomData.maxChance)++;
            else{
              if(randomData.maxChance == 1)
                randomData.maxChance = 10;
              else{
                (randomData.maxChance) += 10;
              }
            }
            if(randomData.maxChance>100)
              randomData.maxChance = 100;
          }
          else if(counterB <= -1){
            if(shift && randomData.maxChance>1)
              (randomData.maxChance)--;
            else if(randomData.maxChance>10)
              (randomData.maxChance) -= 10;
            else
              randomData.maxChance = 1;
            if(randomData.maxChance<randomData.minChance)
              randomData.minChance = randomData.maxChance;
          }
          counterB += counterB<0?1:-1;
          break;
        //randomize selection/everything
        case 3:
          randomData.target = !randomData.target;
          counterB += counterB<0?1:-1;
          break;
        //velocity
        case 4:
          if(counterB >= 1){
            if(shift)
              randomData.maxVel++;
            else{
              if(randomData.maxVel == 1)
                randomData.maxVel = 16;
              else
                (randomData.maxVel) += 16;
            }
            if(randomData.maxVel>127)
              randomData.maxVel = 127;
          }
          else if(counterB <= -1){
            if(shift && randomData.maxVel>1)
             (randomData.maxVel)--;
            else if(randomData.maxVel>16)
              (randomData.maxVel) -= 16;
            else
              randomData.maxVel = 1;
            
            if(randomData.maxVel<randomData.minVel)
              randomData.minVel = randomData.maxVel;
          }
          counterB += counterB<0?1:-1;
          break;
        //changing grid subDiv
        case 5:
          if(counterB >= 1){
            changeSubDivInt(true);
          }
          else if(counterB <= -1){
            changeSubDivInt(false);
          }
          counterB += counterB<0?1:-1;
          break;
      }
    }
    return true;
}

void randMenu(){
    //which menu you're on
    uint8_t whichTab = 1;
    //vars for randomizer are stored in randomData struct
    WireFrame cube = makeCube(70);
    cube.xPos = 64;
    cube.yPos = 32;
    while(true){
        if(!randMenuControls(&whichTab)){
            break;
        }
        //rotating cube
        cube.rotate(1,0);
        cube.rotate(1,1);
        
        display.clearDisplay();
        cube.renderDie();
        printItalic(29,0,"randomize",1);//printing title
        drawRandMenu(whichTab);//printing the side tabs (and bouncing arrows)
        //printing the submenus
        drawRandMenuOptions(whichTab,randomData);
        display.display();
    }
}
