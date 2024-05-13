
void drawConsoleTitle(){
  const uint8_t x1 = 86;
  display.fillRect(x1-4,0,33,9,0);
  for(uint8_t i = 0; i<7; i++){
    display.drawBitmap(x1+i*4,1+sin(millis()/100+i),consoleTitle[i],4,7,1);
  }
}


//displays a scrolling list of midi ins and outs
void console(){
  vector<String> midiInMessages;
  vector<String> CCInMessages;

  //stores the last pitch received
  uint8_t lastPitchReceived;
  //stores last pitch
  uint8_t lastVelReceived;
  //stores channel the message was received on 
  uint8_t lastChannelReceived;

  uint8_t lastCCReceived;
  uint8_t lastCCValReceived;
  uint8_t lastCCChannelReceived;

  uint8_t lastPitchSent;
  uint8_t lastVelSent;
  uint8_t lastChannelSent;

  const uint8_t maxLines = 8;

  while(true){
    readButtons();
    if(utils.itsbeen(200)){
      if(menu_Press){
        lastTime = millis();
        return;
      }
    }
    
    display.clearDisplay();
    //topline
    display.drawFastHLine(0,8,114,1);
    //midline
    display.drawFastVLine(44,0,64,1);
    //bounds on graph
    graphics.drawDottedLineV(95,10,64,2);
    graphics.drawDottedLineV(127,15,64,2);
    display.drawBitmap(116,1+((millis()/400)%2),mainMenu_icons[7],12,12,1);

    //note icon
    display.drawBitmap(3,1,epd_bitmap_small_note,5,5,1);
    printSmall(20,1,"V",1);
    display.drawBitmap(34,2,ch_tiny,6,3,1);

    //CC icon
    display.drawBitmap(48,2,cc_tiny,5,3,1);
    printSmall(62,1,"V",1);
    display.drawBitmap(76,2,ch_tiny,6,3,1);

    drawConsoleTitle();

    //printing midi in messages
    for(uint8_t i = 0; i<midiInMessages.size(); i++){
      printSmall(0,10+i*7,midiInMessages[i],1);
    }
    //printing CC in messages
    for(uint8_t i = 0; i<CCInMessages.size(); i++){
      printSmall(48,10+i*7,CCInMessages[i],1);
      //printing line graph
      display.drawFastVLine(95+toInt(CCInMessages[i].substring(8))/4,10+i*7,7,1);
    }
    display.display();

    //note on
    if(noteOnReceived){
      noteOnReceived = false;
      // noteOffReceived = false;
      String text = pitchToString(recentNote.pitch,true,true);
      String text2 = stringify(recentNote.vel);
      String text3 = stringify(recentNote.channel);
      for(int8_t i = 0; i<(3-text2.length()); i++){
        text3 = "  "+text3;
      }
      for(int8_t i = 0; i<(4-text.length()); i++){
        text2 = "  "+text2;
      }
      if(text.charAt(1) != '#'){
        text2 = " "+text2;
      }
      text+=" "+text2+" "+text3;
      //if there are already 8 messages in the buffer
      if(midiInMessages.size()>=maxLines){
        for(uint8_t i = 1; i<maxLines; i++){
          midiInMessages[i-1] = midiInMessages[i];
        }
        midiInMessages[maxLines-1] = text;
      }
      //add the new message
      else{
        midiInMessages.push_back(text);
      }
      lastPitchReceived = recentNote.pitch;
      lastVelReceived = recentNote.vel;
      lastChannelReceived = recentNote.channel;
    } 

    //CC
    if(recentCC.cc != lastCCReceived || recentCC.val != lastCCValReceived || recentCC.channel != lastCCChannelReceived){
        String text = stringify(recentCC.cc);
        String text2 = stringify(recentCC.val);
        String text3 = stringify(recentCC.channel);
        for(int8_t i = 0; i<(3-text2.length()); i++){
          text3 = "  "+text3;
        }
        for(int8_t i = 0; i<(3-text.length()); i++){
          text2 = "  "+text2;
        }
        text+=" "+text2+" "+text3;
      //if there are already 8 messages in the buffer
      if(CCInMessages.size()>=maxLines){
        for(uint8_t i = 1; i<maxLines; i++){
          CCInMessages[i-1] = CCInMessages[i];
        }
        CCInMessages[maxLines - 1] = text;
      }
      //add the new message
      else{
        CCInMessages.push_back(text);
      }
      lastCCReceived = recentCC.cc;
      lastCCValReceived = recentCC.val;
      lastCCChannelReceived = recentCC.channel;
    }

    //Notes sent
  }
}
