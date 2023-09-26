//Random Menu
void drawRandMenuOptions(uint8_t which,int8_t odds,int8_t minChance, int8_t maxChance, uint16_t minLength, uint16_t maxLength, uint8_t minVel, uint8_t maxVel, uint8_t gridBehavior, bool target){
  switch(which){
  //length
  case 0:
    {
    const uint8_t x1 = 46;
    const uint8_t y1 = 10;
    printSmall(x1-8,y1,"length of notes",1);
    printSmall(x1-2,y1+24,"max",1);
    String frac = stepsToMeasures(maxLength);
    printFraction_small(x1-(frac.length()>=4?5:0),y1+15,frac);
    printSmall(x1+29,y1+14,"min",1);
    frac = stepsToMeasures(minLength);
    printFraction_small(x1+29,y1+23,frac);
    int8_t offset1 = 4*sin(millis()/100);
    int8_t offset2 = 4*cos(millis()/100);
    display.fillRect(x1+12,y1+24-maxLength/6-offset1/2,6,maxLength/3+offset1,1);
    display.fillRect(x1+20,y1+24-minLength/6-offset2/2,6,minLength/3+offset2,1);
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
      String val = stringify(odds);
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
    display.drawCircle(x1+28,y1+20,maxChance/6,1);
    display.fillCircle(x1+28,y1+20,minChance/6,1);
    uint8_t diff = ceil(float(maxChance)/8.0-float(minChance)/8.0);
    if(!diff){
        diff = 1;
    }   
    // for(uint8_t i = (minChance/8)+1; i<(maxChance/8-diff/2*sin(float(millis())/float(100)))-2; i+=2){
    display.drawCircle(x1+28,y1+20,minChance/8+(millis()/50)%diff+1,1);
    printSmall(x1,y1+sin(millis()/200),"max",1);
    printSmall(x1,y1+6+sin(millis()/200),stringify(maxChance),1);
    printSmall(x1+45,y1+34+cos(millis()/200),"min",1);
    printSmall(x1+45,y1+40+cos(millis()/200),stringify(minChance),1);
    printSmall(x1,y1+40,"chance",1);
    }
    break;
  //target
  case 3:
    {
    const uint8_t x1 = 36;
    const uint8_t y1 = 14;
    printSmall(x1+8,y1,"randomize...",1);
      if(target){
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
      display.drawCircle(x1+28,y1+20,maxVel/8,1);
      display.fillCircle(x1+28,y1+20,minVel/8,1);
      uint8_t diff = maxVel/8-minVel/8;
      for(uint8_t i = (minVel/8)+1; i<(maxVel/8-diff*abs(sin(float(millis())/float(200))))-1; i+=2){
        display.drawCircle(x1+28,y1+20,i,1);
      }
      printSmall(x1,y1+sin(millis()/200),"max",1);
      printSmall(x1,y1+6+sin(millis()/200),stringify(maxVel),1);
      printSmall(x1+45,y1+34+cos(millis()/200),"min",1);
      printSmall(x1+45,y1+40+cos(millis()/200),stringify(minVel),1);
      printSmall(x1,y1+40,"velocity",1);
      }
      break;
    //grid
    case 5:
      {
      const uint8_t x1 = 44;
      const uint8_t y1 = 10;
      if(gridBehavior){
        printArp_wiggly(x1,y1+10,"On Grid",1);
        printSmall_centered(64,y1+25,"locked to grid:",1);
        printFraction_small_centered(62,y1+35,stepsToMeasures(subDivInt));
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
    // uint8_t startHeight = maxTracksShown==5?debugHeight:8;
    uint8_t startHeight = debugHeight;
    uint8_t startY = (Y1-startTrack)*trackHeight+startHeight;
    uint8_t height = ((Y2 - startTrack + 1)*trackHeight - startY)%(screenHeight-startHeight) + startHeight;
   
   if((millis())%400>200){
      shadeRect(startX,startY,length,height,3);
    }
    else{
      display.drawRect(startX,startY,length,height,1);
    }
  }
}

CoordinatePair selectArea(){
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

void genRandom(int8_t odds, int8_t minChance, int8_t maxChance, uint16_t minLength, uint16_t maxLength, uint8_t minVel, uint8_t maxVel,bool onGrid,bool target){
  while(true){
    CoordinatePair coords = selectArea();
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
    if(!target){
      //iterate over the tracks (inclusively)
      for(uint8_t t = coords.y1; t<=coords.y2; t++){
        for(uint16_t step = coords.x1; step<coords.x2; step++){
          //if it's not only making notes on the grid, or if the step is on a subDiv
          if((!onGrid || !(step%subDivInt)) && lookupData[t][step] == 0){
            if(random(0,100)<odds){
              uint8_t chance = random(minChance,maxChance+1);
              uint16_t length = random(minLength,maxLength+1);
              uint8_t vel = random(minVel,maxVel+1);
              //making sure notes won't run off the end
              if(step+length>coords.x2){
                length = coords.x2-step;
              }
              Note newNote = Note(step,step+length,vel,chance,false,false);
              makeNote(newNote, t);
              step = newNote.endPos-1;
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
            if(random(0,100)<odds){
              uint8_t chance = random(minChance,maxChance+1);
              uint16_t length = random(minLength,maxLength+1);
              uint8_t vel = random(minVel,maxVel+1);
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
      drawArrow(18,y1+12+sin(millis()/100),2,2,false);
    }
    else{
      display.fillRoundRect(-3,y1,30,9,3,0);
      display.drawRoundRect(-3,y1,30,9,3,1);
    }
    printSmall(1,y1+2,"length",2);
    
    //odds
    if(whichTab == 1){
      display.fillRoundRect(-3,y2,22,9,3,1);
      drawArrow(21+sin(millis()/100),y2+4,2,1,false);
    }
    else{
      display.fillRoundRect(-3,y2,22,9,3,0);
      display.drawRoundRect(-3,y2,22,9,3,1);
    }
    printSmall(1,y2+2,"odds",2);
    
    //chance
    if(whichTab == 2){
      display.fillRoundRect(-3,y3,30,9,3,1);
      drawArrow(18,y3-3+sin(millis()/100),2,3,false);
    }
    else{
      display.fillRoundRect(-3,y3,30,9,3,0);
      display.drawRoundRect(-3,y3,30,9,3,1);
    }
    printSmall(1,y3+2,"chance",2);
    
    //target
    if(whichTab == 3){
      display.fillRoundRect(101,y1,32,9,3,1);
      drawArrow(109,y1+12+sin(millis()/100),2,2,false);
    }
    else{
      display.fillRoundRect(101,y1,32,9,3,0);
      display.drawRoundRect(101,y1,32,9,3,1);
    }
    printSmall(104,y1+2,"target",2);
    
    //vel
    if(whichTab == 4){
      display.fillRoundRect(109,y2,21,9,3,1);
      drawArrow(107-sin(millis()/100),y2+4,2,0,false);
    }
    else{
      display.fillRoundRect(109,y2,21,9,3,0);
      display.drawRoundRect(109,y2,21,9,3,1);
    }
    printSmall(112,y2+2,"velo",2);
    
    //target
    if(whichTab == 5){
      display.fillRoundRect(101,y3,32,9,3,1);
      drawArrow(109,y3-3+sin(millis()/100),2,3,false);
    }
    else{
      display.fillRoundRect(101,y3,32,9,3,0);
      display.drawRoundRect(101,y3,32,9,3,1);
    }
    printSmall(104,y3+2,"timing",2);
}

bool randMenuControls(uint8_t * whichTab, int8_t * odds, int8_t * minChance, int8_t * maxChance, uint16_t * minLength, uint16_t * maxLength, uint8_t * minVel, uint8_t * maxVel, bool * gridBehavior, bool * target){
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
        genRandom(*odds, *minChance, * maxChance, *minLength, *maxLength, *minVel, *maxVel,*gridBehavior,*target);
      }
    }
    
    while(counterA != 0){
      switch(*whichTab){
        //length
        case 0:
        {
          if(counterA >= 1){
            if(shift)
              (*maxLength)++;
            else{
              *maxLength = changeSubDiv(true,*maxLength,false);
            }
            if(*maxLength>96)
              *maxLength = 96;
          }
          else if(counterA <= -1){
            if(shift && *maxLength>1)
              (*maxLength)--;
            else
              *maxLength = changeSubDiv(false,*maxLength,false);
          }
          if((*maxLength)<(*minLength))
            *minLength = *maxLength;
          counterA += counterA<0?1:-1;
        }
          break;
        //odds
        case 1:
          if(counterA >= 1){
            if(shift)
              (*odds)++;
            else
              (*odds)+=10;
            if(*odds>100)
              *odds = 100;
          }
          else if(counterA <= -1){
            if(shift)
              (*odds)--;
            else
              (*odds)-=10;
            if(*odds<0)
              (*odds) = 0;
          }
          counterA += counterA<0?1:-1;
          break;
        //chance
        case 2:
          if(counterA >= 1){
            if(shift)
              (*maxChance)++;
            else{
              if(*maxChance == 1)
                *maxChance = 10;
              else{
                (*maxChance) += 10;
              }
            }
            if(*maxChance>100)
              *maxChance = 100;
          }
          else if(counterA <= -1){
            if(shift && *maxChance>1)
              (*maxChance)--;
            else if(*maxChance>10)
              (*maxChance) -= 10;
            else
              *maxChance = 1;
            if(*maxChance<*minChance)
              *minChance = *maxChance;
          }
          counterA += counterA<0?1:-1;
          break;
        //randomize selection/everything
        case 3:
          *target = !*target;
          counterA += counterA<0?1:-1;
          break;
        //velocity
        case 4:
          if(counterA >= 1){
            if(shift)
              maxVel++;
            else{
              if(*maxVel == 1)
                *maxVel = 16;
              else
                (*maxVel) += 16;
            }
            if(*maxVel>127)
              *maxVel = 127;
          }
          else if(counterA <= -1){
            if(shift && *maxVel>1)
             (*maxVel)--;
            else if(*maxVel>16)
              (*maxVel) -= 16;
            else
              *maxVel = 1;
            
            if(*maxVel<*minVel)
              *minVel = *maxVel;
          }
          counterA += counterA<0?1:-1;
          break;
        //grid on/off
        case 5:
          *gridBehavior = !*gridBehavior;
          counterA += counterA<0?1:-1;
          break;
      }
    }

    while(counterB != 0){
      switch(*whichTab){
        //length
        case 0:
          if(counterB >= 1){
            if(shift)
              (*minLength)++;
            else{
              *minLength = changeSubDiv(true,*minLength,false);
            }
            if(*minLength>96)
              *minLength = 96;
          }
          else if(counterB <= -1){
            if(shift && *minLength>1)
              (*minLength)--;
            else
              *minLength = changeSubDiv(false,*minLength,false);
          }
          if(*minLength>*maxLength)
            *maxLength=*minLength;
          counterB += counterB<0?1:-1;
          break;
        //odds
        case 1:
          if(counterB >= 1){
            if(shift)
              (*odds)++;
            else
              (*odds)+=10;
            if(*odds>100)
              *odds = 100;
          }
          else if(counterB <= -1){
            if(shift)
              (*odds)--;
            else
              (*odds)-=10;
            if(*odds<0)
              (*odds) = 0;
          }
          counterB += counterB<0?1:-1;
          break;
        //chance
        case 2:
          if(counterB >= 1){
            if(shift)
              (*minChance)++;
            else{
              if(*minChance == 1)
                *minChance = 10;
              else
                (*minChance) += 10;
            }
            if(*minChance>100)
              *minChance = 100;
          }
          else if(counterB <= -1){
            if(shift && *minChance>1)
              (*minChance)--;
            else if(*minChance>16)
              (*minChance) -= 16;
            else
              (*minChance) = 1;
            
            if(*minChance>*maxChance)
              *maxChance = *minChance;
          }
          counterB += counterB<0?1:-1;
          break;
        //randomize selection/everything
        case 3:
          *target = !*target;
          counterB += counterB<0?1:-1;
          break;
                //velocity
        case 4:
          if(counterB >= 1){
            if(shift)
              (*minVel)++;
            else{
              if(*minVel == 1)
                *minVel = 16;
              else
                (*minVel) += 16;
            }
            if(*minVel>127)
              *minVel = 127;
          }
          else if(counterB <= -1){
            if(shift && *minVel>1)
              (*minVel)--;
            else if(*minVel>16)
             (*minVel) -= 16;
            else
              (*minVel) = 1;
            
            if(*minVel>*maxVel)
              *maxVel = *minVel;
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
    //vars for randomizer
    int8_t odds = 60;
    int8_t minChance = 100;
    int8_t maxChance = 100;
    uint16_t minLength = 24;
    uint16_t maxLength = 48;
    uint8_t minVel = 64;
    uint8_t maxVel = 127;
    bool gridBehavior = true;//true is on the grid, false is off grid
    bool target = 0;//0 is all, 1 is select

    WireFrame cube = makeCube(70);
    cube.xPos = 64;
    cube.yPos = 32;
    while(true){
        if(!randMenuControls(&whichTab,&odds,&minChance,&maxChance,&minLength,&maxLength,&minVel,&maxVel,&gridBehavior,&target)){
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
        drawRandMenuOptions(whichTab,odds,minChance,maxChance,minLength,maxLength,minVel,maxVel,gridBehavior,target);
        display.display();
    }
}
