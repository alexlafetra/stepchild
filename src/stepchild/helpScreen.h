String getTitleText(uint8_t which){
  switch(which){
    case 0:
      return "Sequence Editor";
    case 1:
      return "Track Editor";
    case 2: 
      return "Note Editor";
  }
  return "";
}

uint8_t countStrings(uint8_t which){
  uint8_t size = 0;
  for(int i = 0; i<sizeof(helptext[which])/sizeof(*helptext[which]); i++){
    if(helptext[which][i] != "")
      size++;
  }
  return size;
}

void displayHelpText(uint8_t which){
  uint8_t startLine = 0;//controls starting line
  uint8_t scrollBar = 0;
  uint8_t numberOfLines = 6;
  uint8_t windowLength = 128;
  bool info = false;

  while(true){
    display.clearDisplay();
    readButtons();
    readJoystick();
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        break;
      }
      if(shift){
        info = !info;
        lastTime = millis();
      }
    }
    if(itsbeen(50)){
      if(y != 0){
        if(y == -1 && startLine>0){
          startLine--;
          lastTime = millis();
        }
        else if(y == 1 && startLine+numberOfLines<sizeof(helptext[which])/sizeof(*helptext[which])){
          startLine++;
          lastTime = millis();
        }
      }
    }
    while(counterA != 0 || counterB != 0){
      if(counterA >= 1 || counterB >= 1){
        //if there's a line offscreen
        if(startLine+numberOfLines<23)
          startLine++;
      }
      else if(counterA <= -1||counterB <= -1){
        if(startLine>0)
          startLine--;
      }
      counterA += counterA<0?1:-1;
      counterB += counterB<0?1:-1;
    }
    display.setCursor(0,0);
    display.print(getTitleText(which));
    //drawing the text
    if(info){
      display.drawRect(-1,-1,getTitleText(which).length()*6+4,12,SSD1306_WHITE);
      printSmall_overflow(4,15,4,infotext[which],SSD1306_WHITE);
    }
    else{
      display.drawRect(0,11,windowLength,64,SSD1306_WHITE);
      for(int line = startLine; line<sizeof(helptext[which])/sizeof(*helptext[which]); line++){
        printSmall(4,15+(line-startLine)*7,helptext[which][line],SSD1306_WHITE);
      }
    }
    // //Serial.println(sizeof(helptext[which])/sizeof(*helptext[which]));
    //Serial.println(startLine);
    display.display();
  }
  return;
}