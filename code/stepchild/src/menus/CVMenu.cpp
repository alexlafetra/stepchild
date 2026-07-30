#include "Stepchild.h"


;

using namespace std;

String CVtoPitchString(float v){
    uint8_t pitch = v*12;
    return stepchild.pitchToString(pitch,true,true);
}

//Draws an aesthetic sinewave that gets shorter in period as pitch increases. Vel increases amplitude, and gate determines if it's flat or not;
void drawCVOsc(int8_t pitch, uint8_t vel, bool gate){
    if(gate){
        float amp = float(vel)/127.0 * stepchild.SCREEN_HEIGHT/4;
        //0 should be a period of 64, and 127 should be a period of 3
        //map(pitch,0,127,64,3);
        float period = float(pitch)/127.0 * 100.0+3;
        int8_t lastVal = 0;
        float offset = millis()/10;
        for(uint8_t i = 0; i<stepchild.SCREEN_WIDTH; i++){
            int8_t val = amp*sin(2*PI*i/period+offset);
            stepchild.display.drawLine(i==0?0:i-1,lastVal+32,i,val+32,1);
            lastVal = val;
        }
    }
    else{
        stepchild.display.drawFastHLine(0,32,128,1);
    }
}

void testCVPitches(){
    stepchild.lastTime = millis();
    uint16_t pitch = 0;
//    bool gate = false;
    while(true){
        // stepchild.display.clearDisplay();
        // graphics.printSmall_centered(64,32,stringify(pitch),1);
        // graphics.printSmall_centered(64,38,stringify(stepchild.cv.pitchToVoltage(pitch))+"V",1);
        // graphics.printSmall_centered(64,26,stepchild.pitchToString(pitch,true,true),1);
        // stepchild.display.display();
        ledPulse(16);
        stepchild.buttons.readButtons();
        if(stepchild.itsbeen(200)){
            stepchild.lastTime = millis();
            pitch++;
            pitch%=3200;
            Serial.println("P: "+stringify(pitch));
            Serial.println("V: "+stringify(stepchild.cv.pitchToVoltage(pitch))+"v");
            analogWrite(CV1_PIN,pitch);
            analogWrite(CV2_PIN,pitch);
            analogWrite(CV3_PIN,pitch);
        }
    }
}

//small app that can translate MIDI input to CV output
//loop runs and checks for MIDI notes that have been received
//when it finds one, it reproduces it using the CV PWM
void MIDItoCV(){
    uint8_t pitch = 0;
    bool gate = false;
    //0 is global, anything else is a filter
    int8_t channel = 0;
    testCVPitches();
    while(true){
        if(stepchild.recentNote.vel && (channel == 0 || channel == stepchild.recentNote.channel)){
            stepchild.recentNote.vel = 0;
            gate = true;
            pitch = stepchild.recentNote.pitch;
            stepchild.cv.writeGate(gate);
            stepchild.cv.checkPitch(pitch);
        }
        if(!stepchild.recentNote.vel && stepchild.recentNote.pitch == pitch && (channel == 0 || channel == stepchild.recentNote.channel)){
            gate = false;
            pitch = stepchild.recentNote.pitch;
            stepchild.cv.writeGate(gate);
            stepchild.cv.checkPitch(pitch);
        }
        stepchild.buttons.readJoystick();
        stepchild.buttons.readButtons();
        if(stepchild.itsbeen(200)){
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                break;
            }
        }
        while(stepchild.buttons.counterA != 0){
            if(stepchild.buttons.counterA<0 && channel>0){
                channel--;
            }
            else if(stepchild.buttons.counterA>0 && channel<16){
                channel++;
            }
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
        }
        while(stepchild.buttons.counterB != 0){
            if(stepchild.buttons.counterB<0 && channel>0){
                channel--;
            }
            else if(stepchild.buttons.counterB>0 && channel<16){
                channel++;
            }
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
        }
        stepchild.display.clearDisplay();
        graphics.printSmall(100,0,"ch:"+(channel?stringify(channel):"all"),1);
        drawCVOsc(pitch,stepchild.recentNote.vel,gate);
        graphics.printSmall(0,0,"$: "+stringify(pitch)+" ("+stepchild.pitchToString(pitch,true,true)+")",1);
        graphics.printSmall(0,57,"CV: ",1);
        graphics.printSmall(30,57,stringify(stepchild.cv.pitchToVoltage(pitch))+"V",1);
        if(gate){
            stepchild.display.fillRect(13,55,15,9,1);
            graphics.printSmall(17,57,"on",0);
        }
        else{
            stepchild.display.drawRect(13,55,15,9,1);
            graphics.printSmall(15,57,"off",1);
        }
        stepchild.display.display();
    }
}
void CVEncoders(uint8_t encoderVal, uint8_t cursor){
    switch(cursor){
        //turns CV on/off
        case 0:
            stepchild.cv.on = !stepchild.cv.on;
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    }
}

void CVMenu(){
    uint8_t cursor = 0;
    while(true){
        stepchild.buttons.readJoystick();
        stepchild.buttons.readButtons();
        while(stepchild.buttons.counterA != 0){
            CVEncoders(stepchild.buttons.counterA,cursor);
            stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
        }
        while(stepchild.buttons.counterB != 0){
            CVEncoders(stepchild.buttons.counterB,cursor);
            stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;
        }
        if(stepchild.itsbeen(200)){
            if(stepchild.buttons.MENU()){
                stepchild.lastTime = millis();
                stepchild.buttons.setMENU(false) ;
                break;
            }
            if(stepchild.buttons.PLAY()){
                stepchild.lastTime = millis();
                stepchild.togglePlay();
            }
        }
        stepchild.display.clearDisplay();
        graphics.printSmall(0,1,"CV is ",1);
        graphics.drawLabel(28,1,stepchild.cv.on?"on":"off",true);
        // drawCVOsc(stepchild.cv.currentPitch,127,stepchild.cv.gateState);
        // if(stepchild.cv.currentPitch == -1){
        //     graphics.printSmall(0,57,"CV: 0V",1);
        //     graphics.printSmall(0,50,"$0",1);
        // }
        // else{
            // float p = stepchild.cv.pitchToVoltage(stepchild.cv.currentPitch);
            // graphics.printSmall(0,57,"CV: "+stringify(p)+"V"+" ("+CVtoPitchString(p)+")",1);
            // graphics.printSmall(0,50,"$"+stepchild.pitchToString(stepchild.cv.currentPitch,true,true),1);
        // }
        graphics.printSmall(0,43,"gate:",1);
        // if(stepchild.cv.gateState)
        //     stepchild.display.fillRect(18,43,5,5,1);
        // else
        //     stepchild.display.drawRect(18,43,5,5,1);
        stepchild.display.display();
    }
}
