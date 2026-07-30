#include "Stepchild.h"

#include "mainSequence.h"

;

using namespace std;

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

ProgramChange getPCAtCursor(uint8_t port){
    for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
        if(stepchild.PCData[port][event].timestep == stepchild.cursorPos){
            return stepchild.PCData[port][event];
        }
    }
    return ProgramChange();
}

void PCEditor_joystick(uint8_t &activePort, bool editingMode, uint8_t &editingCursor){
    if(!editingMode){
         if (stepchild.itsbeen(100)) {
            if (stepchild.buttons.joystickX == 1 && !stepchild.buttons.SHIFT()) {
                //if cursor isn't on a measure marker, move it to the nearest one
                if(stepchild.cursorPos%stepchild.subDivision){
                    stepchild.moveCursor(-stepchild.cursorPos%stepchild.subDivision);
                    stepchild.lastTime = millis();
                    //moving entire loop
                    if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                    stepchild.moveLoop(-stepchild.cursorPos%stepchild.subDivision);
                }
                else{
                    stepchild.moveCursor(-stepchild.subDivision);
                    stepchild.lastTime = millis();
                    //moving entire loop
                    if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                    stepchild.moveLoop(-stepchild.subDivision);
                }
                //moving loop start/end
                if(stepchild.movingLoop == MOVING_LOOP_END){
                    stepchild.setLoopPoint(stepchild.cursorPos,true);
                }
                else if(stepchild.movingLoop == MOVING_LOOP_START){
                    stepchild.setLoopPoint(stepchild.cursorPos,false);
                }
                }
                if (stepchild.buttons.joystickX == -1 && !stepchild.buttons.SHIFT()) {
                if(stepchild.cursorPos%stepchild.subDivision){
                    stepchild.moveCursor(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
                    stepchild.lastTime = millis();
                    if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                    stepchild.moveLoop(stepchild.subDivision-stepchild.cursorPos%stepchild.subDivision);
                }
                else{
                    stepchild.moveCursor(stepchild.subDivision);
                    stepchild.lastTime = millis();
                    if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                    stepchild.moveLoop(stepchild.subDivision);
                }
                //moving loop start/end
                if(stepchild.movingLoop == MOVING_LOOP_END){
                    stepchild.setLoopPoint(stepchild.cursorPos,true);
                }
                else if(stepchild.movingLoop == MOVING_LOOP_START){
                    stepchild.setLoopPoint(stepchild.cursorPos,false);
                }
            }
        }
        if(stepchild.itsbeen(100)){
            if (stepchild.buttons.joystickY == 1) {
                if(activePort<4)
                    activePort++;
                stepchild.lastTime = millis();
            }
            if (stepchild.buttons.joystickY == -1) {
                if(activePort>0)
                    activePort--;
                stepchild.lastTime = millis();
            }
        }
        if (stepchild.itsbeen(50)) {
            //moving
            if (stepchild.buttons.joystickX == 1 && stepchild.buttons.SHIFT()) {
            stepchild.moveCursor(-1);
            stepchild.lastTime = millis();
            if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                stepchild.moveLoop(-1);
            else if(stepchild.movingLoop == MOVING_LOOP_END)
                stepchild.setLoopPoint(stepchild.cursorPos,true);
            else if(stepchild.movingLoop == MOVING_LOOP_START)
                stepchild.setLoopPoint(stepchild.cursorPos,false);
            }
            if (stepchild.buttons.joystickX == -1 && stepchild.buttons.SHIFT()) {
            stepchild.moveCursor(1);
            stepchild.lastTime = millis();
            if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS)
                stepchild.moveLoop(1);
            else if(stepchild.movingLoop == MOVING_LOOP_END)
                stepchild.loopData[stepchild.activeLoop].start = stepchild.cursorPos;
            else if(stepchild.movingLoop == MOVING_LOOP_START)
                stepchild.loopData[stepchild.activeLoop].end = stepchild.cursorPos;
            }
        }
    }
    //if in editing mode, jump right to PCMessages
    else{
        if(stepchild.itsbeen(100)){
            if(stepchild.buttons.joystickX == -1){
                jumpCursorToPC(activePort,true,false);
                stepchild.lastTime = millis();
            }
            else if(stepchild.buttons.joystickX == 1){
                jumpCursorToPC(activePort,false,false);
                stepchild.lastTime = millis();
            }
            //jumping up and down tracks
            if (stepchild.buttons.joystickY == 1) {
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
                stepchild.lastTime = millis();
            }
            if (stepchild.buttons.joystickY == -1) {
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
                stepchild.lastTime = millis();
            }
        }
    }
}

void drawPCViewer(uint8_t activePort, bool editingMessage, uint8_t editingCursor){
    const uint8_t portHeight = 11;
    //grid lines -- THIS IS REALLY INEFFICIENT!! you shouldn't check each step here
    for (uint16_t step = stepchild.viewStart; step < stepchild.viewEnd; step++) {
        unsigned short int x1 = stepchild.TRACK_LABEL_WIDTH+int((step-stepchild.viewStart)*stepchild.viewScale);
        //measure bars
        if (!(step % stepchild.subDivision) && (step%96) && (stepchild.subDivision*stepchild.viewScale)>1) {
            graphics.drawDottedLineV(x1,9,64,2);
        }
        if(!(step%96)){
            graphics.drawDottedLineV2(x1,9,64,6);
        }
        //loop point stuff
        if(step == stepchild.loopData[stepchild.activeLoop].start){
            if(stepchild.movingLoop == MOVING_LOOP_END || stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
                stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-3-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-7-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale+4, 9-7-sin(millis()/50),SSD1306_WHITE);
                stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale,9-3,3,SSD1306_WHITE);
            }
            else{
                if(stepchild.cursorPos == step){
                    stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-3, stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-7, stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale+4, 9-7,SSD1306_WHITE);
                    stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale,9-3,3,SSD1306_WHITE);
                }
                else{
                    stepchild.display.fillTriangle(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-1, stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-5, stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale+4, 9-5,SSD1306_WHITE);
                }
            }
        }
        if(step == stepchild.loopData[stepchild.activeLoop].end-1){
            if(stepchild.movingLoop == MOVING_LOOP_START || stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
                stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-3-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale-4, 9-7-sin(millis()/50), stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-7-sin(millis()/50),SSD1306_WHITE);
                stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale,9-3,3,SSD1306_WHITE);
            }
            else{
                if(stepchild.cursorPos == step+1){
                    stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-3, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale-4, 9-7, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-7,SSD1306_WHITE);
                    stepchild.display.drawFastVLine(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale,9-3,3,SSD1306_WHITE);
                }
                else{
                    stepchild.display.drawTriangle(stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-1, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale-4, 9-5, stepchild.TRACK_LABEL_WIDTH+(stepchild.loopData[stepchild.activeLoop].end-stepchild.viewStart)*stepchild.viewScale, 9-5,SSD1306_WHITE);
                }
            }
        }
        if(stepchild.movingLoop == MOVING_BOTH_LOOP_POINTS){
            if(step>stepchild.loopData[stepchild.activeLoop].start && step<stepchild.loopData[stepchild.activeLoop].end && step%2){
                stepchild.display.drawPixel(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale, 9-7-sin(millis()/50),SSD1306_WHITE);
            }
        }
        if(step == stepchild.loopData[stepchild.activeLoop].start+(stepchild.loopData[stepchild.activeLoop].end-stepchild.loopData[stepchild.activeLoop].start)/2)
            graphics.printSmall(stepchild.TRACK_LABEL_WIDTH+(step-stepchild.viewStart)*stepchild.viewScale-1,0,stringify(stepchild.activeLoop),SSD1306_WHITE);
    }
    //one lane for each port
    for(uint8_t port = 0; port<5; port++){
        if(port == activePort)
            stepchild.display.fillRoundRect(3,port*portHeight+9,30,11,3,1);
        if(port == 0)
            stepchild.display.drawBitmap(port == activePort?5:0,port*portHeight+13,tiny_usb,10,4,2);
        else{
            stepchild.display.drawBitmap(port == activePort?5:0,port*portHeight+11,tiny_midi_bmp,7,7,2);
            graphics.printSmall(port == activePort?13:8,port*portHeight+12,stringify(port),2);
        }
        for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
            //if the event is in view
            if(stepchild.PCData[port][event].timestep<stepchild.viewEnd && stepchild.PCData[port][event].timestep>=stepchild.viewStart){
                if(stepchild.playing() && stepchild.playheadPos == stepchild.PCData[port][event].timestep ){
                    stepchild.display.drawBitmap((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-2,port*portHeight+6,PCIcon_full,15,14,1);
                    graphics.printSmall((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH,port*portHeight+12,stringify(stepchild.PCData[port][event].val),0);
                }
                else if(stepchild.cursorPos == stepchild.PCData[port][event].timestep && activePort == port && (millis()%500>250)){
                    stepchild.display.drawBitmap((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-2,port*portHeight+6,PCIcon_full,15,14,1);
                    graphics.printSmall((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH,port*portHeight+12,stringify(stepchild.PCData[port][event].val),0);
                }
                else{
                    stepchild.display.drawBitmap((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-2,port*portHeight+6,PCIcon_full,15,14,0);
                    stepchild.display.drawBitmap((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-2,port*portHeight+6,PCIcon_outline,15,14,1);
                    graphics.printSmall((stepchild.PCData[port][event].timestep-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH,port*portHeight+12,stringify(stepchild.PCData[port][event].val),1);
                }
            }
        }
    }
    //drawing the title (if no info should be drawn)
    // graphics.printItalic(8,0,"PC",1);
    graphics.printItalic(0,0,"P",1);
    graphics.printSmall(6,3,"rg",1);
    graphics.printItalic(14,0,"C",1);
    graphics.printSmall(20,3,"hng",1);
    if(stepchild.cursorPos<stepchild.viewEnd && stepchild.cursorPos>=stepchild.viewStart){
        graphics.drawArrow((stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH+((millis()/200)%2)-1,activePort*portHeight+14,3,ARROW_RIGHT,false);
        if(millis()%500>250)
            stepchild.display.drawFastVLine((stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH,8,stepchild.SCREEN_HEIGHT-8,1);
    }
    if(stepchild.playing() && stepchild.playheadPos<stepchild.viewEnd && stepchild.playheadPos>=stepchild.viewStart){
        stepchild.display.drawRoundRect(stepchild.TRACK_LABEL_WIDTH+(stepchild.playheadPos-stepchild.viewStart)*stepchild.viewScale,9,3, stepchild.SCREEN_HEIGHT-9, 3, SSD1306_WHITE);
    }
    else if(stepchild.recording() && stepchild.recheadPos<stepchild.viewEnd && stepchild.recheadPos>=stepchild.viewStart){
        stepchild.display.drawRoundRect(stepchild.TRACK_LABEL_WIDTH+(stepchild.recheadPos-stepchild.viewStart)*stepchild.viewScale,9,3, stepchild.SCREEN_HEIGHT-9, 3, SSD1306_WHITE);
    }

    //message editing box
    if(editingMessage){
        uint8_t x1;
        bool toTheRight;
        //the box will be to the right
        if((stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH<64){
            x1 = (stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH+15;
            toTheRight = true;
        }
        //the box will be to the left
        else{
            x1 = (stepchild.cursorPos-stepchild.viewStart)*stepchild.viewScale+stepchild.TRACK_LABEL_WIDTH-38;
            toTheRight = false;
        }
        uint8_t y1 = 9+activePort*portHeight;
        if(y1>47)
            y1 = 47;
        stepchild.display.fillRoundRect(x1,y1,33,16,3,0);
        stepchild.display.drawRoundRect(x1,y1,33,16,3,1);

        stepchild.display.fillRect(x1,y1+1+editingCursor*7,33,7,1);

        graphics.printSmall(x1+2,y1+2,"val:  "+stringify(getPCAtCursor(activePort).val),2);
        graphics.printSmall(x1+2,y1+9,"ch:    "+stringify(getPCAtCursor(activePort).channel+1),2);

        if(!toTheRight){
            stepchild.display.fillCircle(x1-6,y1+2,4,1);
            graphics.printSmall(x1-7,y1,"A",0);
            stepchild.display.fillCircle(x1-6,y1+12,4,1);
            graphics.printSmall(x1-7,y1+10,"B",0);
        }
        else{
            stepchild.display.fillCircle(x1+38,y1+2,4,1);
            graphics.printSmall(x1+37,y1,"A",0);
            stepchild.display.fillCircle(x1+38,y1+12,4,1);
            graphics.printSmall(x1+37,y1+10,"B",0);
        }

    }
}

//jumps the cursor to the next PC event in one direction
bool jumpCursorToPC(uint8_t port, bool forward, bool allowSameTS){
    bool atLeastOne = false;
    uint16_t nearest = 65535;
    uint16_t nearestDist = 65535;
    for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
        if(allowSameTS || (stepchild.PCData[port][event].timestep != stepchild.cursorPos)){
            //only care if it's greater than stepchild.cursorPos if moving forward
            if(forward){
                if(stepchild.PCData[port][event].timestep>=stepchild.cursorPos){
                    atLeastOne = true;
                    if((stepchild.PCData[port][event].timestep-stepchild.cursorPos) < nearestDist){
                        nearest = event;
                        nearestDist = stepchild.PCData[port][event].timestep-stepchild.cursorPos;
                    }
                }
            }
            //only care if it's less than stepchild.cursorPos if moving backwards
            else{
                if(stepchild.PCData[port][event].timestep<=stepchild.cursorPos){
                    atLeastOne = true;
                    if((stepchild.cursorPos - stepchild.PCData[port][event].timestep) < nearestDist){
                        nearest = event;
                        nearestDist = stepchild.cursorPos - stepchild.PCData[port][event].timestep;
                    }
                }
            }
        }
    }
    if(atLeastOne){
        stepchild.moveCursor(nearestDist*(forward?1:-1));
        return true;
    }
    else{
        return false;
    }
}

void makePCEvent(uint8_t port, uint8_t channel, uint8_t bank, uint8_t subBank, uint8_t val, uint16_t timestep){
    stepchild.PCData[port].push_back(ProgramChange(channel,bank,subBank,val,timestep));
}
void deletePCEvent(uint8_t port, uint16_t timestep){
    for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
        if(stepchild.PCData[port][event].timestep == timestep){
            stepchild.PCData[port].erase(stepchild.PCData[port].begin()+event);
        }
    }

}

bool cursorOnPC(uint8_t port){
    for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
        if(stepchild.PCData[port][event].timestep == stepchild.cursorPos){
            return true;
        }
    }
    return false;
}

uint16_t getPCIndexAtCursor(uint8_t port){
    for(uint16_t event = 0; event<stepchild.PCData[port].size(); event++){
        if(stepchild.PCData[port][event].timestep == stepchild.cursorPos){
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
    while(stepchild.buttons.counterA != 0){
        if((*editingCursor) == 0)
            (*editingCursor) = 1;
        if(stepchild.buttons.counterA<0){
            if(stepchild.PCData[activePort][targetPC].channel>1)
                stepchild.PCData[activePort][targetPC].channel--;
        }
        if(stepchild.buttons.counterA>0){
            if(stepchild.PCData[activePort][targetPC].channel<16)
                stepchild.PCData[activePort][targetPC].channel++;
        }
        stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
    }

    while(stepchild.buttons.counterB != 0){
        if((*editingCursor) == 1)
            (*editingCursor) = 0;
        if(stepchild.buttons.counterB<0){
            if(stepchild.buttons.SHIFT()){
                if(stepchild.PCData[activePort][targetPC].val>10)
                    stepchild.PCData[activePort][targetPC].val-=10;
                else
                    stepchild.PCData[activePort][targetPC].val=0;
            }
            else{
                if(stepchild.PCData[activePort][targetPC].val>0)
                    stepchild.PCData[activePort][targetPC].val--;
            }
        }
        if(stepchild.buttons.counterB>0){
            if(stepchild.buttons.SHIFT()){
                if(stepchild.PCData[activePort][targetPC].val<117)
                    stepchild.PCData[activePort][targetPC].val+=10;
                else
                    stepchild.PCData[activePort][targetPC].val=127;
            }
            else{
                if(stepchild.PCData[activePort][targetPC].val<127)
                    stepchild.PCData[activePort][targetPC].val++;
            }
        }
        stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
    }
}

void PCEditor(){
    uint8_t activePort = 0;
    uint8_t editingCursor = 0;
    bool editingMessage = false;
    while(true){
        stepchild.buttons.readButtons();
        stepchild.buttons.readJoystick();
        PCEditor_joystick(activePort,editingMessage,editingCursor);
        if(editingMessage)
            PCEditingEncoderControls(activePort,&editingCursor);
        else
            defaultEncoderControls();
        if(stepchild.itsbeen(200)){
            defaultLoopControls();
            if(stepchild.buttons.NEW()){
                makePCEvent(activePort,0,0,0,0,stepchild.cursorPos);
                stepchild.lastTime = millis();
            }
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                if(editingMessage){
                    editingMessage = false;
                }
                else{
                    break;
                }
            }
            if(stepchild.buttons.DELETE()){
                deletePCEvent(activePort,stepchild.cursorPos);
                editingMessage = false;
                stepchild.lastTime = millis();
            }
            if(stepchild.buttons.SELECT() ){
                if(!editingMessage && cursorOnPC(activePort))
                    editingMessage = true;
                else
                    editingMessage = false;
                stepchild.lastTime = millis();
            }
            if(stepchild.buttons.PLAY()){
                stepchild.togglePlay();
                stepchild.lastTime = millis();
            }
        }
        stepchild.display.clearDisplay();
        drawPCViewer(activePort,editingMessage,editingCursor);
        stepchild.display.display();
    }
}