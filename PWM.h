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

// const uint8_t CV1_Pin = 5;
const uint8_t CV1_Pin = 7;//just for V0.4 testing
// const uint8_t CV2_Pin = 6;
// const uint8_t CV3_Pin = 28;

void initPWM(){
#ifndef HEADLESS
  pinMode(CV1_Pin, OUTPUT);
//   pinMode(CV2_Pin, OUTPUT);
//   pinMode(CV3_Pin, OUTPUT);

  //frequency of the oscillation
  analogWriteFreq(1000000);
  //max value you can put into it (65535 @ 16bit res let's us do a 100% duty cycle)
  analogWriteRange(65535);
  //16 bit resolution
  analogWriteResolution(16);
#endif
}

//this converts a midi 8-bit pitch to a 12note/V CV voltage
float pitchToVoltage(uint8_t pitch){
    //the CV scale is 12 tones/volt
    //so the first thing we need to do is crop the pitch to be within C1-C4 since we only have ~3V ==> 3 octaves
    //actually, the max pitch we could write would be 3.3*12semitones above the lowest, so 39.6 semitones is our range
    //C3 is 48

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
//    analogWrite(CV1_Pin,dCycle);
}

void writeCVGate(bool on){
    uint16_t dCycle = on?65535:0;
//    analogWrite(CV2_Pin,dCycle);
}

String CVtoPitch(float v){
    uint8_t pitch = v*12;
    return pitchToString(pitch,true,true);
}

//Draws an aesthetic sinewave that gets shorter in period as pitch increases. Vel increases amplitude, and gate determines if it's flat or not;
void drawCVOsc(uint8_t pitch, uint8_t vel, bool gate){
    if(gate){
        float amp = float(vel)/127.0 * screenHeight/2;
        //0 should be a period of 64, and 127 should be a period of 3
        //map(pitch,0,127,64,3);
        float period = float(pitch)/127.0 * 61+3;
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
    while(true){
        if(noteOnReceived){
            noteOnReceived = false;
            gate = true;
            pitch = recentNote[0];
            writeCVGate(gate);
            writeCVPitch(pitch);
        }
        if(noteOffReceived){
            noteOffReceived = false;
            gate = false;
            pitch = 0;
            writeCVGate(gate);
            writeCVPitch(pitch);
        }
        joyRead();
        readButtons();
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis();
                break;
            }

        }
        if(x == -1){
            lastTime = millis();
            noteOnReceived = true;
            recentNote[0] = 64;
            recentNote[1] = 100;
        }
        if(x == 1){
            lastTime = millis();
            noteOnReceived = true;
            recentNote[0] = 96;
            recentNote[1] = 127;
        }
        if(x == 0 && gate){
            noteOffReceived = true;
        }

        display.clearDisplay();
        drawCVOsc(pitch,recentNote[1],gate);
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
    Serial.println("hello?");
    Serial.flush();
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
