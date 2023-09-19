void selectInstrumentMenu(){
  uint8_t cursor = 0;
  while(true){
    readButtons();
    joyRead();
    if(itsbeen(100)){
      if(x != 0){
        if(x == -1 && (cursor%4!=3)){
          cursor++;
          lastTime = millis();
        }
        else if(x == 1 && (cursor%4)){
          cursor--;
          lastTime = millis();
        }
      }
      if(y != 0){
        if(y == 1 && (cursor<12)){
          cursor+=4;
          lastTime = millis();
        }
        else if(y == -1 && (cursor>3)){
          cursor-=4;
          lastTime = millis();
        }
      }
    }
    if(itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        menu_Press = false;
        return;
      }
      if(sel){
        lastTime = millis();
        sel = false;
        switch(cursor){
          case 0:
            xyGrid();
            break;
          case 1:
            testSolarSystem();
            break;
          case 2:
            rain();
            break;
          case 3:
            knobs();
            break;
          case 4:
            toggleKeys();
            break;
          case 5:
            toggleDrumPads();
            break;
          case 6:
            MIDItoCV();
            break;
          case 7:
            chordBuilder();
            break;
        }
      }
    }
    display.clearDisplay();
    const uint8_t x1 = 36;
    const uint8_t y1 = 1;
    const uint8_t width = 14;
    const uint8_t height = 14;
    uint8_t count = 0;
    for(uint8_t j = 0; j<4; j++){
      for(uint8_t i = 0; i<4; i++){
        display.drawRoundRect(x1+16*i,y1+16*j+sin(millis()/200+j*4+i),width,height,3,1);
        display.drawBitmap(x1+16*i+1,y1+16*j+sin(millis()/200+j*4+i)+1,instrument_icons[count],12,12,1);
        count++;
      }
    }

    //cursor
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_fill_bmp,9,15,0);
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_outline_bmp,9,15,1);
    
    //title
    // printCursive(-1,0,"instruments",1);
    display.drawPixel(64,5,0);
    String text;
    switch(cursor){
      case 0:
        text = "joystick";
        break;
      case 1:
        text = "Planets";
        break;
      case 2:
        text = "Rain";
        break;
      case 3:
        text = "Knobs";
        break;
      case 4:
        text = "keys";
        break;
      case 5:
        text = "pads";
        break;
      case 6:
        text = "CV";
        break;
      case 7:
        text = "chord";
        break;
    }
    // printArp(0,10,text,1);
    display.setRotation(1);
    // printSmall(0,10,text,1);
    printItalic(screenHeight-text.length()*8,27,text,1);
    display.setRotation(UPRIGHT);
    display.display();
  }
}