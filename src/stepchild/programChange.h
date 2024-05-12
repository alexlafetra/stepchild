// 'PCIcon_outline', 15x14px
const unsigned char PCIcon_outline [] = {
	0x20, 0x00, 0x50, 0x00, 0x50, 0x00, 0x5f, 0xf8, 0x80, 0x04, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 
	0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x40, 0x04, 0x3f, 0xf8
};
// 'PCIcon_full', 15x14px
const unsigned char PCIcon_full [] = {
	0x20, 0x00, 0x70, 0x00, 0x70, 0x00, 0x7f, 0xf8, 0xff, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 
	0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0x7f, 0xfc, 0x3f, 0xf8
};

bool jumpCursorToPC(uint8_t port, bool forward, bool allowSameTS);

class ProgramChange{
    public:
        //value message
        uint8_t val;

        //channel of the PC
        uint8_t channel;

        //bank message
        uint8_t bank;
        //sub bank message
        uint8_t subBank;

        //Sub bank and bank aren't real! but they seem like a logical way to implement it. MIDI actually calls these MSB and LSB
        //vals, and they need to be sent as CC messages
        //(not implemented yet)

        //timestep the PC occurs on
        uint16_t timestep;
        ProgramChange(){
        }
        ProgramChange(uint8_t ch, uint8_t b, uint8_t sb, uint8_t v, uint16_t t){
            channel = ch;
            bank = b;
            subBank = sb;
            val = v;
            timestep = t;
        }
};

vector<ProgramChange> PCData[5];

ProgramChange getPCAtCursor(uint8_t port){
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(PCData[port][event].timestep == cursorPos){
            return PCData[port][event];
        }
    }
    return ProgramChange();
}

void playPCData(uint16_t timestep){
    for(uint8_t port = 0; port<5; port++){
        for(uint16_t event = 0; event<PCData[port].size(); event++){
            if(PCData[port][event].timestep == timestep){
                MIDI.sendPC(port,PCData[port][event].val,PCData[port][event].channel);
            }
        }
    }
}

void PCEditor_joystick(uint8_t &activePort, bool editingMode, uint8_t &editingCursor){
    if(!editingMode){
         if (itsbeen(100)) {
            if (x == 1 && !shift) {
                //if cursor isn't on a measure marker, move it to the nearest one
                if(cursorPos%subDivInt){
                    moveCursor(-cursorPos%subDivInt);
                    lastTime = millis();
                    //moving entire loop
                    if(movingLoop == 2)
                    moveLoop(-cursorPos%subDivInt);
                }
                else{
                    moveCursor(-subDivInt);
                    lastTime = millis();
                    //moving entire loop
                    if(movingLoop == 2)
                    moveLoop(-subDivInt);
                }
                //moving loop start/end
                if(movingLoop == -1){
                    setLoopPoint(cursorPos,true);
                }
                else if(movingLoop == 1){
                    setLoopPoint(cursorPos,false);
                }
                }
                if (x == -1 && !shift) {
                if(cursorPos%subDivInt){
                    moveCursor(subDivInt-cursorPos%subDivInt);
                    lastTime = millis();
                    if(movingLoop == 2)
                    moveLoop(subDivInt-cursorPos%subDivInt);
                }
                else{
                    moveCursor(subDivInt);
                    lastTime = millis();
                    if(movingLoop == 2)
                    moveLoop(subDivInt);
                }
                //moving loop start/end
                if(movingLoop == -1){
                    setLoopPoint(cursorPos,true);
                }
                else if(movingLoop == 1){
                    setLoopPoint(cursorPos,false);
                }
            }
        }
        if(itsbeen(100)){
            if (y == 1) {
                if(activePort<4)
                    activePort++;
                lastTime = millis();
            }
            if (y == -1) {
                if(activePort>0)
                    activePort--;
                lastTime = millis();
            }
        }
        if (itsbeen(50)) {
            //moving
            if (x == 1 && shift) {
            moveCursor(-1);
            lastTime = millis();
            if(movingLoop == 2)
                moveLoop(-1);
            else if(movingLoop == -1)
                setLoopPoint(cursorPos,true);
            else if(movingLoop == 1)
                setLoopPoint(cursorPos,false);
            }
            if (x == -1 && shift) {
            moveCursor(1);
            lastTime = millis();
            if(movingLoop == 2)
                moveLoop(1);
            else if(movingLoop == -1)
                loopData[activeLoop].start = cursorPos;
            else if(movingLoop == 1)
                loopData[activeLoop].end = cursorPos;
            }
        }
    }
    //if in editing mode, jump right to PCMessages
    else{
        if(itsbeen(100)){
            if(x == -1){
                jumpCursorToPC(activePort,true,false);
                lastTime = millis();
            }
            else if(x == 1){
                jumpCursorToPC(activePort,false,false);
                lastTime = millis();
            }
            //jumping up and down tracks
            if(shift){
                if (y == 1) {
                    uint8_t originalP = activePort;
                    bool success = false;
                    while(activePort<4){
                        activePort++;
                        //try to jump on the next one
                        if(jumpCursorToPC(activePort,true,true)){
                            success = true;
                            break;
                        }
                        else if(jumpCursorToPC(activePort,false,true)){
                            success = true;
                            break;
                        }
                    }
                    if(!success)
                        activePort = originalP;
                    lastTime = millis();
                }
                if (y == -1) {
                    uint8_t originalP = activePort;
                    bool success = false;
                    while(activePort>0){
                        activePort--;
                        //try to jump on the next one
                        if(jumpCursorToPC(activePort,true,true)){
                            success = true;
                            break;
                        }
                        else if(jumpCursorToPC(activePort,false,true)){
                            success = true;
                            break;
                        }
                    }
                    if(!success)
                        activePort = originalP;
                    lastTime = millis();
                }
            }
            //moving editing cursor
            else{
                if(y == -1 && editingCursor > 0){
                    editingCursor--;
                    lastTime = millis();
                }
                else if(y == 1 && editingCursor < 1){
                    editingCursor++;
                    lastTime = millis();
                }
            }
        }
    }
}

void drawPCViewer(uint8_t activePort, bool editingMessage, uint8_t editingCursor){
    const uint8_t portHeight = 11;
    //grid lines
    for (uint16_t step = viewStart; step < viewEnd; step++) {
        unsigned short int x1 = trackDisplay+int((step-viewStart)*scale);
        unsigned short int x2 = x1 + (step-viewStart)*scale;
        //measure bars
        if (!(step % subDivInt) && (step%96) && (subDivInt*scale)>1) {
            graphics.drawDottedLineV(x1,9,64,2);
        }
        if(!(step%96)){
            graphics.drawDottedLineV2(x1,9,64,6);
        }
      if(step == loopData[activeLoop].start){
          if(movingLoop == -1 || movingLoop == 2){
            display.fillTriangle(trackDisplay+(step-viewStart)*scale, 9-3-sin(millis()/50), trackDisplay+(step-viewStart)*scale, 9-7-sin(millis()/50), trackDisplay+(step-viewStart)*scale+4, 9-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-viewStart)*scale,9-3,3,SSD1306_WHITE);
          }
          else{
            if(cursorPos == step){
              display.fillTriangle(trackDisplay+(step-viewStart)*scale, 9-3, trackDisplay+(step-viewStart)*scale, 9-7, trackDisplay+(step-viewStart)*scale+4, 9-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-viewStart)*scale,9-3,3,SSD1306_WHITE);
            }
            else{
              display.fillTriangle(trackDisplay+(step-viewStart)*scale, 9-1, trackDisplay+(step-viewStart)*scale, 9-5, trackDisplay+(step-viewStart)*scale+4, 9-5,SSD1306_WHITE);
            }
          }
        // if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
        //   display.drawFastVLine(trackDisplay+(step-viewStart)*scale,9,screenHeight-9-(endTrack == trackData.size()),SSD1306_WHITE);
        //   display.drawFastVLine(trackDisplay+(step-viewStart)*scale-1,9,screenHeight-9-(endTrack == trackData.size()),SSD1306_WHITE);
        // }
      }
      if(step == loopData[activeLoop].end-1){
          if(movingLoop == 1 || movingLoop == 2){
            display.drawTriangle(trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-3-sin(millis()/50), trackDisplay+(loopData[activeLoop].end-viewStart)*scale-4, 9-7-sin(millis()/50), trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-viewStart)*scale,9-3,3,SSD1306_WHITE);
          }
          else{
            if(cursorPos == step+1){
              display.drawTriangle(trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-3, trackDisplay+(loopData[activeLoop].end-viewStart)*scale-4, 9-7, trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-viewStart)*scale,9-3,3,SSD1306_WHITE);
            }
            else{
              display.drawTriangle(trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-1, trackDisplay+(loopData[activeLoop].end-viewStart)*scale-4, 9-5, trackDisplay+(loopData[activeLoop].end-viewStart)*scale, 9-5,SSD1306_WHITE);
            }
          }
        // if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
        //   display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-viewStart)*scale+1,9,screenHeight-9-(endTrack == trackData.size()),SSD1306_WHITE);
        //   display.drawFastVLine(trackDisplay+(loopData[activeLoop].end-viewStart)*scale+2,9,screenHeight-9-(endTrack == trackData.size()),SSD1306_WHITE);
        // }
      }
      if(movingLoop == 2){
        if(step>loopData[activeLoop].start && step<loopData[activeLoop].end && step%2){
          display.drawPixel(trackDisplay+(step-viewStart)*scale, 9-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(step == loopData[activeLoop].start+(loopData[activeLoop].end-loopData[activeLoop].start)/2)
        printSmall(trackDisplay+(step-viewStart)*scale-1,0,stringify(activeLoop),SSD1306_WHITE);
    }
    //one lane for each port
    for(uint8_t port = 0; port<5; port++){
        if(port == 0)
            display.drawBitmap(port == activePort?5:0,port*portHeight+13,tiny_usb,10,4,1);
        else{
            display.drawBitmap(port == activePort?5:0,port*portHeight+11,tiny_midi_bmp,7,7,1);
            printSmall(port == activePort?13:8,port*portHeight+12,stringify(port),1);
        }
        for(uint16_t event = 0; event<PCData[port].size(); event++){
            //if the event is in view
            if(PCData[port][event].timestep<viewEnd && PCData[port][event].timestep>=viewStart){
                if(cursorPos == PCData[port][event].timestep && activePort == port){
                    display.drawBitmap((PCData[port][event].timestep-viewStart)*scale+trackDisplay-2,port*portHeight+6,PCIcon_full,15,14,1);
                    printSmall((PCData[port][event].timestep-viewStart)*scale+trackDisplay,port*portHeight+12,stringify(PCData[port][event].val),0);
                }
                else{
                    display.drawBitmap((PCData[port][event].timestep-viewStart)*scale+trackDisplay-2,port*portHeight+6,PCIcon_full,15,14,0);
                    display.drawBitmap((PCData[port][event].timestep-viewStart)*scale+trackDisplay-2,port*portHeight+6,PCIcon_outline,15,14,1);
                    printSmall((PCData[port][event].timestep-viewStart)*scale+trackDisplay,port*portHeight+12,stringify(PCData[port][event].val),1);
                }
            }
        }
    }
    //drawing the title (if no info should be drawn)
    printItalic(8,0,"PC",1);
    if(cursorPos<viewEnd && cursorPos>=viewStart){
        graphics.drawArrow((cursorPos-viewStart)*scale+trackDisplay+((millis()/200)%2)-1,activePort*portHeight+14,3,0,false);
    }
    if(playing && playheadPos<viewEnd && playheadPos>=viewStart){
        display.drawRoundRect(trackDisplay+(playheadPos-viewStart)*scale,9,3, screenHeight-9, 3, SSD1306_WHITE);
    }
    else if(recording && recheadPos<viewEnd && recheadPos>=viewStart){
        display.drawRoundRect(trackDisplay+(recheadPos-viewStart)*scale,9,3, screenHeight-9, 3, SSD1306_WHITE);
    }

    //message editing box
    if(editingMessage){
        uint8_t x1;
        //the box will be to the right
        if((cursorPos-viewStart)*scale+trackDisplay<64){
            x1 = (cursorPos-viewStart)*scale+trackDisplay+15;
        }
        //the box will be to the left
        else{
            x1 = (cursorPos-viewStart)*scale+trackDisplay-38;
        }
        uint8_t y1 = 9+activePort*portHeight;
        if(y1>47)
            y1 = 47;
        display.fillRoundRect(x1,y1,33,16,3,0);
        display.drawRoundRect(x1,y1,33,16,3,1);

        display.fillRect(x1,y1+1+editingCursor*7,33,7,1);

        printSmall(x1+2,y1+2,"val:  "+stringify(getPCAtCursor(activePort).val),2);
        printSmall(x1+2,y1+9,"ch:    "+stringify(getPCAtCursor(activePort).channel+1),2);
    }
}

//jumps the cursor to the next PC event in one direction
bool jumpCursorToPC(uint8_t port, bool forward, bool allowSameTS){
    bool atLeastOne = false;
    uint16_t nearest = 65535;
    uint16_t nearestDist = 65535;
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(allowSameTS || (PCData[port][event].timestep != cursorPos)){
            //only care if it's greater than cursorPos if moving forward
            if(forward){
                if(PCData[port][event].timestep>=cursorPos){
                    atLeastOne = true;
                    if((PCData[port][event].timestep-cursorPos) < nearestDist){
                        nearest = event;
                        nearestDist = PCData[port][event].timestep-cursorPos;
                    }
                }
            }
            //only care if it's less than cursorPos if moving backwards
            else{
                if(PCData[port][event].timestep<=cursorPos){
                    atLeastOne = true;
                    if((cursorPos - PCData[port][event].timestep) < nearestDist){
                        nearest = event;
                        nearestDist = cursorPos - PCData[port][event].timestep;
                    }
                }
            }
        }
    }
    if(atLeastOne){
        moveCursor(nearestDist*(forward?1:-1));
        return true;
    }
    else{
        return false;
    }
}

void makePCEvent(uint8_t port, uint8_t channel, uint8_t bank, uint8_t subBank, uint8_t val, uint16_t timestep){
    PCData[port].push_back(ProgramChange(channel,bank,subBank,val,timestep));
}
void deletePCEvent(uint8_t port, uint16_t timestep){
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(PCData[port][event].timestep == timestep){
            PCData[port].erase(PCData[port].begin()+event);
        }
    }

}

bool cursorOnPC(uint8_t port){
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(PCData[port][event].timestep == cursorPos){
            return true;
        }
    }
    return false;
}

uint16_t getPCIndexAtCursor(uint8_t port){
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(PCData[port][event].timestep == cursorPos){
            return event;
        }
    }
    return 65535;
}

void PCEditingEncoderControls(uint8_t activePort,uint8_t* editingCursor){
    uint16_t targetPC = getPCIndexAtCursor(activePort);
    //if there is no data there, just return
    if(targetPC == 65535)
        return;
    while(counterA != 0){
        if((*editingCursor) == 0)
            (*editingCursor) = 1;
        if(counterA<0){
            if(PCData[activePort][targetPC].channel>0)
                PCData[activePort][targetPC].channel--;
        }
        if(counterA>0){
            if(PCData[activePort][targetPC].channel<15)
                PCData[activePort][targetPC].channel++;
        }
        counterA += counterA<0?1:-1;
    }

    while(counterB != 0){
        if((*editingCursor) == 1)
            (*editingCursor) = 0;
        if(counterB<0){
            if(shift){
                if(PCData[activePort][targetPC].val>10)
                    PCData[activePort][targetPC].val-=10;
                else
                    PCData[activePort][targetPC].val=0;
            }
            else{
                if(PCData[activePort][targetPC].val>0)
                    PCData[activePort][targetPC].val--;
            }
        }
        if(counterB>0){
            if(shift){
                if(PCData[activePort][targetPC].val<117)
                    PCData[activePort][targetPC].val+=10;
                else
                    PCData[activePort][targetPC].val=127;
            }
            else{
                if(PCData[activePort][targetPC].val<127)
                    PCData[activePort][targetPC].val++;
            }
        }
        counterB += counterB<0?1:-1;
    }
}

void PCEditor(){
    uint8_t activePort = 0;
    uint8_t editingCursor = 0;
    bool editingMessage = false;
    while(true){
        readButtons();
        readJoystick();
        PCEditor_joystick(activePort,editingMessage,editingCursor);
        if(editingMessage)
            PCEditingEncoderControls(activePort,&editingCursor);
        else
            defaultEncoderControls();
        if(itsbeen(200)){
            defaultLoopControls();
            if(n){
                makePCEvent(activePort,0,0,0,0,cursorPos);
                lastTime = millis();
            }
            if(menu_Press){
                lastTime = millis();
                if(editingMessage){
                    editingMessage = false;
                }
                else{
                    break;
                }
            }
            if(del){
                deletePCEvent(activePort,cursorPos);
                editingMessage = false;
                lastTime = millis();
            }
            if(sel){
                if(!editingMessage && cursorOnPC(activePort))
                    editingMessage = true;
                else
                    editingMessage = false;
                lastTime = millis();
            }
            if(play){
                togglePlayMode();
                lastTime = millis();
            }
        }
        display.clearDisplay();
        drawPCViewer(activePort,editingMessage,editingCursor);
        display.display();
    }
}
