#include "Stepchild.h"
#include "StepchildGraphics.h"

extern StepchildGraphics graphics;
extern Stepchild stepchild;
extern const unsigned char epd_bitmap_small_note[];
using namespace std;


// 'c', 3x7px
const unsigned char consoleTitle_c []  = {
	0x60, 0x80, 0x80, 0x00, 0x80, 0x80, 0x60
};
// 'e', 4x7px
const unsigned char consoleTitle_e []  = {
	0x60, 0x80, 0x80, 0x60, 0x80, 0x80, 0x60
};
// 'l', 4x7px
const unsigned char consoleTitle_l []  = {
	0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x60
};
// 'n', 4x7px
const unsigned char consoleTitle_n []  = {
	0x60, 0x90, 0x90, 0x00, 0x90, 0x90, 0x00
};
// 'o', 4x7px
const unsigned char consoleTitle_o []  = {
	0x60, 0x90, 0x90, 0x00, 0x90, 0x90, 0x60
};
// 's', 4x7px
const unsigned char consoleTitle_s []  = {
	0x60, 0x80, 0x80, 0x60, 0x10, 0x10, 0x60
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 192)
const unsigned char* consoleTitle[7] = {
	consoleTitle_c,
	consoleTitle_o,
	consoleTitle_n,
	consoleTitle_s,
	consoleTitle_o,
	consoleTitle_l,
	consoleTitle_e
};

void drawConsoleTitle(){
  const uint8_t x1 = 86;
  stepchild.display.fillRect(x1-4,0,33,9,0);
  for(uint8_t i = 0; i<7; i++){
    stepchild.display.drawBitmap(x1+i*4,1+sin(millis()/100+i),consoleTitle[i],4,7,1);
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

  uint8_t lastCCReceived = 0;
  uint8_t lastCCValReceived = 0;
  uint8_t lastCCChannelReceived = 0;

//  uint8_t lastPitchSent;
//  uint8_t lastVelSent;
//  uint8_t lastChannelSent;

  const uint8_t maxLines = 8;

  while(true){
    stepchild.buttons.readButtons();
    if(stepchild.itsbeen(200)){
      if(stepchild.buttons.MENU()){
        stepchild.lastTime = millis();
        return;
      }
    }
    
    stepchild.display.clearDisplay();
    //topline
    stepchild.display.drawFastHLine(0,8,114,1);
    //midline
    stepchild.display.drawFastVLine(44,0,64,1);
    //bounds on graph
    graphics.drawDottedLineV(95,10,64,2);
    graphics.drawDottedLineV(127,15,64,2);
    stepchild.display.drawBitmap(116,1+((millis()/400)%2),mainMenu_icons[7],12,12,1);

    //note icon
    stepchild.display.drawBitmap(3,1,epd_bitmap_small_note,5,5,1);
    graphics.printSmall(20,1,"V",1);
    stepchild.display.drawBitmap(34,2,ch_tiny,6,3,1);

    //CC icon
    stepchild.display.drawBitmap(48,2,cc_tiny,5,3,1);
    graphics.printSmall(62,1,"V",1);
    stepchild.display.drawBitmap(76,2,ch_tiny,6,3,1);

    drawConsoleTitle();

    //printing midi in messages
    for(uint8_t i = 0; i<midiInMessages.size(); i++){
      graphics.printSmall(0,10+i*7,midiInMessages[i],1);
    }
    //printing CC in messages
    for(uint8_t i = 0; i<CCInMessages.size(); i++){
      graphics.printSmall(48,10+i*7,CCInMessages[i],1);
      //printing line graph
      stepchild.display.drawFastVLine(95+toInt(CCInMessages[i].substring(8))/4,10+i*7,7,1);
    }
    stepchild.display.display();

    //note on
    if(stepchild.recentNote.vel){
      
      // noteOffReceived = false;
      String text = stepchild.pitchToString(stepchild.recentNote.pitch,true,true);
      String text2 = stringify(stepchild.recentNote.vel);
      String text3 = stringify(stepchild.recentNote.channel);
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
      lastPitchReceived = stepchild.recentNote.pitch;
      lastVelReceived = stepchild.recentNote.vel;
      lastChannelReceived = stepchild.recentNote.channel;
      stepchild.recentNote.vel = 0;
    } 

    //CC
    if(stepchild.recentCC.cc != lastCCReceived || stepchild.recentCC.val != lastCCValReceived || stepchild.recentCC.channel != lastCCChannelReceived){
        String text = stringify(stepchild.recentCC.cc);
        String text2 = stringify(stepchild.recentCC.val);
        String text3 = stringify(stepchild.recentCC.channel);
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
      lastCCReceived = stepchild.recentCC.cc;
      lastCCValReceived = stepchild.recentCC.val;
      lastCCChannelReceived = stepchild.recentCC.channel;
    }

    //Notes sent
  }
}
