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
      icon.xPos = 12;
      icon.yPos = 8;
    }

    bool midiMenuControls(){
      controls.readButtons();
      controls.readJoystick();
      if(utils.itsbeen(100)){
        //changing channel filter cursor
        if(controls.UP()){
          if(filterCursor == 9 && filterMenuStart<8){
            filterMenuStart++;
            if(controls.SELECT()){
              MIDI.toggleMidiChannel(filterCursor-2+filterMenuStart,cursor);
            }
          }
          else if(filterCursor<9){
            filterCursor++;
            if(controls.SELECT()){
              MIDI.toggleMidiChannel(filterCursor-2+filterMenuStart,cursor);
            }
          }
          lastTime = millis();
        }
        if(controls.DOWN()){
          if(filterCursor == 0 && filterMenuStart>0){
            filterMenuStart--;
            if(controls.SELECT()){
              MIDI.toggleMidiChannel(filterCursor-2+filterMenuStart,cursor);
            }
          }
          else if(filterCursor>0){
            filterCursor--;
            if(controls.SELECT()){
              MIDI.toggleMidiChannel(filterCursor-2+filterMenuStart,cursor);
            }
          }
          lastTime = millis();
        }
      }
      if(utils.itsbeen(200)){
        if(controls.LEFT() && cursor>0){
          //if a window is open, set it to close and open the next one
          if(portWindow[cursor].open){
            portWindow[cursor].coords.y = 36;
            portWindow[cursor].open = false;
            portWindow[cursor-1].coords.y = 16;
            portWindow[cursor-1].open = true;
          }
          cursor--;
          lastTime = millis();
        }
        if(controls.RIGHT() && cursor < 4){
          //if a window is open, set it to close and open the next one
          if(portWindow[cursor].open){
            portWindow[cursor].coords.y = 36;
            portWindow[cursor].open = false;
            portWindow[cursor+1].coords.y = 16;
            portWindow[cursor+1].open = true;
          }
          cursor++;
          lastTime = millis();
        }
        //exiting
        if(controls.MENU()){
          lastTime = millis();
          return false;
        }
        if(controls.SELECT()){
          switch(filterCursor+filterMenuStart){
            //set mute
            case 0:
              MIDI.toggleMute(cursor);
              break;
            //set thru
            case 1:
              MIDI.toggleThru(cursor);
              break;
            //toggle midi channel filter
            default:
              if(controls.SHIFT()){
                MIDI.setAllChannels(!MIDI.isChannelActive(filterCursor-2+filterMenuStart,cursor),cursor);
              }
              else{
                MIDI.toggleMidiChannel(filterCursor-2+filterMenuStart,cursor);
              }
              break;
          }
          lastTime = millis();
        }
      }
      return true;
    }
    void displayMenu(){
      SequenceRenderSettings settings;
      settings.topLabels = false;
      settings.drawPram = false;
      icon.rotate(1,1);
      display.clearDisplay();
      drawSeq(settings);
      printChunky(coords.start.x+coords.start.y+32,0,"MIDI MENU",1);
      icon.render();

      //port edit box
      display.fillRoundRect(coords.start.x+coords.start.y+88,0,43,65,3,0);
      display.drawRoundRect(coords.start.x+coords.start.y+88,0,43,65,3,1);

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
              if(controls.SHIFT())
                s = "toggle all";
              else
                s = "toggle ch"+stringify(filterCursor-1+filterMenuStart);
              break;
          }
          display.fillRoundRect(coords.start.x+portWindow[i].tempCoords.x,portWindow[i].tempCoords.y-coords.start.y,getSmallTextLength(s)+4,36-portWindow[i].tempCoords.y,3,0);
          display.drawRoundRect(coords.start.x+portWindow[i].tempCoords.x,portWindow[i].tempCoords.y-coords.start.y,getSmallTextLength(s)+4,36-portWindow[i].tempCoords.y,3,1);
          printSmall(coords.start.x+portWindow[i].tempCoords.x+2,portWindow[i].tempCoords.y-coords.start.y+4,"[sel]:",1);
          printSmall(coords.start.x+portWindow[i].tempCoords.x+2,portWindow[i].tempCoords.y-coords.start.y+10,s,1);
        }
      }
      //port box
      display.fillRoundRect(coords.start.x+5,coords.start.y+34,86,32,3,0);
      display.drawRoundRect(coords.start.x+5,coords.start.y+34,86,32,3,1);

      const uint8_t xOffset = 6+coords.start.x;
      const uint8_t yOffset = 36+coords.start.y;
      for(uint8_t midiPort = 0; midiPort<5; midiPort++){
        if(midiPort == 0){
          printSmall(xOffset+3+midiPort*16,yOffset,"USB",SSD1306_WHITE);
          if(MIDI.isMuted(0)){
            display.drawBitmap(xOffset+midiPort*16+3,yOffset+6,usb_logo_muted_bmp,11,16,SSD1306_WHITE);
          }
          else{
            display.drawBitmap(xOffset+midiPort*16+4,yOffset+7,usb_logo_bmp,9,15,SSD1306_WHITE);
          }
        }
        else{
          printSmall(xOffset+7+midiPort*16,yOffset,stringify(midiPort),SSD1306_WHITE);
          display.drawBitmap(xOffset+midiPort*16+2,yOffset+8,MIDI.isMuted(midiPort)?MIDI_port_muted_bmp:MIDI_port_bmp,13,13,SSD1306_WHITE);
        }
        if(MIDI.isThru(midiPort)){
          display.drawBitmap(xOffset+midiPort*16+14,yOffset+5,thru_arrow_bmp,4,4,SSD1306_WHITE);
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
          printSmall(x1-6,y1+i*6,"mute",1);
          if(filterCursor == i+filterMenuStart)
            graphics.drawArrow(x1-8+(millis()/200)%2,y1+i*6+2,2,ARROW_RIGHT,true);
          graphics.drawLabel(x1+20,y1+i*6,MIDI.isMuted(cursor)?"yes":"nah",true);
        }
        else if(1 == i+filterMenuStart){
          printSmall(x1-6,y1+i*6,"thru",1);
          graphics.drawLabel(x1+20,y1+i*6,MIDI.isThru(cursor)?"yes":"nah",true);
          if(filterCursor == i+filterMenuStart)
            graphics.drawArrow(x1-8+(millis()/200)%2,y1+i*6+2,2,ARROW_RIGHT,true);
        }
        else{
          //port channel filters
          //print channel numbers
          printSmall(x1+12, y1+i*6, stringify(i+filterMenuStart-1),SSD1306_WHITE);
          //if channel is active
          if(MIDI.isChannelActive(filterMenuStart+i-2,cursor)){
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
      display.setRotation(DISPLAY_SIDEWAYS_R);
      printSmall(16,0,"chnl filters",1);
      display.setRotation(DISPLAY_UPRIGHT);
      display.display();
    }
};

void midiMenu(){
  MidiMenu menu;
  menu.slideIn(IN_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  while(menu.midiMenuControls()){
    menu.displayMenu();
  }
  menu.slideOut(OUT_FROM_BOTTOM,MENU_SLIDE_MEDIUM);
  controls.clearButtons();
}
