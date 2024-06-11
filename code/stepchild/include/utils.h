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
    CoordinatePair(uint16_t xStart, uint16_t xEnd, uint8_t yStart, uint8_t yEnd){
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
    //
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

int getOctave(int val){
  int oct = 0;
  while(val>11){
    oct++;
    val-=12;
  }
  return oct;
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

void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}


String enterText(String title){
  bool done = false;
  int highlight = 0;
  int rows = 5;
  int columns = 8;
  int textWidth = 12;
  int textHeight = 8;
  String text = "";
  vector<String> alphabet = {};
  vector<String> alpha1 = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  vector<String> alpha2 = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","_",".","!","0","1","2","3","4","5","6","7","8","9","Enter"};
  alphabet = alpha1;
  while(!done){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.SHIFT()){
        if(alphabet[0] == "a")
          alphabet = alpha2;
        else 
          alphabet = alpha1;
        lastTime = millis();
      }

      if(controls.DELETE() && text.length()>0){
        controls.setDELETE(false);
        String newString = text.substring(0,text.length()-1);
        text = newString;
        lastTime = millis();
      }
      if(controls.SELECT() ){
        //adding character to text
        if(alphabet[highlight] != "Enter" && text.length()<10){
          text+=alphabet[highlight];
        }
        //or quitting
        else if(alphabet[highlight] == "Enter")
          done = true;
        lastTime = millis();
      }
      if(controls.MENU()){
        text = "";
        done = true;
        lastTime = millis();
      }
    }
    if(utils.itsbeen(100)){
      if(controls.joystickX == -1 && highlight<alphabet.size()-1){
        highlight++;
        lastTime = millis();
      }
      if(controls.joystickX == 1 && highlight>0){
        highlight--;
        lastTime = millis();
      }
      if(controls.joystickY == -1 && highlight>=columns){
        highlight-=columns;
        lastTime = millis();
      }
      if(controls.joystickY == 1 && highlight<alphabet.size()-columns){
        highlight+=columns;
        lastTime = millis();
      }
    }
    //title
    display.clearDisplay();
    display.setCursor(5,6);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(title);
    display.setFont();

    //text tooltip
    printSmall(88,1,"[SHF] caps",SSD1306_WHITE);
    printSmall(88,8,"[MNU] exit",SSD1306_WHITE);

    //input text
    display.setCursor(10,15);
    if(alphabet[highlight] != "Enter"){
      display.print(text+alphabet[highlight]);
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,15,10,SSD1306_WHITE);
      }
    }
    else
      display.print(text);
    

    //drawing alphabet
    int count = 0;
    for(int i = 0; i<rows; i++){
      for(int j = 0; j<columns; j++){
        if(count<alphabet.size()){
          display.setCursor(j*textWidth+10, i*textHeight+24);
          if(count == highlight){
            //cursor highlight
            if(millis()%750>250){
              display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
              display.fillRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
              // display.print(alphabet[count]);
              // display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            // else{
              display.print(alphabet[count]);
              display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
              // display.drawRect(j*textWidth+9,i*textHeight+23,textWidth-5,textHeight+1,SSD1306_WHITE);
            // }
          }
          else{
            display.print(alphabet[count]);
          }
          count++;
        }
      }
    }

    display.display();
  }
  controls.clearButtons();
  lastTime = millis();
  if(text == ""){
    text = "default";
  }
  return text;
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