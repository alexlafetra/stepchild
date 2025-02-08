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

//Resolution = 16 - log2(PWM_FREQ)
#define PWM_RESOLUTION 16 //16-bit res
#define PWM_MAX_VAL 65535 //16-bit max resolution
// #define PWM_FREQUENCY 8312500 //133MHz/16 so you can use all 16-bits of resolution
// #define PWM_FREQUENCY 1000 //default freq, see: https://arduino-pico.readthedocs.io/en/latest/analog.html
// #define PWM_FREQUENCY 1024000//6-bit resolution
// #define PWM_FREQUENCY 4000000
#define PWM_FREQUENCY 100000
// #define PWM_FREQUENCY 2000


/*
CV ports can grab data from:
a single track (gate)
global/specific channels (mono pitch -- only the most recent note will send though!)
an autotrack
the clock
*/

enum CV_TYPE:uint8_t{
    CV_PITCH,
    CV_GATE,
    CV_CLOCK
};

struct CV_JACK{
    CV_TYPE type = CV_PITCH;
    uint16_t currentVal = 0;
    uint8_t pin;
    CV_JACK(){};
    CV_JACK(uint8_t p, CV_TYPE t){
        pin = p;
        type = t;
    }
    void write(uint16_t v){
        //check value based on type of port, make sure you don't write something
        //crazy or dangerous
        switch(type){
            case CV_PITCH:
                break;
            case CV_GATE:
                break;
            case CV_CLOCK:
                break;
        }
        analogWrite(pin,v);
        currentVal = v;
    }
};

class StepchildCV{
    public:
    bool on = false;
    CV_JACK CV1;
    CV_JACK CV2;
    CV_JACK CV3;
    CV_JACK CV4;
    StepchildCV(){
    }
    void init(){
        #ifndef HEADLESS
        //Set CV pins to output
        pinMode(CV1_PIN, OUTPUT);
        pinMode(CV2_PIN, OUTPUT);
        pinMode(CV3_PIN, OUTPUT);
        pinMode(CV4_PIN, OUTPUT);
        //frequency of the PWM driver
        analogWriteFreq(PWM_FREQUENCY);
        //max value you can put into it (65535 @ 16bit res let's us do a 100% duty cycle)
        analogWriteRange(PWM_MAX_VAL);
        //16 bit resolution
        analogWriteResolution(PWM_RESOLUTION);
        CV1 = CV_JACK(CV1_PIN,CV_PITCH);
        CV2 = CV_JACK(CV2_PIN,CV_GATE);
        CV3 = CV_JACK(CV3_PIN,CV_CLOCK);
        CV3 = CV_JACK(CV4_PIN,CV_CLOCK);
        #endif
    }

    //this converts a midi 8-bit pitch to a 12note/V CV voltage
    float pitchToVoltage(int8_t pitch){
        //the CV scale is 12 tones/volt
        if(pitch == -1){
            return -1;
        }
        //each semitone is a 1/12 of a volt, so semitones*1/12 = voltage
        float voltage = float(pitch)/12.0;
        return voltage;
    }
    uint16_t getDutyCycleFromVoltage(float voltage){
        // float dutyCycle = voltage/3.3;
        // uint16_t twoByteNumber = dutyCycle*65535.0;
        // return twoByteNumber;
        return voltage/12.0 * float(PWM_MAX_VAL);
    }
    void writePitch(uint8_t pitch){
        float V = this->pitchToVoltage(pitch);
        uint16_t dCycle = this->getDutyCycleFromVoltage(V);
        analogWrite(CV1_PIN,dCycle);
    }
    void writeGate(bool on){
        uint16_t dCycle = on?65535:0;
        analogWrite(CV2_PIN,dCycle);
    }
    //writes a clock pulse
    void writeClock(){
    }
    void check(){
        //if the CV functionality is turned off, just return immediately
        if(!this->on){
            return;
        }
        bool gateState = false;
        uint8_t pitch = 0;
        //if a track is sending a pitch, 
        for(uint8_t i = 0; i<sequence.trackData.size(); i++){
            if(sequence.trackData[i].noteLastSent != 255){
                gateState = true;
                pitch = sequence.trackData[i].noteLastSent;
            }
        }
        //if the gate was off, and a pitch is on, write CVGate on
        this->writeGate(gateState);
        this->writePitch(pitch);
        this->writeClock();
    }
    void off(){            
        // CV1.write(0);
        // CV2.write(0);
        // CV3.write(0);
        digitalWrite(CV1_PIN,0);
        digitalWrite(CV2_PIN,0);
        digitalWrite(CV3_PIN,0);
    }
};

StepchildCV CV;

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
            CV.writePitch(pitch);
        }
        if(noteOffReceived && recentNote.pitch == pitch && (channel == 0 || channel == recentNote.channel)){
            noteOffReceived = false;
            gate = false;
            pitch = recentNote.pitch;
            CV.writeGate(gate);
            CV.writePitch(pitch);
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
                togglePlayMode();
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
