
String CVtoPitchString(float v){
    uint8_t pitch = v*12;
    return pitchToString(pitch,true,true);
}

//Draws an aesthetic sinewave that gets shorter in period as pitch increases. Vel increases amplitude, and gate determines if it's flat or not;
void drawCVOsc(int8_t pitch, uint8_t vel, bool gate){
    if(gate){
        float amp = float(vel)/127.0 * screenHeight/4;
        //0 should be a period of 64, and 127 should be a period of 3
        //map(pitch,0,127,64,3);
        float period = float(pitch)/127.0 * 100.0+3;
        int8_t lastVal = 0;
        float offset = millis()/10;
        for(uint8_t i = 0; i<screenWidth; i++){
            int8_t val = amp*sin(2*PI*i/period+offset);
            display.drawLine(i==0?0:i-1,lastVal+32,i,val+32,1);
            lastVal = val;
        }
    }
    else{
        display.drawFastHLine(0,32,128,1);
    }
}

void testCVPitches(){
    lastTime = millis();
    uint16_t pitch = 0;
//    bool gate = false;
    while(true){
        // display.clearDisplay();
        // printSmall_centered(64,32,stringify(pitch),1);
        // printSmall_centered(64,38,stringify(CV.pitchToVoltage(pitch))+"V",1);
        // printSmall_centered(64,26,pitchToString(pitch,true,true),1);
        // display.display();
        ledPulse(16);
        controls.readButtons();
        if(utils.itsbeen(200)){
            lastTime = millis();
            pitch++;
            pitch%=3200;
            Serial.println("P: "+stringify(pitch));
            Serial.println("V: "+stringify(CV.pitchToVoltage(pitch))+"v");
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
        if(noteOnReceived && (channel == 0 || channel == recentNote.channel)){
            noteOnReceived = false;
            gate = true;
            pitch = recentNote.pitch;
            CV.writeGate(gate);
            CV.checkPitch(pitch);
        }
        if(noteOffReceived && recentNote.pitch == pitch && (channel == 0 || channel == recentNote.channel)){
            noteOffReceived = false;
            gate = false;
            pitch = recentNote.pitch;
            CV.writeGate(gate);
            CV.checkPitch(pitch);
        }
        controls.readJoystick();
        controls.readButtons();
        if(utils.itsbeen(200)){
            if(controls.MENU()){
                lastTime = millis();
                break;
            }
        }
        while(controls.counterA != 0){
            if(controls.counterA<0 && channel>0){
                channel--;
            }
            else if(controls.counterA>0 && channel<16){
                channel++;
            }
            controls.counterA += controls.counterA<0?1:-1;
        }
        while(controls.counterB != 0){
            if(controls.counterB<0 && channel>0){
                channel--;
            }
            else if(controls.counterB>0 && channel<16){
                channel++;
            }
            controls.counterB += controls.counterB<0?1:-1;
        }
        display.clearDisplay();
        printSmall(100,0,"ch:"+(channel?stringify(channel):"all"),1);
        drawCVOsc(pitch,recentNote.vel,gate);
        printSmall(0,0,"$: "+stringify(pitch)+" ("+pitchToString(pitch,true,true)+")",1);
        printSmall(0,57,"CV: ",1);
        printSmall(30,57,stringify(CV.pitchToVoltage(pitch))+"V",1);
        if(gate){
            display.fillRect(13,55,15,9,1);
            printSmall(17,57,"on",0);
        }
        else{
            display.drawRect(13,55,15,9,1);
            printSmall(15,57,"off",1);
        }
        display.display();
    }
}
void CVEncoders(uint8_t encoderVal, uint8_t cursor){
    switch(cursor){
        //turns CV on/off
        case 0:
            CV.on = !CV.on;
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
        controls.readJoystick();
        controls.readButtons();
        while(controls.counterA != 0){
            CVEncoders(controls.counterA,cursor);
            controls.counterA += controls.counterA<0?1:-1;
        }
        while(controls.counterB != 0){
            CVEncoders(controls.counterB,cursor);
            controls.counterB += controls.counterB<0?1:-1;
        }
        if(utils.itsbeen(200)){
            if(controls.MENU()){
                lastTime = millis();
                controls.setMENU(false) ;
                break;
            }
            if(controls.PLAY()){
                lastTime = millis();
                sequence.togglePlay();
            }
        }
        display.clearDisplay();
        printSmall(0,1,"CV is ",1);
        graphics.drawLabel(28,1,CV.on?"on":"off",true);
        // drawCVOsc(CV.currentPitch,127,CV.gateState);
        // if(CV.currentPitch == -1){
        //     printSmall(0,57,"CV: 0V",1);
        //     printSmall(0,50,"$0",1);
        // }
        // else{
            // float p = CV.pitchToVoltage(CV.currentPitch);
            // printSmall(0,57,"CV: "+stringify(p)+"V"+" ("+CVtoPitchString(p)+")",1);
            // printSmall(0,50,"$"+pitchToString(CV.currentPitch,true,true),1);
        // }
        printSmall(0,43,"gate:",1);
        // if(CV.gateState)
        //     display.fillRect(18,43,5,5,1);
        // else
        //     display.drawRect(18,43,5,5,1);
        display.display();
    }
}
