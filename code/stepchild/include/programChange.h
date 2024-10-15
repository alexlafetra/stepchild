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
        if(PCData[port][event].timestep == sequence.cursorPos){
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
         if (utils.itsbeen(100)) {
            if (controls.joystickX == 1 && !controls.SHIFT()) {
                //if cursor isn't on a measure marker, move it to the nearest one
                if(sequence.cursorPos%sequence.subDivision){
                    sequence.moveCursor(-sequence.cursorPos%sequence.subDivision);
                    lastTime = millis();
                    //moving entire loop
                    if(movingLoop == 2)
                    sequence.moveLoop(-sequence.cursorPos%sequence.subDivision);
                }
                else{
                    sequence.moveCursor(-sequence.subDivision);
                    lastTime = millis();
                    //moving entire loop
                    if(movingLoop == 2)
                    sequence.moveLoop(-sequence.subDivision);
                }
                //moving loop start/end
                if(movingLoop == -1){
                    sequence.setLoopPoint(sequence.cursorPos,true);
                }
                else if(movingLoop == 1){
                    sequence.setLoopPoint(sequence.cursorPos,false);
                }
                }
                if (controls.joystickX == -1 && !controls.SHIFT()) {
                if(sequence.cursorPos%sequence.subDivision){
                    sequence.moveCursor(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
                    lastTime = millis();
                    if(movingLoop == 2)
                    sequence.moveLoop(sequence.subDivision-sequence.cursorPos%sequence.subDivision);
                }
                else{
                    sequence.moveCursor(sequence.subDivision);
                    lastTime = millis();
                    if(movingLoop == 2)
                    sequence.moveLoop(sequence.subDivision);
                }
                //moving loop start/end
                if(movingLoop == -1){
                    sequence.setLoopPoint(sequence.cursorPos,true);
                }
                else if(movingLoop == 1){
                    sequence.setLoopPoint(sequence.cursorPos,false);
                }
            }
        }
        if(utils.itsbeen(100)){
            if (controls.joystickY == 1) {
                if(activePort<4)
                    activePort++;
                lastTime = millis();
            }
            if (controls.joystickY == -1) {
                if(activePort>0)
                    activePort--;
                lastTime = millis();
            }
        }
        if (utils.itsbeen(50)) {
            //moving
            if (controls.joystickX == 1 && controls.SHIFT()) {
            sequence.moveCursor(-1);
            lastTime = millis();
            if(movingLoop == 2)
                sequence.moveLoop(-1);
            else if(movingLoop == -1)
                sequence.setLoopPoint(sequence.cursorPos,true);
            else if(movingLoop == 1)
                sequence.setLoopPoint(sequence.cursorPos,false);
            }
            if (controls.joystickX == -1 && controls.SHIFT()) {
            sequence.moveCursor(1);
            lastTime = millis();
            if(movingLoop == 2)
                sequence.moveLoop(1);
            else if(movingLoop == -1)
                sequence.loopData[sequence.activeLoop].start = sequence.cursorPos;
            else if(movingLoop == 1)
                sequence.loopData[sequence.activeLoop].end = sequence.cursorPos;
            }
        }
    }
    //if in editing mode, jump right to PCMessages
    else{
        if(utils.itsbeen(100)){
            if(controls.joystickX == -1){
                jumpCursorToPC(activePort,true,false);
                lastTime = millis();
            }
            else if(controls.joystickX == 1){
                jumpCursorToPC(activePort,false,false);
                lastTime = millis();
            }
            //jumping up and down tracks
            if(controls.SHIFT()){
                if (controls.joystickY == 1) {
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
                if (controls.joystickY == -1) {
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
                if(controls.joystickY == -1 && editingCursor > 0){
                    editingCursor--;
                    lastTime = millis();
                }
                else if(controls.joystickY == 1 && editingCursor < 1){
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
    for (uint16_t step = sequence.viewStart; step < sequence.viewEnd; step++) {
        unsigned short int x1 = trackDisplay+int((step-sequence.viewStart)*sequence.viewScale);
        unsigned short int x2 = x1 + (step-sequence.viewStart)*sequence.viewScale;
        //measure bars
        if (!(step % sequence.subDivision) && (step%96) && (sequence.subDivision*sequence.viewScale)>1) {
            graphics.drawDottedLineV(x1,9,64,2);
        }
        if(!(step%96)){
            graphics.drawDottedLineV2(x1,9,64,6);
        }
      if(step == sequence.loopData[sequence.activeLoop].start){
          if(movingLoop == -1 || movingLoop == 2){
            display.fillTriangle(trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-3-sin(millis()/50), trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-7-sin(millis()/50), trackDisplay+(step-sequence.viewStart)*sequence.viewScale+4, 9-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(step-sequence.viewStart)*sequence.viewScale,9-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step){
              display.fillTriangle(trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-3, trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-7, trackDisplay+(step-sequence.viewStart)*sequence.viewScale+4, 9-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(step-sequence.viewStart)*sequence.viewScale,9-3,3,SSD1306_WHITE);
            }
            else{
              display.fillTriangle(trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-1, trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-5, trackDisplay+(step-sequence.viewStart)*sequence.viewScale+4, 9-5,SSD1306_WHITE);
            }
          }
        // if(!movingLoop || (movingLoop != 1 && (millis()/400)%2)){
        //   display.drawFastVLine(trackDisplay+(step-sequence.viewStart)*sequence.viewScale,9,screenHeight-9-(sequence.endTrack == sequence.trackData.size()),SSD1306_WHITE);
        //   display.drawFastVLine(trackDisplay+(step-sequence.viewStart)*sequence.viewScale-1,9,screenHeight-9-(sequence.endTrack == sequence.trackData.size()),SSD1306_WHITE);
        // }
      }
      if(step == sequence.loopData[sequence.activeLoop].end-1){
          if(movingLoop == 1 || movingLoop == 2){
            display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-3-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale-4, 9-7-sin(millis()/50), trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-7-sin(millis()/50),SSD1306_WHITE);
            display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale,9-3,3,SSD1306_WHITE);
          }
          else{
            if(sequence.cursorPos == step+1){
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-3, trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale-4, 9-7, trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-7,SSD1306_WHITE);
              display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale,9-3,3,SSD1306_WHITE);
            }
            else{
              display.drawTriangle(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-1, trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale-4, 9-5, trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale, 9-5,SSD1306_WHITE);
            }
          }
        // if(!movingLoop || (movingLoop != -1 && (millis()/400)%2)){
        //   display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale+1,9,screenHeight-9-(sequence.endTrack == sequence.trackData.size()),SSD1306_WHITE);
        //   display.drawFastVLine(trackDisplay+(sequence.loopData[sequence.activeLoop].end-sequence.viewStart)*sequence.viewScale+2,9,screenHeight-9-(sequence.endTrack == sequence.trackData.size()),SSD1306_WHITE);
        // }
      }
      if(movingLoop == 2){
        if(step>sequence.loopData[sequence.activeLoop].start && step<sequence.loopData[sequence.activeLoop].end && step%2){
          display.drawPixel(trackDisplay+(step-sequence.viewStart)*sequence.viewScale, 9-7-sin(millis()/50),SSD1306_WHITE);
        }
      }
      if(step == sequence.loopData[sequence.activeLoop].start+(sequence.loopData[sequence.activeLoop].end-sequence.loopData[sequence.activeLoop].start)/2)
        printSmall(trackDisplay+(step-sequence.viewStart)*sequence.viewScale-1,0,stringify(sequence.activeLoop),SSD1306_WHITE);
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
            if(PCData[port][event].timestep<sequence.viewEnd && PCData[port][event].timestep>=sequence.viewStart){
                if(sequence.cursorPos == PCData[port][event].timestep && activePort == port){
                    display.drawBitmap((PCData[port][event].timestep-sequence.viewStart)*sequence.viewScale+trackDisplay-2,port*portHeight+6,PCIcon_full,15,14,1);
                    printSmall((PCData[port][event].timestep-sequence.viewStart)*sequence.viewScale+trackDisplay,port*portHeight+12,stringify(PCData[port][event].val),0);
                }
                else{
                    display.drawBitmap((PCData[port][event].timestep-sequence.viewStart)*sequence.viewScale+trackDisplay-2,port*portHeight+6,PCIcon_full,15,14,0);
                    display.drawBitmap((PCData[port][event].timestep-sequence.viewStart)*sequence.viewScale+trackDisplay-2,port*portHeight+6,PCIcon_outline,15,14,1);
                    printSmall((PCData[port][event].timestep-sequence.viewStart)*sequence.viewScale+trackDisplay,port*portHeight+12,stringify(PCData[port][event].val),1);
                }
            }
        }
    }
    //drawing the title (if no info should be drawn)
    printItalic(8,0,"PC",1);
    if(sequence.cursorPos<sequence.viewEnd && sequence.cursorPos>=sequence.viewStart){
        graphics.drawArrow((sequence.cursorPos-sequence.viewStart)*sequence.viewScale+trackDisplay+((millis()/200)%2)-1,activePort*portHeight+14,3,0,false);
    }
    if(sequence.playing() && sequence.playheadPos<sequence.viewEnd && sequence.playheadPos>=sequence.viewStart){
        display.drawRoundRect(trackDisplay+(sequence.playheadPos-sequence.viewStart)*sequence.viewScale,9,3, screenHeight-9, 3, SSD1306_WHITE);
    }
    else if(sequence.recording() && sequence.recheadPos<sequence.viewEnd && sequence.recheadPos>=sequence.viewStart){
        display.drawRoundRect(trackDisplay+(sequence.recheadPos-sequence.viewStart)*sequence.viewScale,9,3, screenHeight-9, 3, SSD1306_WHITE);
    }

    //message editing box
    if(editingMessage){
        uint8_t x1;
        //the box will be to the right
        if((sequence.cursorPos-sequence.viewStart)*sequence.viewScale+trackDisplay<64){
            x1 = (sequence.cursorPos-sequence.viewStart)*sequence.viewScale+trackDisplay+15;
        }
        //the box will be to the left
        else{
            x1 = (sequence.cursorPos-sequence.viewStart)*sequence.viewScale+trackDisplay-38;
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
        if(allowSameTS || (PCData[port][event].timestep != sequence.cursorPos)){
            //only care if it's greater than sequence.cursorPos if moving forward
            if(forward){
                if(PCData[port][event].timestep>=sequence.cursorPos){
                    atLeastOne = true;
                    if((PCData[port][event].timestep-sequence.cursorPos) < nearestDist){
                        nearest = event;
                        nearestDist = PCData[port][event].timestep-sequence.cursorPos;
                    }
                }
            }
            //only care if it's less than sequence.cursorPos if moving backwards
            else{
                if(PCData[port][event].timestep<=sequence.cursorPos){
                    atLeastOne = true;
                    if((sequence.cursorPos - PCData[port][event].timestep) < nearestDist){
                        nearest = event;
                        nearestDist = sequence.cursorPos - PCData[port][event].timestep;
                    }
                }
            }
        }
    }
    if(atLeastOne){
        sequence.moveCursor(nearestDist*(forward?1:-1));
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
        if(PCData[port][event].timestep == sequence.cursorPos){
            return true;
        }
    }
    return false;
}

uint16_t getPCIndexAtCursor(uint8_t port){
    for(uint16_t event = 0; event<PCData[port].size(); event++){
        if(PCData[port][event].timestep == sequence.cursorPos){
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
    while(controls.counterA != 0){
        if((*editingCursor) == 0)
            (*editingCursor) = 1;
        if(controls.counterA<0){
            if(PCData[activePort][targetPC].channel>0)
                PCData[activePort][targetPC].channel--;
        }
        if(controls.counterA>0){
            if(PCData[activePort][targetPC].channel<15)
                PCData[activePort][targetPC].channel++;
        }
        controls.counterA += controls.counterA<0?1:-1;
    }

    while(controls.counterB != 0){
        if((*editingCursor) == 1)
            (*editingCursor) = 0;
        if(controls.counterB<0){
            if(controls.SHIFT()){
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
        if(controls.counterB>0){
            if(controls.SHIFT()){
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
        controls.counterB += controls.counterB<0?1:-1;
    }
}

void PCEditor(){
    uint8_t activePort = 0;
    uint8_t editingCursor = 0;
    bool editingMessage = false;
    while(true){
        controls.readButtons();
        controls.readJoystick();
        PCEditor_joystick(activePort,editingMessage,editingCursor);
        if(editingMessage)
            PCEditingEncoderControls(activePort,&editingCursor);
        else
            defaultEncoderControls();
        if(utils.itsbeen(200)){
            defaultLoopControls();
            if(controls.NEW()){
                makePCEvent(activePort,0,0,0,0,sequence.cursorPos);
                lastTime = millis();
            }
            if(controls.MENU()){
                lastTime = millis();
                if(editingMessage){
                    editingMessage = false;
                }
                else{
                    break;
                }
            }
            if(controls.DELETE()){
                deletePCEvent(activePort,sequence.cursorPos);
                editingMessage = false;
                lastTime = millis();
            }
            if(controls.SELECT() ){
                if(!editingMessage && cursorOnPC(activePort))
                    editingMessage = true;
                else
                    editingMessage = false;
                lastTime = millis();
            }
            if(controls.PLAY()){
                togglePlayMode();
                lastTime = millis();
            }
        }
        display.clearDisplay();
        drawPCViewer(activePort,editingMessage,editingCursor);
        display.display();
    }
}
