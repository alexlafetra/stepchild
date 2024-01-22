
String getCCParameterName(uint8_t param){
  if(param<16)
    return CCparameters[param];
  else if(param >= 16 && param <= 19)
    return CCparameters[16];
  else if(param >= 20 && param <= 31)
    return CCparameters[17];
  else if(param >= 32 && param <= 63)
    return CCparameters[18]+stringify(param-32);
  else if(param >= 64 && param <= 79)
    return CCparameters[param-45];
  else if(param >= 80 && param <= 83)
    return CCparameters[35];
  else if(param == 84)
    return CCparameters[36];
  else if(param >= 85 && param <= 87)
    return CCparameters[37];
  else if(param == 88)
    return CCparameters[38];
  else if(param == 89 || param == 90)
    return CCparameters[39];
  else if(param >= 91 && param <= 101)
    return CCparameters[param - 51];
  else if(param >= 102 && param <= 119)
    return CCparameters[51];
  else  
    return "";
}


String getBUSFromChannel(uint8_t channel){
  if(channel >= 1 && channel <= 4)
    return "BUS "+stringify(channel);
  else if(channel == 5)
    return "INPUT";
  else
    return "[No BUS]";
}

String getMKIIParameterName(uint8_t param,  uint8_t channel){
  String bus = getBUSFromChannel(channel);
  if(param >= 16 && param <= 18)
    return bus+" "+MKIICCparameters[0]+stringify(param-15);
  else if (param == 19)
    return bus+" "+MKIICCparameters[1];
  else if(param >= 80 && param <= 82)
    return bus+" "+MKIICCparameters[0]+stringify(param-76);
  else if(param == 83)
    return bus+" "+MKIICCparameters[2];
  else
    return "";
}

String getStepchildCCParameterName(uint8_t param, uint8_t channel){
  String name;
  if(param<5){
    if(param>2){
      name+= "Global "+stepChildCCParameters[param];
    }
    else{
      if(channel == 0){
        name += "Global ";
      }
      else
        name+= "Ch"+stringify(channel)+" ";
      name+=stepChildCCParameters[param];
    }
  }
  return name;
}

//turns a numbered parameter like "third in the MKII list" to a MIDI CC number like "83"
uint8_t MKIIParamToCC(uint8_t param){
  if(param >= 0 && param <= 3)
    return param+16;
  else if(param >= 4 && param <= 7)
    return param+76;
  else
    return 100;
}

uint8_t stepchildParamToCC(uint8_t p){
  if(p>4)
    p = 4;
  return p;
}

uint8_t moveToNextCCParam(uint8_t param, bool up, uint8_t whichList){
  switch(whichList){
    //default
    case 0:
      if(param>0 && !up)
        param--;
      else if(param<127 && up)
        param++;
      break;
    //mkII
    case 1:
      if(!up){
        //if it's within param bounds
        if((param > 16 && param <= 19) || (param > 80 && param <= 83))
          param--;
        //if it aint
        else if(param > 83)
          param = 83;
        else if(param > 19 && param <= 80)
          param = 19;
      }
      else{
        if((param >= 16 && param < 19) || (param >= 80 && param < 83))
          param++;
        else if(param < 16)
          param = 16;
        else if(param >= 19 && param < 80)
          param = 80;
      }
      break;
    //stepChild
    case 2:
      if(up){
        if(param<4)
          param++;
        else
          param = 4;
      }
      else{
        if(param>0)
          param--;
        else
          param = 0;
      }
      break;
  }
  return param;
}



String getCCParam(uint8_t param, uint8_t channel, uint8_t type){
  switch(type){
    case 0:
      return getCCParameterName(param);
    case 1:
      return getMKIIParameterName(param,channel);
    case 2:
      return getStepchildCCParameterName(param,channel);
  }
  return "";
}


void printParam_centered(uint8_t which, uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC){
  String p;
  switch(type){
    case 0:
      p = getCCParameterName(param);
      break;
    case 1:
      p = getMKIIParameterName(param,autotrackData[which].channel);
      break;
    case 2:
      p = getStepchildCCParameterName(param,autotrackData[which].channel);
      break;
  }
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*(p.length()+stringify(param).length())+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*(p.length()+stringify(param).length())+3,9,SSD1306_WHITE);
  }
  if(withCC)
    printSmall(xPos-p.length()*2-6,yPos,"CC"+stringify(param)+":"+p,SSD1306_WHITE);
  else
    printSmall(xPos-p.length()*2,yPos,p,SSD1306_WHITE);
}

void printParam(uint8_t xPos, uint8_t yPos, uint8_t param, bool withBox, uint8_t type, bool withCC){
  String p;
  if(withCC)
    p = "CC"+stringify(param)+":";
  switch(type){
    case 0:
      p += getCCParameterName(param);
      break;
    case 1:
      p += getMKIIParameterName(param,autotrackData[activeAutotrack].channel);
      break;
    case 2:
      p += getStepchildCCParameterName(param,autotrackData[activeAutotrack].channel);
      break;
  }
  if(withBox){
    display.fillRect(xPos-2,yPos-2,4*p.length()+3,9,SSD1306_BLACK);
    display.drawRect(xPos-2,yPos-2,4*p.length()+3,9,SSD1306_WHITE);
  }
  printSmall(xPos,yPos,p,SSD1306_WHITE);
}


void printParamList(uint8_t which, uint8_t target, uint8_t start, uint8_t end, uint8_t channel){
  switch(which){
    //default
    case 0:
      for(uint8_t p = start; p<end; p++){
        // display.setCursor(0,(p-start)*8);
        // display.print(getCCParameterName(p));
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        if(p<10)
          printSmall(3,27+(p-start)*6,stringify(p),2);
        else
          printSmall(2,27+(p-start)*6,stringify(p),2);
        printSmall(20+xPos,27+(p-start)*6,getCCParameterName(p),2);
      }
      break;
    //mkII
    case 1:
      for(uint8_t p = start; p<end; p++){
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        if(MKIIParamToCC(p)<10)
          printSmall(3,27+(p-start)*6,stringify(MKIIParamToCC(p)),2);
        else
          printSmall(2,27+(p-start)*6,stringify(MKIIParamToCC(p)),2);
        //convert number to CC, then to the parameter name
        printSmall(20+xPos,27+(p-start)*6,getMKIIParameterName(MKIIParamToCC(p),channel),2);
      }
      break;
    //Internal
    case 2:
      for(uint8_t p = start; p<end; p++){
        int8_t xPos = 0;
        if(p == target){
          display.fillRoundRect(0,26+(p-start)*6,screenWidth,7,3,SSD1306_WHITE);
        }
        printSmall(2,27+(p-start)*6,stringify(p),2);
        //convert number to CC, then to the parameter name
        printSmall(20+xPos,27+(p-start)*6,getStepchildCCParameterName(p,channel),2);
      }
      break;
  }
}

vector<uint8_t> switchBetweenCCLists(uint8_t start, uint8_t end, uint8_t targetParam, uint8_t which, bool left){
  vector<uint8_t> listControls = {start,end,targetParam,which};
  if(left){
    if(which>0){
      which--;
    }
    else
      return listControls;
  }
  else{
    if(which<2){
      which++;
    }
    else
      return listControls;
  }
  switch(which){
    //default
    case 0:
      end = start+6;
      if(end>127){
        end = 127;
        start = end - 6;
      }
      break;
    //mkII
    case 1:
      end = start+6;
      if(end>8){
        end = 8;
        start = 2;
      }
      break;
    //stepchild
    case 2:
      start = 0;
      end = 5;
      break;
  }
  targetParam = start;
  listControls[0] = start;
  listControls[1] = end;
  listControls[2] = targetParam;
  listControls[3] = which;
  return listControls;
}

//menu for selecting a CC parameter
uint8_t selectCCParam_autotrack(uint8_t which){
  //which "list" you choose from
  //can be 0 -- "all" or 1 -- "SP404mkII"
  int8_t targetParam = 0;
  int8_t start = 0;
  uint8_t end = 6;
  if(which == 2)
    end = 5;
  while(true){
    readJoystick();
    readButtons();
    if(itsbeen(200)){
      if(x != 0){
        if(x == -1){
          vector<uint8_t> listControls = switchBetweenCCLists(start,end,targetParam,which,false);
          start = listControls[0];
          end = listControls[1];
          targetParam = listControls[2];
          which = listControls[3];
          lastTime = millis();
        }
        else if(x == 1){
          vector<uint8_t> listControls = switchBetweenCCLists(start,end,targetParam,which,true);
          start = listControls[0];
          end = listControls[1];
          targetParam = listControls[2];
          which = listControls[3];
          lastTime = millis();
        }
      }
      if(sel){
        sel = false;
        lastTime = millis();
        //if it's no longer an internal track, 
        autotrackData[activeAutotrack].parameterType = which;
        if(autotrackData[activeAutotrack].parameterType != 2 && autotrackData[activeAutotrack].channel == 0)
          autotrackData[activeAutotrack].channel = 1; 
        if(which == 1)
          return MKIIParamToCC(targetParam);
        else if(which == 2)
          return stepchildParamToCC(targetParam);
        else
          return targetParam;
      }
      if(menu_Press){
        menu_Press = false;
        lastTime = millis();
        return autotrackData[activeAutotrack].control;
      }
      if(note_Press && which != 2){
        start-= 6;
        if(start<0)
          start = 0;
        end = start + 6;
        targetParam = start;
        lastTime = millis();
      }
      if(track_Press && which != 2){
        end+= 6;
        if(which == 1){
          if(end > 7){
            end = 7;
            start = 1;
          }
        }
        else if(end>127) 
          end = 127;
        start = end - 6;
        targetParam = start;
        lastTime = millis();
      }
    }
    if(itsbeen(100)){
      if(y != 0){
        if(y == -1 && targetParam>0){
          targetParam--;
          lastTime = millis();
        }
        else if(y == 1 && targetParam<127){
          targetParam++;
          if(which == 1){
            if(targetParam > 7)
              targetParam = 7;
          }
          else if(which == 2){
            if(targetParam>4)
              targetParam = 4;
          }
          lastTime = millis();
        }
        
        if(targetParam<start){
          start = targetParam;
          end = start+6;
        }
        else if(targetParam>(end-1)){
          end = targetParam+1;
          start = end-6;
        }
      }
    }

    display.clearDisplay();

    //carriage
    display.drawBitmap(0,1+((millis()/200)%2),carriage_bmp,14,15,SSD1306_WHITE);

    //title
    printCursive(16,0,"midi   list",SSD1306_WHITE);
    printChunky(44,0,"CC",SSD1306_WHITE);

    //list brackets
    switch(which){
      case 0:
        display.fillRect(8,8,14,8,SSD1306_BLACK);
        display.drawLine(0,16,8,16,SSD1306_WHITE);
        display.drawLine(8,16,8,8,SSD1306_WHITE);
        display.drawLine(8,8,22,8,SSD1306_WHITE);
        display.drawLine(22,8,22,16,SSD1306_WHITE);
        display.drawLine(22,16,screenWidth,16,SSD1306_WHITE);
        break;
      case 1:
        display.drawLine(0,16,28,16,SSD1306_WHITE);
        display.drawLine(28,16,28,8,SSD1306_WHITE);
        display.drawLine(28,8,66,8,SSD1306_WHITE);
        display.drawLine(66,8,66,16,SSD1306_WHITE);
        display.drawLine(66,16,screenWidth,16,SSD1306_WHITE);
        break;
      case 2:
        display.drawLine(0,16,72,16,SSD1306_WHITE);
        display.drawLine(72,16,72,8,SSD1306_WHITE);
        display.drawLine(72,8,110,8,SSD1306_WHITE);
        display.drawLine(110,8,110,16,SSD1306_WHITE);
        display.drawLine(110,16,screenWidth,16,SSD1306_WHITE);
        break;
    }

    //list options
    printSmall(10,10,"All",SSD1306_WHITE);
    printSmall(30,10,"SP404mkii",SSD1306_WHITE);
    printSmall(74,10,"stepChild",SSD1306_WHITE);

    //list titles
    printSmall(2,20,"#",SSD1306_WHITE);
    printSmall(20,20,"name",SSD1306_WHITE);

    //parameter values
    printParamList(which,targetParam,start,end,autotrackData[activeAutotrack].channel);

    display.display();
  }

  return targetParam;
}