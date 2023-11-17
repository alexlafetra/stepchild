//this code enables the RP2040 to fake some analog output
//and send CV signals to 3 outputs

//basic idea is that the pico's PIO state machines can cycle from 3.3v+ to 0v
//so at a 50% duty cycle, you can have 1.65v+
//and at a 0% duty cycle, you can have 0v+

/*
analogWriteFreq(uint32_t frequency) to set the frequency of the PWM signal. It supports a frequency of 100Hz to 1MHz*

analogWriteRange(uint32_t range) to set the range of the PWM signal — the maximum value you’ll use to set 100% duty cycle. It supports a value from 16 to 65535.

analogWriteResolution (int resolution) to set the resolution of the PWM signal — up to 16-bit.

analogWrite(GPIO, duty cycle) to output a PWM signal to a specified pin with a defined duty cycle. It continuously outputs a PWM signal until a digitalWrite() or other digital output is performed.
*/

//pitch
const uint8_t CV1_Pin = 5;
//gate
const uint8_t CV2_Pin = 6;
//clock
const uint8_t CV3_Pin = 28;

//stores -1 if no pitch is being sent, or 0-127 for the current pitch
int8_t currentCVPitch = -1;
//on or off, set at the end of the "play all tracks" loop.
//set to on if currentCVPitch != 0
bool CVGate = false;
bool usingCV = true;

void checkCV(){
    //if the CV functionality is turned off, just return immediately
    if(!usingCV){
        CVGate = false;
        return;
    }
    CVGate = false;
    //if a track is sending a pitch, 
    for(uint8_t i = 0; i<trackData.size(); i++){
        if(trackData[i].noteLastSent != 255){
            CVGate = true;
            currentCVPitch = trackData[i].noteLastSent;
        }
    }
    //if the gate was off, and a pitch is on, write CVGate on
    writeCVGate(CVGate);
    writeCVPitch(currentCVPitch);
    writeCVClock();
}

void initPWM(){
#ifndef HEADLESS
  pinMode(CV1_Pin, OUTPUT);
  pinMode(CV2_Pin, OUTPUT);
  pinMode(CV3_Pin, OUTPUT);
  pinMode(onboard_ledPin, OUTPUT);

  //frequency of the oscillation
  analogWriteFreq(1000000);
  //max value you can put into it (65535 @ 16bit res let's us do a 100% duty cycle)
  analogWriteRange(65535);
  //16 bit resolution
  analogWriteResolution(16);
#endif
}

//this converts a midi 8-bit pitch to a 12note/V CV voltage
float pitchToVoltage(int8_t pitch){
    //the CV scale is 12 tones/volt
    //so the first thing we need to do is crop the pitch to be within C1-C4 since we only have ~3V ==> 3 octaves
    //actually, the max pitch we could write would be 3.3*12semitones above the lowest, so 39.6 semitones is our range
    //C3 is 48

    if(pitch == -1){
        return -1;
    }

    const uint8_t lowerBound = 24;
    const uint8_t upperBound = lowerBound+39;

    //C1 is 24
    while(pitch<lowerBound){
        pitch+=12;
    }
    //63 is 
    while(pitch>upperBound){
        pitch-=12;
    }

    //each semitone is a 1/12 of a volt, so semitones*1/12 = voltage
    float voltage = float(pitch)/12.0;
    return voltage;
}


uint16_t getDutyCycleFromVoltage(float voltage){
    float dutyCycle = voltage/3.3;
    uint16_t twoByteNumber = dutyCycle*65535;
    return twoByteNumber;
}

void writeCVPitch(uint8_t pitch){
    float V = pitchToVoltage(pitch);
    uint16_t dCycle = getDutyCycleFromVoltage(V);
   analogWrite(CV1_Pin,dCycle);
}

void writeCVGate(bool on){
    uint16_t dCycle = on?65535:0;
   analogWrite(CV2_Pin,dCycle);
}

//writes a clock pulse
void writeCVClock(){
    analogWrite(CV3_Pin,65535);
    analogWrite(CV3_Pin,0);
}

String CVtoPitch(float v){
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

//small app that can translate MIDI input to CV output
//loop runs and checks for MIDI notes that have been received
//when it finds one, it reproduces it using the CV PWM
void MIDItoCV(){
    uint8_t pitch = 0;
    bool gate = false;
    //0 is global, anything else is a filter
    int8_t channel = 0;
    while(true){
        if(noteOnReceived && (channel == 0 || channel == recentNote.channel)){
            noteOnReceived = false;
            gate = true;
            pitch = recentNote.pitch;
            writeCVGate(gate);
            writeCVPitch(pitch);
        }
        if(noteOffReceived && recentNote.pitch == pitch && (channel == 0 || channel == recentNote.channel)){
            noteOffReceived = false;
            gate = false;
            pitch = recentNote.pitch;
            writeCVGate(gate);
            writeCVPitch(pitch);
        }
        readJoystick();
        readButtons();
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis();
                break;
            }
        }
        while(counterA != 0){
            if(counterA<0 && channel>0){
                channel--;
            }
            else if(counterA>0 && channel<16){
                channel++;
            }
            counterA += counterA<0?1:-1;
        }
        while(counterB != 0){
            if(counterB<0 && channel>0){
                channel--;
            }
            else if(counterB>0 && channel<16){
                channel++;
            }
            counterB += counterB<0?1:-1;
        }
        display.clearDisplay();
        printSmall(100,0,"ch:"+(channel?stringify(channel):"all"),1);
        drawCVOsc(pitch,recentNote.vel,gate);
        printSmall(0,0,"$: "+stringify(pitch)+" ("+pitchToString(pitch,true,true)+")",1);
        printSmall(0,57,"CV: ",1);
        printSmall(30,57,stringify(pitchToVoltage(pitch))+"V",1);
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

void testCVPitches(){
    lastTime = millis();
    uint8_t pitch = 24;
    bool gate = false;
    while(true){
        if(itsbeen(1000)){
            lastTime = millis();
            writeCVPitch(pitch);
            // writeCVGate(gate);
            pitch++;
            if(pitch>63){
                pitch = 0;
            }
            gate = !gate;
        }
        float readVoltage = 0;
        const float sampleNumber = 6000.0;
        for(uint32_t i = 0; i<sampleNumber; i++){
            readVoltage+=float(analogRead(26))/sampleNumber;
        }
        //record analog val
        Serial.print("Variable_1:");
        Serial.print(readVoltage*3.3/1024);
        Serial.print(",");
        Serial.print("Variable_2:");
        Serial.println(readVoltage*3.3*12.0/1024.0);
    }
}

void CVEncoders(uint8_t encoderVal, uint8_t cursor){
    switch(cursor){
        //turns CV on/off
        case 0:
            usingCV = !usingCV;
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
        readJoystick();
        readButtons();
        while(counterA != 0){
            CVEncoders(counterA,cursor);
            counterA += counterA<0?1:-1;
        }
        while(counterB != 0){
            CVEncoders(counterB,cursor);
            counterB += counterB<0?1:-1;
        }
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis();
                menu_Press = false;
                break;
            }
            if(play){
                lastTime = millis();
                togglePlayMode();
            }
        }
        display.clearDisplay();
        printSmall(0,1,"CV is ",1);
        drawLabel(28,1,usingCV?"on":"off",true);
        drawCVOsc(currentCVPitch,127,CVGate);
        if(currentCVPitch == -1){
            printSmall(0,57,"CV: 0V",1);
            printSmall(0,50,"$0",1);
        }
        else{
            float p = pitchToVoltage(currentCVPitch);
            printSmall(0,57,"CV: "+stringify(p)+"V"+" ("+CVtoPitch(p)+")",1);
            printSmall(0,50,"$"+pitchToString(currentCVPitch,true,true),1);
        }
        printSmall(0,43,"gate:",1);
        if(CVGate)
            display.fillRect(18,43,5,5,1);
        else
            display.drawRect(18,43,5,5,1);
        display.display();
    }
}
