#include "Stepchild.h"
#include "StepchildGraphics.h"
#include "graphics/WireFrame.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;
using namespace std;

// uint16_t midiChannelFilter = 65535;
// uint8_t midiMessageFilter = 255;

// void toggleFilter_channel(uint8_t channel){
//   bool state = midiChannelFilter & (1<<channel);
//   uint16_t mask = 1<<channel;//a 1 in the target place

//input menu can set a filter on the input
//two filters: one for channels, one for kinds of messages
//kinds of messages:
/*
- note
- cc
-clock (start, stop, timeframe)

okay, new concept for midi menu:

port icons on top, with an arrow indicating which one you're on.  When you select one, the options
for routing, mute on/off pop up.
*/


// 'MIDI3_mute', 13x13px
const unsigned char MIDI_port_muted_bmp [] = {
	0x08, 0x80, 0x38, 0xe0, 0x47, 0x10, 0x40, 0x10, 0x80, 0x08, 0x88, 0x88, 0x85, 0x08, 0x82, 0x08, 
	0x85, 0x08, 0x48, 0x90, 0x40, 0x10, 0x30, 0x60, 0x0f, 0x80
};
// 'MIDI3_nooutline', 13x13px
const unsigned char MIDI_port_bmp [] = {
	0x08, 0x80, 0x38, 0xe0, 0x7f, 0xf0, 0x7f, 0xf0, 0xff, 0xf8, 0xdf, 0xd8, 0xbf, 0xe8, 0xb7, 0x68, 
	0xed, 0xb8, 0x6d, 0xb0, 0x7d, 0xf0, 0x3f, 0xe0, 0x0f, 0x80
};

// 'usb_test_smaller', 9x15px
const unsigned char usb_logo_bmp [] = {
	0x08, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x08, 0x00, 0x0b, 0x80, 0x4b, 0x80, 0xeb, 0x80, 0x49, 0x00, 
	0x4a, 0x00, 0x2c, 0x00, 0x18, 0x00, 0x08, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00
};

// 'usb_inverse', 11x16px
const unsigned char usb_logo_muted_bmp []  = {
	0x04, 0x00, 0x0a, 0x00, 0x11, 0x00, 0x11, 0xc0, 0x2a, 0x20, 0x5a, 0x20, 0x8a, 0x20, 0x5b, 0x40, 
	0x5a, 0x80, 0x29, 0x00, 0x12, 0x00, 0x0a, 0x00, 0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0e, 0x00
};
// 'thru_arrow', 4x4px
const unsigned char thru_arrow_bmp [] = {
	0x70, 0x30, 0x50, 0x80
};


//small class to help w/ port window opening/closing
//(this could be more efficient, u could ditch the coordinate pairs)
class PortWindow{
  public:
    bool open = false;
    Coordinate coords;
    Coordinate tempCoords;
    PortWindow(Coordinate c){
      coords = c;
      tempCoords = c;
    }
};
class MidiMenu:public StepchildMenu{
  public:
    //cursor for selecting which port options
    uint8_t filterCursor = 0;
    uint8_t filterMenuStart = 0;
    WireFrame icon;
    SequenceRenderSettings settings;

    //displaying port options
    PortWindow portWindow[5] = {
      PortWindow(Coordinate(0,16)),
      PortWindow(Coordinate(16,36)),
      PortWindow(Coordinate(32,36)),
      PortWindow(Coordinate(48,36)),
      PortWindow(Coordinate(53,36))
    };
    MidiMenu(){
      coords = CoordinatePair(0,0,128,64);
      portWindow[0].open = true;
      icon = makeMIDI();
      icon.rotate(-20,0);
      icon.scale = 2;
      icon.offset.x = 12;
      icon.offset.y = 8;
      settings.topLabels = false;
      settings.drawPram = false;
      settings.stepSequencerLEDs = false;
    }
    void writeLEDs(){
      stepchild.buttons.writeLEDs(stepchild.midi.isMuted(cursor)?0:stepchild.midi.midiChannelFilters[cursor]);
    }
    bool midiMenuControls(){
      stepchild.buttons.readButtons();
      stepchild.buttons.readJoystick();
      if(stepchild.itsbeen(100)){
        //changing channel filter cursor
        if(stepchild.buttons.UP()){
          if(filterCursor == 9 && filterMenuStart<8){
            filterMenuStart++;
            if(stepchild.buttons.SELECT()){
              stepchild.midi.toggleMidiChannel(filterCursor-1+filterMenuStart,cursor);
            }
          }
          else if(filterCursor<9){
            filterCursor++;
            if(stepchild.buttons.SELECT()){
              stepchild.midi.toggleMidiChannel(filterCursor-1+filterMenuStart,cursor);
            }
          }
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.DOWN()){
          if(filterCursor == 0 && filterMenuStart>0){
            filterMenuStart--;
            if(stepchild.buttons.SELECT()){
              stepchild.midi.toggleMidiChannel(filterCursor-1+filterMenuStart,cursor);
            }
          }
          else if(filterCursor>0){
            filterCursor--;
            if(stepchild.buttons.SELECT()){
              stepchild.midi.toggleMidiChannel(filterCursor-1+filterMenuStart,cursor);
            }
          }
          stepchild.lastTime = millis();
        }
      }
      if(stepchild.itsbeen(200)){
        if(stepchild.buttons.LEFT() && cursor>0){
          //if a window is open, set it to close and open the next one
          if(portWindow[cursor].open){
            portWindow[cursor].coords.y = 36;
            portWindow[cursor].open = false;
            portWindow[cursor-1].coords.y = 16;
            portWindow[cursor-1].open = true;
          }
          cursor--;
          stepchild.lastTime = millis();
        }
        if(stepchild.buttons.RIGHT() && cursor < 4){
          //if a window is open, set it to close and open the next one
          if(portWindow[cursor].open){
            portWindow[cursor].coords.y = 36;
            portWindow[cursor].open = false;
            portWindow[cursor+1].coords.y = 16;
            portWindow[cursor+1].open = true;
          }
          cursor++;
          stepchild.lastTime = millis();
        }
        //exiting
        if(stepchild.buttons.MENU()){
          stepchild.lastTime = millis();
          return false;
        }
        //use step buttons to toggle channels
        for(uint8_t i = 0; i<16; i++){
          if(stepchild.buttons.stepButton(i)){
            stepchild.midi.toggleMidiChannel(i+1,cursor);
            stepchild.lastTime = millis();
          }
        }
        if(stepchild.buttons.SELECT()){
          switch(filterCursor+filterMenuStart){
            //set mute
            case 0:
              stepchild.midi.toggleMute(cursor);
              break;
            //set thru
            case 1:
              stepchild.midi.toggleThru(cursor);
              break;
            //toggle midi channel filter
            default:
              if(stepchild.buttons.SHIFT()){
                stepchild.midi.setAllChannels(!stepchild.midi.isChannelActive(filterCursor-1+filterMenuStart,cursor),cursor);
              }
              else{
                stepchild.midi.toggleMidiChannel(filterCursor-1+filterMenuStart,cursor);
              }
              break;
          }
          stepchild.lastTime = millis();
        }
      }
      return true;
    }
    void displayMenu(){
      icon.rotate(1,1);
      stepchild.display.clearDisplay();
      graphics.drawSeq(settings);
      graphics.printChunky(coords.start.x+coords.start.y+32,0,"MIDI MENU",1);
      icon.render();

      //port edit box
      stepchild.display.fillRoundRect(coords.start.x+coords.start.y+88,0,43,65,3,0);
      stepchild.display.drawRoundRect(coords.start.x+coords.start.y+88,0,43,65,3,1);

      //port window opening
      for(uint8_t i = 0; i<5; i++){
        //opening
        if(portWindow[i].open){
          if(portWindow[i].tempCoords.y>portWindow[i].coords.y){
            portWindow[i].tempCoords.y-=2;
          }
        }
        //closing
        else{
          if(portWindow[i].tempCoords.y<portWindow[i].coords.y){
            portWindow[i].tempCoords.y+=2;
          }
        }
        //drawing
        if(portWindow[i].tempCoords.y<36){
          String s = "";
          switch(filterCursor+filterMenuStart){
            case 0:
              s = "set mute";
              break;
            case 1:
              s = "set thru";
              break;
            default:
              if(stepchild.buttons.SHIFT())
                s = "toggle all";
              else
                s = "toggle ch"+stringify(filterCursor-1+filterMenuStart);
              break;
          }
          stepchild.display.fillRoundRect(coords.start.x+portWindow[i].tempCoords.x,portWindow[i].tempCoords.y-coords.start.y,graphics.getSmallTextLength(s)+4,36-portWindow[i].tempCoords.y,3,0);
          stepchild.display.drawRoundRect(coords.start.x+portWindow[i].tempCoords.x,portWindow[i].tempCoords.y-coords.start.y,graphics.getSmallTextLength(s)+4,36-portWindow[i].tempCoords.y,3,1);
          graphics.printSmall(coords.start.x+portWindow[i].tempCoords.x+2,portWindow[i].tempCoords.y-coords.start.y+4,"[sel]:",1);
          graphics.printSmall(coords.start.x+portWindow[i].tempCoords.x+2,portWindow[i].tempCoords.y-coords.start.y+10,s,1);
        }
      }
      //port box
      stepchild.display.fillRoundRect(coords.start.x+5,coords.start.y+34,86,32,3,0);
      stepchild.display.drawRoundRect(coords.start.x+5,coords.start.y+34,86,32,3,1);

      const uint8_t xOffset = 6+coords.start.x;
      const uint8_t yOffset = 36+coords.start.y;
      for(uint8_t midiPort = 0; midiPort<5; midiPort++){
        if(midiPort == 0){
          graphics.printSmall(xOffset+3+midiPort*16,yOffset,"USB",SSD1306_WHITE);
          if(stepchild.midi.isMuted(0)){
            stepchild.display.drawBitmap(xOffset+midiPort*16+3,yOffset+6,usb_logo_muted_bmp,11,16,SSD1306_WHITE);
          }
          else{
            stepchild.display.drawBitmap(xOffset+midiPort*16+4,yOffset+7,usb_logo_bmp,9,15,SSD1306_WHITE);
          }
        }
        else{
          graphics.printSmall(xOffset+7+midiPort*16,yOffset,stringify(midiPort),SSD1306_WHITE);
          stepchild.display.drawBitmap(xOffset+midiPort*16+2,yOffset+8,stepchild.midi.isMuted(midiPort)?MIDI_port_muted_bmp:MIDI_port_bmp,13,13,SSD1306_WHITE);
        }
        if(stepchild.midi.isThru(midiPort)){
          stepchild.display.drawBitmap(xOffset+midiPort*16+14,yOffset+5,thru_arrow_bmp,4,4,SSD1306_WHITE);
        }
        if(cursor == midiPort)
          graphics.drawArrow(xOffset+midiPort*16+8,yOffset+24+sin(midiPort+millis()/100),3,ARROW_UP,false);
      }

      const uint8_t x1 = coords.start.x+coords.start.y+100;
      const uint8_t y1 = 3;
      //port info
      for(uint8_t i = 0; i<10; i++){
        //mute/thru info
        if(0 == i+filterMenuStart){
          graphics.printSmall(x1-6,y1+i*6,"mute",1);
          if(filterCursor == i+filterMenuStart)
            graphics.drawArrow(x1-8+(millis()/200)%2,y1+i*6+2,2,ARROW_RIGHT,true);
          graphics.drawLabel(x1+20,y1+i*6,stepchild.midi.isMuted(cursor)?"yes":"nah",true);
        }
        else if(1 == i+filterMenuStart){
          graphics.printSmall(x1-6,y1+i*6,"thru",1);
          graphics.drawLabel(x1+20,y1+i*6,stepchild.midi.isThru(cursor)?"yes":"nah",true);
          if(filterCursor == i+filterMenuStart)
            graphics.drawArrow(x1-8+(millis()/200)%2,y1+i*6+2,2,ARROW_RIGHT,true);
        }
        else{
          //port channel filters
          //print channel numbers
          graphics.printSmall(x1+12, y1+i*6, stringify(i+filterMenuStart-1),SSD1306_WHITE);
          //if channel is active
          if(stepchild.midi.isChannelActive(filterMenuStart+i-1,cursor)){
            //if this box is cursore'd, AND if it's the active midi port
            if(i == filterCursor)
              graphics.drawCheckbox(x1+1, y1+i*6+1, true, true);
            else
              graphics.drawCheckbox(x1+1, y1+i*6+1, true, false);
          }
          else{
            if(i == filterCursor)
              graphics.drawCheckbox(x1+1, y1+i*6+1, false, true);
            else
              graphics.drawCheckbox(x1+1, y1+i*6+1, false, false);
          }
        }
      }
      stepchild.display.setRotation(DISPLAY_SIDEWAYS_R);
      graphics.printSmall(max(16-filterMenuStart*6,2),0,"chnl filters",1);
      stepchild.display.setRotation(DISPLAY_UPRIGHT);
      stepchild.display.display();
    }
};

void midiMenu(){
  MidiMenu menu;
  menu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  while(menu.midiMenuControls()){
    menu.displayMenu();
    menu.writeLEDs();
  }
  menu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  stepchild.buttons.clearButtons();
}
