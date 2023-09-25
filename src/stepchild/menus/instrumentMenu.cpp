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

// 'xy', 12x12px
const unsigned char xy_bmp []  = {
	0x00, 0x00, 0x50, 0xe0, 0x20, 0x20, 0x50, 0xa0, 0x01, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x00, 
	0x50, 0xa0, 0x40, 0x40, 0x70, 0x80, 0x00, 0x00
};
// 'planets', 12x12px
const unsigned char planets_bmp []  = {
	0x00, 0x00, 0x31, 0x00, 0x48, 0x00, 0x68, 0x60, 0x33, 0xa0, 0x04, 0x40, 0x08, 0x60, 0x48, 0xa0, 
	0x09, 0x20, 0x16, 0x40, 0x1b, 0x80, 0x00, 0x00
};
// 'rain', 12x12px
const unsigned char rain_bmp []  = {
	0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x09, 0x00, 0x10, 0x80, 0x22, 0x40, 0x28, 0x40, 0x28, 0x40, 
	0x24, 0x40, 0x10, 0x80, 0x0f, 0x00, 0x00, 0x00
};
// 'knobs', 12x12px
const unsigned char knobs_bmp []  = {
	0x00, 0x00, 0x0f, 0x00, 0x10, 0x80, 0x23, 0x40, 0x30, 0xc0, 0x4f, 0x20, 0x50, 0xa0, 0x50, 0xa0, 
	0x50, 0xa0, 0x20, 0x40, 0x1f, 0x80, 0x00, 0x00
};
// 'keys', 12x12px
const unsigned char keys_bmp []  = {
	0x00, 0x00, 0x6f, 0x60, 0x69, 0x60, 0x69, 0x60, 0x69, 0x60, 0x69, 0x60, 0x6f, 0x60, 0x60, 0x60, 
	0x76, 0xe0, 0x76, 0xe0, 0x76, 0xe0, 0x00, 0x00
};
// 'drum', 12x12px
const unsigned char drum_bmp []  = {
	0x00, 0x00, 0x42, 0x20, 0x28, 0x40, 0x00, 0x00, 0x1f, 0x80, 0x60, 0x60, 0x60, 0x60, 0x5f, 0xa0, 
	0x40, 0x20, 0x20, 0x40, 0x1f, 0x80, 0x00, 0x00
};

// 'cv', 12x12px
const unsigned char cv_icon_bmp[] = {
	0x00, 0x00, 0x1c, 0xe0, 0x22, 0x20, 0x5f, 0xc0, 0x52, 0xa0, 0x5e, 0xa0, 0x22, 0xa0, 0x1e, 0xa0, 
	0x42, 0xa0, 0x41, 0x40, 0x70, 0x80, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 48)
const unsigned char* instrument_icons[16] = {
	xy_bmp,
	rain_bmp,
	knobs_bmp,
	keys_bmp,
	drum_bmp,
	cv_icon_bmp,
  // planets_bmp,
	// epd_bitmap_chord,
  epd_bitmap_empty,
  epd_bitmap_empty,

  epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty,
	epd_bitmap_empty
};

void drawInstrumentPanel(uint8_t x1, uint8_t y1){
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
}

void instrumentPanelAnimation(bool in){
  int16_t animFrame = 100;
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
          display.drawBitmap(x1+16*i+1,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i)+1,instrument_icons[count],12,12,1);
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
    while(animFrame<100){
      const uint8_t width = 14;
      const uint8_t height = 14;
      const uint8_t x1 = 36;
      const uint8_t y1 = 1;
      uint8_t count = 0;
      display.clearDisplay();
      for(uint8_t j = 0; j<4; j++){
        for(uint8_t i = 0; i<4; i++){
          display.drawRoundRect(x1+16*i,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i),width,height,3,1);
          display.drawBitmap(x1+16*i+1,y1+animFrame*(1+count)+16*j+sin(millis()/200+j*4+i)+1,instrument_icons[count],12,12,1);
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
        instrumentPanelAnimation(false);
        return;
      }
      if(sel){
        lastTime = millis();
        sel = false;
        switch(cursor){
          case 0:
            xyGrid();
            break;
          // case 1:
            // testSolarSystem();
            // break;
          case 1:
            rain();
            break;
          case 2:
            knobs();
            break;
          case 3:
            keyboard();
            break;
          case 4:
            drumPad();
            break;
          case 5:
            MIDItoCV();
            break;
          // case 7:
          //   chordBuilder();
          //   break;
        }
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
    String text;
    switch(cursor){
      case 0:
        text = "joystick";
        break;
      // case 1:
      //   text = "Planets";
      //   break;
      case 1:
        text = "Rain";
        break;
      case 2:
        text = "Knobs";
        break;
      case 3:
        text = "keys";
        break;
      case 4:
        text = "pads";
        break;
      case 5:
        text = "CV";
        break;
      // case 7:
      //   text = "chord";
      //   break;
    }
    // printArp(0,10,text,1);
    display.setRotation(1);
    // printSmall(0,10,text,1);
    printItalic(screenHeight-text.length()*8,27,text,1);
    display.setRotation(UPRIGHT);
    display.display();
  }
}
