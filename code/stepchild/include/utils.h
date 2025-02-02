//Stores one x,y pair
struct Coordinate{
    int16_t x;
    int16_t y;
    Coordinate(){
        this->x = 0;
        this->y = 0;
    }
    Coordinate(int16_t x1, int16_t y1){
        this->x = x1;
        this->y = y1;
    }
};

struct CoordinatePair{
    Coordinate start;
    Coordinate end;
    CoordinatePair(){}
    CoordinatePair(uint16_t xStart, uint16_t xEnd){
        this->start = Coordinate(xStart,0);
        this->end = Coordinate(xEnd,0);
    }
    CoordinatePair(uint16_t xStart, uint8_t yStart, uint16_t xEnd, uint8_t yEnd){
        this->start = Coordinate(xStart,yStart);
        this->end = Coordinate(xEnd,yEnd);
    }

    bool isVertical(){
        return (this->start.x) == (this->end.x);
    }
};

unsigned long lastTime;

class StepchildUtilities{
    public:
    Coordinate getRadian(uint8_t h, uint8_t k, int a, int b, float angle) {
        float x1 = h + a * cos(radians(angle));
        float y1;
        if (angle > 180) {
            y1 = k - b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        }
        else {
            y1 = k + b * sqrt(1 - pow((x1 - h), 2) / pow(a, 2));
        }
        return Coordinate(int16_t(x1),int8_t(y1));
    }
    bool itsbeen(unsigned long time){
        return ((millis()-lastTime)>=time);
    }
};

StepchildUtilities utils;

uint16_t changeSubDiv(bool direction, uint8_t subDiv, bool allowZero){
  //down
  if(!direction){
    if(subDiv == 1 && allowZero)
      subDiv = 0;
    else if(subDiv>3)
      subDiv /= 2;
    else if(subDiv == 3)
      subDiv = 1;
  }
  else{
    if(subDiv == 0)
      subDiv = 1;
    else if(subDiv == 1)//if it's one, set it to 3
      subDiv = 3;
    else if(subDiv !=  96 && subDiv != 32){
      //if triplet mode
      if(!(subDiv%2))
        subDiv *= 2;
      else if(!(subDiv%3))
        subDiv *=2;
    }
  }
  return subDiv;
}

uint16_t toggleTriplets(uint16_t subDiv){
  //this breaks the pattern, but lets you swap from 2/1 to 3/1 (rare case probs)
  if(subDiv == 192){
    subDiv = 32;
  }
  else if(!(subDiv%3)){//if it's in 1/4 mode...
    subDiv = 2*subDiv/3;//set it to triplet mode
  }
  else if(!(subDiv%2)){//if it was in triplet mode...
    subDiv = 3*subDiv/2;//set it to 1/4 mode
  }
  return subDiv;
}

//Formats a number 1-13 into its Roman Numeral equivalent
//This is really only used for printing intervals in Roman Numeral notation
//eg. printing the 3rd degree of a scale as "III"
String decimalToNumeral(int dec){
  String numeral = "";
  if(dec < 4){
    for(int i = 0; i<dec; i++){
      numeral+="I";
    }
  }
  if(dec == 4)
    numeral = "IV";
  if(dec>4 && dec<9){
    numeral = "V";
    for(int i = 5; i<dec; i++){
      numeral += "I";
    }
  }
  if(dec == 9)
    numeral = "IX";
  if(dec>9 && dec<13){
    numeral = "X";
    for(int i = 10; i<dec; i++){
      numeral += "I";
    }
  }
  return numeral;
}

//returns a byteCount as a more recognizeable value, like 7kB or 5mB
String bytesToString(uint32_t bytes){
  //if it's less than 500, print bytes as-is
  if(bytes<500){
    return stringify(bytes)+"B";
  }
  //probably never going to go over this
  else if(bytes<500000){
    //divide by 100 so the decimal value is now in the 1's place
    bytes/= 100;
    String byteCount = stringify(bytes);
    byteCount += byteCount[byteCount.length()-1];
    byteCount[byteCount.length()-2] = '.';
    return byteCount+"kB";
  }
  else{
    return "bruh";
  }
}

//helper function for writing bytes to the serial buffer
void writeBytesToSerial(uint8_t* byteArray, uint16_t numberOfBytes){
#ifndef HEADLESS
  Serial.write(byteArray,numberOfBytes);
#endif
}

//returns the pitch in english AND in either sharp, or flat form
String pitchToString(uint8_t input, bool oct, bool sharps){
  String pitch;
  int octave = (input/12)-2;//idk why this offset is needed
  input = input%12;
  if(!sharps){
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "Db";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "Eb";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "Gb";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "Ab";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "Bb";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  else{
    switch(input){
      case 0:
        pitch = "C";
        break;
      case 1:
        pitch = "C#";
        break;
      case 2:
        pitch = "D";
        break;
      case 3:
        pitch = "D#";
        break;
      case 4:
        pitch = "E";
        break;
      case 5:
        pitch = "F";
        break;
      case 6:
        pitch = "F#";
        break;
      case 7:
        pitch = "G";
        break;
      case 8:
        pitch = "G#";
        break;
      case 9:
        pitch = "A";
        break;
      case 10:
        pitch = "A#";
        break;
      case 11:
        pitch = "B";
        break;
    }
  }
  if(oct){
    pitch+=stringify(octave);
  }
  return pitch;
}

String stepsToPosition(int steps,bool verby){
  String text;
  text += stringify(steps/96 + 1);
  text += stringify(":");
  text += stringify((steps%96)/24 + 1);
  return text;
}

String stepsToMeasures(int32_t stepVal){
  // //Serial.println("steps:"+stringify(stepVal));
  String text;
  uint16_t smallest = 0;
  uint16_t steps = stepVal;

  //% steps by each denominator to see if there's no remainder
  //if there's no remainder, then you know that the smallest
  uint16_t denominators[12] = {96,48,32,24,16,12,8,6,4,3,2,1};
  for(uint8_t i = 0; i<12; i++){
    if(smallest == 0){
      steps=stepVal%denominators[i];
      if(steps == 0){
        smallest = i;
        break;
      }
    }
  }
  //steps are each a 1/96
  //this is basically asking "if steps is cleanly divisible by X, with no remainder, then X is the smallest denominator needed"
  //reset steps to the length - wholenotes
  steps = stepVal;
  if(steps>=96){
    text = stringify(steps/96);
    steps %= 96;
  }
  //if there's no numerator
  if(steps/denominators[smallest] == 0){
    if(text == "")
      text = "0";
    return text;
  }
  if(text != "")
    text += " ";
  text += stringify(steps/denominators[smallest])+"/"+stringify(denominators[11-smallest]);
  return text;
}

int16_t getOctave(int val){
  return val/12;
}


//returns the number of digits (eg. 100 = 3, 1 = 1) in a byte-sized (8bit) number
uint8_t countDigits_byte(uint8_t number){
  uint8_t count = 1;//it's ~always~ at least 1 digit
  while(number>=10){
    number/=10;
    count++;
  }
  return count;
}

uint8_t countSpaces(String text){
  uint8_t count = 0;
  for(uint8_t i = 0; i<text.length(); i++){
    if(text.charAt(i) == ' '){
      count++;
    }
  }
  return count;
}

uint8_t countChar(String text,unsigned char c){
  uint8_t count = 0;
  for(uint8_t i = 0; i<text.length(); i++){
    if(text.charAt(i) == c){
      count++;
    }
  }
  return count;
}

uint8_t getSmallTextLength(String t){
  return t.length()*4-countChar(t,' ')*2;
}

//Convert a list of bytes into corresponding C-B pitches
String convertVectorToPitches(vector<uint8_t> intervals){
  String text;
  for(uint8_t i = 0; i<intervals.size(); i++){
    text += pitchToString(intervals[i],false,true);
    if(i != intervals.size()-1)
      text += ",";
  }
  return text;
}

String getInterval(int root, int pitch) {
  int interval = (pitch - root) % 12;
  switch (interval) {
    case 0://root (unison)
      return "I";
      break;
    case 1://minor 2nd
      return "ii";
      break;
    case 2://major 2nd
      return "II";
      break;
    case 3://minor 3rd
      return "iii";
      break;
    case 4://major 3rd
      return "III";
      break;
    case 5://perfect 4th
      return "IV";
      break;
    case 6://diminished fifth
      return "dV";
      break;
    case 7://p 5th
      return "V";
      break;
    case 8://minor 6th
      return "vi  ";
      break;
    case 9://major 6th
      return "VI";
      break;
    case 10://minor 7th
      return "vii";
      break;
    case 11://major 7th
      return "VII";
      break;
  }
  return "";
}


void filterOutUnisonNotes(vector<uint8_t>& notes){
  vector<uint8_t> uniqueNotes;
  for(uint8_t i = 0; i<notes.size(); i++){
    if(!uniqueNotes.size())
      uniqueNotes.push_back(notes[i]%12);
    else{
      if(find(uniqueNotes.begin(),uniqueNotes.end(),notes[i]%12) == uniqueNotes.end()){
        uniqueNotes.push_back(notes[i]%12);
      }
    }
  }
}

uint8_t getLowestVal(vector<uint8_t> vec){
  uint8_t lowest = 255;
  for(uint8_t i = 0; i<vec.size(); i++){
    if(vec[i]<lowest)
      lowest = vec[i];
  }
  return lowest;
}

bool isInVector(int val, vector<uint8_t> vec){
  if(vec.size() == 0) 
    return false;
  for(int i = 0; i<vec.size(); i++){
    if(val == vec[i]){
      return true;
    }
  }
  return false;
}


void hardReset(){
  rp2040.reboot();
}

//update mode
void enterBootsel(){
  display.clearDisplay();
  display.drawBitmap(0,0,childOS,128,64,SSD1306_WHITE);
  display.display();
  reset_usb_boot(1<<PICO_DEFAULT_LED_PIN,0);
}

bool isConnectedToUSBPower(){
  return digitalRead(USB_PIN);
}

#define BATTSCALE 0.00966796875
//3.0*3.3/1024.0;
//idk why ^^this isn't 4095.0, but it ain't

float getBattLevel(){
  //So when USB is in, VSYS is ~5.0
  //When all 3AA's are in, if they're 1.5v batts VSYS is ~4.5
  //But if they're 1.2v batts VSYS is ~3.6;
  float val = float(analogRead(VOLTAGE_PIN))*BATTSCALE;
  return val;
}

void maxCurrentDrawTest(){
  controls.writeLEDs(0b1111111111111111);
  display.fillRect(0,0,128,64,1);
  display.display();
  while(true){
  }
}

//pulses the onboard LED
void ledPulse(uint8_t speed){
  //use abs() so that it counts DOWN when millis() overflows into the negatives
  //Multiply by 4 so that it's 'saturated' for a while --> goes on, waits, then pulses
  analogWrite(ONBOARD_LED,4*abs(int8_t(millis()/speed)));
}

void testButton(uint8_t bit){
  do{
    controls.readMainButtons();
  }
  while(!(controls.mainButtonState&(1<<bit)));
}

/*
  Test routine that prompts the user to use each input, one by one, on the Stepchild to see if everything is working correctly
*/
void testAllInputs(){
  Serial.println("--- Input Test! ---");
  Serial.print("Press 'New': ");
  Serial.flush();
  testButton(NEW_BUTTON);
  Serial.println("NEW!");
  Serial.print("Press 'Shift': ");
  Serial.flush();
  testButton(SHIFT_BUTTON);
  Serial.println("SHIFT!");
  Serial.print("Press 'Select': ");
  Serial.flush();
  testButton(SELECT_BUTTON);
  Serial.println("SELECT!");
  Serial.print("Press 'Delete': ");
  Serial.flush();
  testButton(DELETE_BUTTON);
  Serial.println("DELETE!");
  Serial.print("Press 'Loop': ");
  Serial.flush();
  testButton(LOOP_BUTTON);
  Serial.println("LOOP!");
  Serial.print("Press 'Play': ");
  Serial.flush();
  testButton(PLAY_BUTTON);
  Serial.println("PLAY!");
  Serial.print("Press 'Copy': ");
  Serial.flush();
  testButton(COPY_BUTTON);
  Serial.println("COPY!");
  Serial.print("Press 'Menu': ");
  Serial.flush();
  testButton(MENU_BUTTON);
  Serial.println("Menu!");
  Serial.print("Press 'A': ");
  Serial.flush();
  testButton(A_BUTTON);
  Serial.println("A!");
  Serial.print("Press 'B': ");
  Serial.flush();
  testButton(B_BUTTON);
  Serial.println("B!");

  //Encoder Dials
  controls.clearButtons();
  Serial.print("Roll A: ");
  while(!controls.counterA){};
  Serial.println(controls.counterA>0?"UP!":"DOWN!");
  controls.clearButtons();
  Serial.print("Roll B: ");
  while(!controls.counterB){};
  Serial.println(controls.counterB>0?"UP!":"DOWN!");

  //Joystick
}

void debugPrintButtons(){
  Serial.println("-- main buttons --");
  for(uint8_t i = 0; i<8; i++){
    Serial.print((controls.mainButtonState>>i)&1);
  }
  Serial.println("X:"+stringify(controls.joystickX)+" ("+stringify(analogRead(JOYSTICK_X))+")");
  Serial.println("Y:"+stringify(controls.joystickY)+" ("+stringify(analogRead(JOYSTICK_Y))+")");
  Serial.flush();
}

