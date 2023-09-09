//Random Menu
void drawRandMenuOptions(uint8_t which,int8_t odds,int8_t minChance, int8_t maxChance, uint16_t minLength, uint16_t maxLength, uint8_t minVel, uint8_t maxVel, uint8_t gridBehavior, bool targetTrack){
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
    uint8_t diff = (maxChance-minChance)/8;
    for(uint8_t i = (minChance/8)+1; i<(maxChance/8-diff/2*sin(float(millis())/float(100)))-2; i+=2){
      display.drawCircle(x1+28,y1+20,i,1);
    }
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
    printSmall(x1+10,y1,"randomize...",1);
    printArp_wiggly(x1,y1+16,targetTrack?"Selected":"Everything",1);
    }
    break;
    //velocity
    case 4:
      {
      const uint8_t x1 = 36;
      const uint8_t y1 = 14;
      display.drawCircle(x1+28,y1+20,maxVel/8,1);
      display.fillCircle(x1+28,y1+20,minVel/8,1);
      uint8_t diff = (maxVel-minVel)/8;
      for(uint8_t i = (minVel/8)+1; i<(maxVel/8-diff/2*sin(float(millis())/float(100)))-2; i+=2){
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
      printArp_wiggly(x1,y1+10,gridBehavior?"On Grid":"Off Grid",1);
      printSmall(x1,y1+25,"grid size:",1);
      printFraction_small(x1,y1+35,stepsToMeasures(subDivInt));
      }
      break;
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
    if(whichTab == 0)
        display.fillRoundRect(-3,y1,30,9,3,1);
    else
        display.drawRoundRect(-3,y1,30,9,3,1);
    printSmall(1,y1+2,"length",2);
    
    //odds
    if(whichTab == 1)
        display.fillRoundRect(-3,y2,22,9,3,1);
    else
        display.drawRoundRect(-3,y2,22,9,3,1);
    printSmall(1,y2+2,"odds",2);
    
    //odds
    if(whichTab == 2)
        display.fillRoundRect(-3,y3,30,9,3,1);
    else
        display.drawRoundRect(-3,y3,30,9,3,1);
    printSmall(1,y3+2,"chance",2);
    
    //target
    if(whichTab == 3)
        display.fillRoundRect(101,y1,32,9,3,1);
    else
        display.drawRoundRect(101,y1,32,9,3,1);
    printSmall(104,y1+2,"target",2);
    
    //vel
    if(whichTab == 4)
        display.fillRoundRect(109,y2,21,9,3,1);
    else
        display.drawRoundRect(109,y2,21,9,3,1);
    printSmall(112,y2+2,"velo",2);
    
    //target
    if(whichTab == 5)
        display.fillRoundRect(101,y3,32,9,3,1);
    else
        display.drawRoundRect(101,y3,32,9,3,1);
    printSmall(104,y3+2,"timing",2);
}

bool randMenuControls(uint8_t * whichTab, int8_t * odds, int8_t * minChance, int8_t * maxChance, uint16_t * minLength, uint16_t * maxLength, uint8_t * minVel, uint8_t * maxVel, bool * gridBehavior, bool * targetTrack){
    joyRead();
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
    }
    
    
    while(counterA != 0){
      switch(*whichTab){
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
              *odds = 0;
          }
          counterA += counterA<0?1:-1;;
          break;
        //length
        case 2:
          if(counterA >= 1){
            if(shift)
              (*maxLength)++;
            else{
              if(*maxLength == 1)
                *maxLength = 12;
              else
                (*maxLength)+=12;
            }
            if(*maxLength>96)
              *maxLength = 96;
          }
          else if(counterA <= -1){
            if(shift && *maxLength>1)
              (*maxLength)--;
            else if(*maxLength>12)
              (*maxLength)-=12;
            else
              (*maxLength) = 1;
          }
          if(*maxLength<*minLength)
            minLength = maxLength;
          counterA += counterA<0?1:-1;;
          break;
        //velocity
        case 3:
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
          counterA += counterA<0?1:-1;;
          break;
        //chance
        case 4:
          if(counterA >= 1){
            if(shift)
              (*maxChance)++;
            else{
              if(*maxChance == 1)
                *maxChance = 10;
              else{
                (*maxChance) += 10;
                Serial.println(*maxChance);
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
          counterA += counterA<0?1:-1;;
          break;
        //randomize selection/everything
        case 5:
          *targetTrack = !*targetTrack;
          counterA += counterA<0?1:-1;;
          break;
        //grid on/off
        case 6:
          *gridBehavior = !*gridBehavior;
          counterA += counterA<0?1:-1;;
          break;
      }
    }

    while(counterB != 0){
      switch(*whichTab){
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
          counterB += counterB<0?1:-1;;
          break;
        //length
        case 2:
          if(counterB >= 1){
            if(shift)
              (*minLength)++;
            else{
              if(*minLength == 1)
                *minLength = 12;
              else
                (*minLength)+=12;
            }
            if(*minLength>96)
              *minLength = 96;
          }
          else if(counterB <= -1){
            if(shift && *minLength>1)
              (*minLength)--;
            else if(*minLength>12)
              (*minLength)-=12;
            else
              *minLength = 1;
          }
          if(*minLength>*maxLength)
            *maxLength=*minLength;
          counterB += counterB<0?1:-1;;
          break;
        //velocity
        case 3:
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
          counterB += counterB<0?1:-1;;
          break;
        //chance
        case 4:
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
          counterB += counterB<0?1:-1;;
          break;
        //randomize selection/everything
        case 5:
          *targetTrack = !*targetTrack;
          counterB += counterB<0?1:-1;;
          break;
        //changing grid subDiv
        case 6:
          if(counterB >= 1){
            changeSubDivInt(true);
          }
          else if(counterB <= -1){
            changeSubDivInt(false);
          }
          counterB += counterB<0?1:-1;;
          break;
      }
    }
    return true;
}


void randMenu(){
    //which menu you're on
    uint8_t whichTab = 1;
    //vars for randomizer
    int8_t odds = 100;
    int8_t minChance = 100;
    int8_t maxChance = 100;
    uint16_t minLength = 24;
    uint16_t maxLength = 48;
    uint8_t minVel = 64;
    uint8_t maxVel = 127;
    bool gridBehavior = true;//true is on the grid, false is off grid
    bool targetTrack = 0;//0 is all, 1 is select
    
    float angleX,angleY,angleZ;
    while(true){
        if(!randMenuControls(&whichTab,&odds,&minChance,&maxChance,&minLength,&maxLength,&minVel,&maxVel,&gridBehavior,&targetTrack)){
            break;
        }
        
        //wiggling all the objects before rendering them
        angleX = sin(millis()/200)-20;
        angleY = sin(millis()/200)+20;
        angleZ = 2*sin(millis()/200)-20;
        
        display.clearDisplay();
        
        printItalic(29,0,"randomize",1);
        drawRandMenu(whichTab);
        drawRandMenuOptions(whichTab,odds,minChance,maxChance,minLength,maxLength,minVel,maxVel,gridBehavior,targetTrack);
        display.display();
    }
}
