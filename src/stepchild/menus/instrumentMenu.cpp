// 'mouse cursor', 9x15px
const unsigned char mouse_cursor_outline_bmp []  = {
	0x00, 0x80, 0x01, 0x80, 0x02, 0x80, 0x04, 0x80, 0x08, 0x80, 0x10, 0x80, 0x20, 0x80, 0x40, 0x80, 
	0xf0, 0x80, 0x12, 0x80, 0x25, 0x80, 0x24, 0x80, 0x48, 0x00, 0x48, 0x00, 0x30, 0x00
};
// 'cursor_fill', 9x15px
const unsigned char mouse_cursor_fill_bmp []  = {
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x1f, 0x00, 0x3f, 0x00, 
	0x0f, 0x00, 0x0d, 0x00, 0x18, 0x00, 0x18, 0x00, 0x30, 0x00, 0x30, 0x00, 0x00, 0x00
};

void drawInstrumentPanel(uint8_t x1, uint8_t y1){
  const uint8_t width = 14;
  const uint8_t height = 14;
  uint8_t count = 0;
  for(uint8_t j = 0; j<4; j++){
    for(uint8_t i = 0; i<4; i++){
      display.drawRoundRect(x1+16*i,y1+16*j+sin(millis()/200+j*4+i),width,height,3,1);
      // display.drawBitmap(x1+16*i+1,y1+16*j+sin(millis()/200+j*4+i)+1,instrument_icons[count],12,12,1);
      display.drawBitmap(x1+16*i+1,y1+16*j+sin(millis()/200+j*4+i)+1,instrumentApplicationIcons[count],12,12,1);
      count++;
    }
  }
}

void instrumentPanelAnimation(bool in){
  int16_t animFrame = 50;
  if(in){
    while(animFrame>0){
      const uint8_t width = 14;
      const uint8_t height = 14;
      const uint8_t x1 = 36;
      const uint8_t y1 = 1;
      uint8_t count = 0;
      display.clearDisplay();
      for(uint8_t j = 0; j<4; j++){
        for(uint8_t i = 0; i<4; i++){
          display.drawRoundRect(x1+16*i,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i),width,height,3,1);
          display.drawBitmap(x1+16*i+1,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i)+1,instrumentApplicationIcons[count],12,12,1);
          count++;
        }
      }
      display.display();
      animFrame-=4;
      // delay(100);
    }
  }
  else{
    animFrame = 0;
    while(animFrame<50){
      const uint8_t width = 14;
      const uint8_t height = 14;
      const uint8_t x1 = 36;
      const uint8_t y1 = 1;
      uint8_t count = 0;
      display.clearDisplay();
      for(uint8_t j = 0; j<4; j++){
        for(uint8_t i = 0; i<4; i++){
          display.drawRoundRect(x1+16*i,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i),width,height,3,1);
          display.drawBitmap(x1+16*i+1,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i)+1,instrumentApplicationIcons[count],12,12,1);
          count++;
        }
      }
      display.display();
      animFrame+=4;
      // delay(100);
    }
  }
}

void selectInstrumentMenu(){
  instrumentPanelAnimation(true);
  lastTime = millis();
  uint8_t cursor = 0;
  while(true){
    readButtons();
    readJoystick();
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
        instrumentPanelAnimation(false);
        return;
      }
      if(sel){
        lastTime = millis();
        sel = false;
        instrumentApplicationFunctions[cursor]();
      }
    }
    display.clearDisplay();
    const uint8_t x1 = 36;
    const uint8_t y1 = 1;
    const uint8_t width = 14;
    const uint8_t height = 14;
    drawInstrumentPanel(x1,y1);

    //cursor
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_fill_bmp,9,15,0);
    display.drawBitmap(x1+16*(cursor%4)+width/2-12,y1+16*(cursor/4)+height/2+2,mouse_cursor_outline_bmp,9,15,1);
    
    //title
    // printCursive(-1,0,"instruments",1);
    display.drawPixel(64,5,0);
    String text = instrumentApplicationTexts[cursor];
    // printArp(0,10,text,1);
    display.setRotation(1);
    // printSmall(0,10,text,1);
    printItalic(screenHeight-text.length()*8,27,text,1);
    display.setRotation(UPRIGHT);
    display.display();
  }
}
