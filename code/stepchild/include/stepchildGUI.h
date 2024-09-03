/*
    Functions that involve custom GUI layouts for user input/output
*/
void alert(String text, int time){
  unsigned short int len = text.length()*6;
  display.clearDisplay();
  display.setCursor(screenWidth/2-len/2, 29);
  display.print(text);
  display.display();
  delay(time);
}

String enterText(String title){
  return enterText(title, 10);
}
String enterText(String title, uint8_t count){
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
        if(alphabet[highlight] != "Enter" && text.length()<count){
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
      if(controls.NEW()){
        lastTime = millis();
        highlight = alphabet.size()-1;
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
      display.print(text+(text.length()<count?alphabet[highlight]:""));
      //cursor
      if(millis()%750>250){
        display.drawFastVLine(9+text.length()*6,14,9,SSD1306_WHITE);
      }
    }
    else
      display.print(text);

    //blank spaces
    for(uint8_t i = text.length(); i<count; i++){
      display.drawFastHLine(10+i*6,22,5,1);
    }
    

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
  return text;
}


void binarySelectionBoxPlaceholderDisplayFunction(){}

//this one has a title, but displays a blank screen
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title){
  return binarySelectionBox(x1,y1,op1,op2,title,binarySelectionBoxPlaceholderDisplayFunction);
}

//Binary Selection box w no title
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, void (*drawingFunction)()){
  return binarySelectionBox(x1,y1,op1,op2,"",drawingFunction);
}

//centered on x1 and y1
//returns -1 (no answer/exit), 0 (no) or 1 (yes)
//Binary Selection box w a title, returns -1 for 
int8_t binarySelectionBox(int8_t x1, int8_t y1, String op1, String op2, String title, void (*drawingFunction)()){
  //bool for breaking from the loop
  bool notChosenYet = true;
  //storing the state
  bool state = false;
  lastTime = millis();
  
  while(true){
    display.clearDisplay();
    drawingFunction();
    graphics.drawBinarySelectionBox(x1,y1,op1,op2,title,state);
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      //x to select option
      if(controls.joystickX != 0){
        if(controls.joystickX == 1 && state){
          state = !state;
          lastTime = millis();
        }
        else if(controls.joystickX == -1 && !state){
          state = !state;
          lastTime = millis();
        }
      }
      //menu/del to cancel
      if(controls.MENU() || controls.DELETE()){
        lastTime = millis();
        return -1;
      }
      //choose option
      else if(controls.NEW() || controls.SELECT() ){
        lastTime = millis();
          controls.setNEW(false);
        controls.setSELECT(false);
        return state;
      }
    }
  }
  return false;
}
//Specialized functions for drawing geometries
unsigned short int horzSelectionBox(String caption, vector<String> options, unsigned short int x1, unsigned short int y1, unsigned short int width, unsigned short int height){
  long int time = millis();
  unsigned short int select = 0;
  bool selected = false;
  while(!selected){
    display.fillRect(x1,y1,width,height,SSD1306_BLACK);
    display.drawRect(x1,y1,width,height,SSD1306_WHITE);
    display.setCursor(x1+20, y1+10);
    printSmall(x1+2,y1+2,caption,SSD1306_WHITE);
    for(int i = 0; i<options.size(); i++){
      display.setCursor(x1+40+i*20,y1+30);
      if(i == select)
        display.drawRect(x1+i*20-2,y1+8,8,options[i].length()*4+2,SSD1306_WHITE);
      printSmall(x1+i*20,y1+10,options[i],SSD1306_WHITE);
    }
    display.display();
    controls.readJoystick();
    controls.readButtons();
    if(utils.itsbeen(200)){
      if(controls.joystickX != 0 || controls.joystickY != 0){
        if(controls.joystickX == -1 && select<options.size()-1){
          select++;
          time = millis();
        }
        if(controls.joystickX == 1 && select>0){
          select--;
          time = millis();
        }
        if(select<0)
          select = 0;
        if(select>options.size()-1)
          select = options.size()-1;
      }
      if(controls.SELECT() ){
        selected = true;
        time = millis();
      }
    }
  }
  controls.setSELECT(false);
  lastTime = millis();
  display.invertDisplay(false);
  return select;
}
