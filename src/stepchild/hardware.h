void restartSerial(unsigned int baud){
  Serial.end();
  delay(1000);
  Serial.begin(baud);
}

void hardReset(){
  rp2040.reboot();
}

void resetUSBInterface(){
}

//update mode
void enterBootsel(){
  // display.clearDisplay();
  // display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  // display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}
void writeLEDs(uint8_t data){
  lowerBoard.writeLEDs(data);
}

void writeLEDs(uint8_t led, bool state){
  if(LEDsOn){
    bool leds[8];
    leds[led] = state;
    writeLEDs(leds);
  }
}
//this one turns on a range of LEDS
void writeLEDs(uint8_t first, uint8_t last){
  uint8_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<8; i++){
      dat = dat<<1;
      if(i >= first && i <= last){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  // sending data to shift reg
  // digitalWrite(latchPin_LEDS, LOW);
  // shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  // digitalWrite(latchPin_LEDS, HIGH);
  writeLEDs(dat);
}
void writeLEDs(bool leds[8]){
  uint8_t dat = 0;
  if(LEDsOn){
    for(int i = 0; i<8; i++){
      dat = dat<<1;
      if(leds[i]){
        dat++;
      }
    }
  }
  dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
  writeLEDs(dat);
}

void turnOffLEDs(){
  uint8_t dat = 0;
  // sending data to shift reg
  writeLEDs(dat);
}

void restartDisplay(){
  #ifndef HEADLESS
  Wire.end();
  // Wire.begin();
  display.begin(SCREEN_ADDR,true);
  display.display();
  #endif
}


//array to hold the LED states
//displays notes on LEDs
void updateLEDs(){
  uint8_t dat = 0;//00000000
  if(LEDsOn && !screenSaverActive){
    uint16_t viewLength = viewEnd-viewStart;
    //move through the view, check every subDivInt
    const uint16_t jump = viewLength/8;
    //if there are any notes, check
    if(seqData[activeTrack].size()>1){
      for(uint8_t i = 0; i<8; i++){
        uint16_t step = viewStart+i*jump;
        if(lookupData[activeTrack][step] != 0){
          if(seqData[activeTrack][lookupData[activeTrack][step]].startPos == step){
            //if playing or recording, and the head isn't on that step, it should be on
            //if it is on that step, then the step should blink
            if((playing && (playheadPos <  seqData[activeTrack][lookupData[activeTrack][step]].startPos || playheadPos > seqData[activeTrack][lookupData[activeTrack][step]].endPos)) || !playing){
              dat = dat|(1<<i);
            }
          }
        }
      }
    }
  }

  //reversing dat (since I reversed my shift register for cleanliness)
  // dat = ((dat * 0x0802LU & 0x22110LU) | (dat * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;


  //https://dronebotworkshop.com/shift-registers/

  // sending data to shift reg
  // digitalWrite(latchPin_LEDS, LOW);
  // shiftOut(dataPin_LEDS, clockPin_LEDS, MSBFIRST, dat);
  // digitalWrite(latchPin_LEDS, HIGH);
  writeLEDs(dat);
}



//Sleep---------------------------
//pause core1 (you need to call this fn from core0)
bool sleeping = false;
#ifdef HEADLESS
  void enterSleepMode(){
    return;
  }
  void leaveSleepMode(){
    return;
  }
#else
void leaveSleepMode(){
  //vv these break it? for some reason
  // Serial.end();
  // startSerial();
  restartDisplay();
  rp2040.resumeOtherCore();
  // rp2040.restartCore1(); <-- this also breaks it
  while(!core1ready){//wait for core1
    // //Serial.println("waiting");
    // Serial.flush();
  }
  // //Serial.println("ready");
  // Serial.flush();
  return;
}
void enterSleepMode(){
  //turn off power consuming things
  display.clearDisplay();
  display.display();
  clearButtons();
  turnOffLEDs();
  
  sleeping = true;
  //wait for core1 to sleep
  while(core1ready){
  }
   //idle core1
  rp2040.idleOtherCore();
  //sleep until a change is detected on 
  while(sleeping){
    long time  = millis();
    sleep_ms(1000);
    if(anyActiveInputs()){
      sleeping = false;
    }
    // //Serial.println("sleeping...");
    // //Serial.print(millis()-time);
    // Serial.flush();
  }
  //wake up
  // //Serial.println("gooood morning");
  // Serial.flush();
  leaveSleepMode();
}
#endif


#define BATTSCALE 0.00966796875
//3.0*3.3/1024.0;
//idk why ^^this isn't 4095.0, but it ain't

float getBattLevel(){
  //So when USB is in, VSYS is ~5.0
  //When all 3AA's are in, if they're 1.5v batts VSYS is ~4.5
  //But if they're 1.2v batts VSYS is ~3.6;
  float val = float(analogRead(Vpin))*BATTSCALE;
  return val;
}