//Menu object
class Menu{
  public:
  Menu();
  Menu(uint16_t,uint16_t,uint16_t,uint16_t);
  Menu(unsigned short int, unsigned short int, unsigned short int, unsigned short int, String, vector<String>, vector<unsigned short int *>, unsigned char, bool);
  Menu(unsigned short int, unsigned short int, unsigned short int, unsigned short int, String, vector<String>, unsigned char, bool);  
  void displayMenu();
  void displayMenu(bool,bool);
  void displayMenu(bool, bool, bool);
  void displayMainMenu();
  void displayFilesMenu(int16_t, bool, uint8_t, uint8_t);
  void displayFilesMenu(int16_t, bool, uint8_t, uint8_t,vector<String>);
  void displayTrackMenu();
  void displayTrackMenu_trackEdit(uint8_t);
  void displayEditMenu(uint8_t*, uint8_t);
  void displayEditMenu();
  void displaySettingsMenu(uint8_t,uint8_t,uint8_t);
  void displaySwingMenu();
  void displayEchoMenu();
  void displayRecMenu(uint8_t,uint8_t,uint8_t);
  void displayFxMenu();
  void displayHumanizeMenu();
  void displayArpMenu();
  void displayClockMenu(float);
  bool moveMenuCursor(bool);
  vector<String> options;
  // vector<unsigned short int *> data;
  // uint8_t listStart;
  // uint8_t listEnd;
  uint8_t highlight;
  unsigned short int topL[2];
  unsigned short int bottomR[2];
  CoordinatePair coords;
  bool isActive;
  String menuTitle;
  int8_t page;
};

Menu::Menu(){
  options = {"test","test","test"};
  isActive = false;
  highlight = 0;
  topL [0] = 0;
  topL [1] = 0;
  bottomR [0] = screenWidth;
  bottomR [1] = screenHeight;
  menuTitle = "test";
  page = 0;
  coords.x1 = 0;
  coords.y1 = 0;
  coords.x2 = 0;
  coords.y2 = 0;
}

Menu::Menu(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
  topL[0] = x1;
  topL[1] = y1;
  bottomR[0] = x2;
  bottomR[1] = y2;
  options = {};
  menuTitle = "MENU";
  page = 0;
  coords.x1 = x1;
  coords.y1 = y1;
  coords.x2 = x2;
  coords.y2 = y1;
}

Menu::Menu(unsigned short int x1, unsigned short int y1, unsigned short int x2, unsigned short int y2, String title, vector<String> ops, unsigned char s, bool f){
  topL [0] = x1;
  topL [1] = y1;
  bottomR [0] = x2;
  bottomR [1] = y2;

  coords.x1 = x1;
  coords.y1 = y1;
  coords.x2 = x2;
  coords.y2 = y1;

  options = ops;
  isActive = false;
  highlight = 0;
  menuTitle = title;
  page = 0;
}

Menu activeMenu;

void Menu::displayMenu(bool bounded, bool underlined, bool separateWindow){
  if(menuTitle == "CLOCK")
    activeMenu.displayClockMenu(1);
  else if(menuTitle == "SWING")
    activeMenu.displaySwingMenu();
  else if(menuTitle == "TRK")
    activeMenu.displayTrackMenu();
  else if(menuTitle == "EDIT"){
    activeMenu.displayEditMenu();
  }
  else if(menuTitle == "FX"){
    activeMenu.displayFxMenu();
  }
  else{
    unsigned short int menuHeight = abs(bottomR[1]-topL[1]);
    if(separateWindow)
      display.clearDisplay();
    //drawing menu box (+16 so the title is transparent)
    display.fillRect(topL[0],topL[1]+13, bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
    display.setCursor(topL[0],topL[1]+3);

    //this is so the "MENU" sign slides out from the right
    if(menuTitle == "MENU"){
      display.setCursor(topL[0]+topL[1]-2,5);
      display.fillRect(trackDisplay,0,screenWidth-trackDisplay,debugHeight,SSD1306_BLACK);
    }
    display.setFont(&FreeSerifItalic9pt7b);
    display.print(menuTitle);
    display.setFont();

    if(bounded)
      display.drawRect(topL[0],topL[1]+12, bottomR[0]-topL[0], bottomR[1]-topL[1]-12, SSD1306_WHITE);
    else if(underlined)
      display.drawFastHLine(topL[0],topL[1]+12,bottomR[0]-topL[0],SSD1306_WHITE);
    //drawing menu options, and the highlight
    unsigned short int textWidth = 20;
    unsigned short int textHeight = menuHeight/5;

    unsigned short int menuStart;
    unsigned short int menuEnd;
    unsigned short int yLoc = 0;

    menuStart = 0;
    menuEnd = options.size()-1;
    if(highlight == 0){
      menuStart = highlight;
      menuEnd = highlight+3;
    }
    else if(highlight>0 && highlight<options.size()-2){
      menuStart = highlight-1;
      menuEnd = highlight+2;
    }
    else if(highlight >= options.size()-2){
      menuStart = options.size()-4;
      menuEnd = options.size()-1;
    }
    //making sure it can't read options that don't exist
    if(menuEnd>=options.size())
      menuEnd = options.size()-1;
    //printing out the menu
    for(int i = menuStart; i<=menuEnd; i++){
      display.setCursor(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2);
      if(highlight != i){
        //printing the special note icon
        if(menuTitle == "TRK" && options[i] == "ptch"){
          display.drawChar(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2,0x0E,SSD1306_WHITE, SSD1306_BLACK,1);
          display.setCursor(topL[0]+3+5,(yLoc+1)*textHeight+topL[1]+2);
        }
        else{
          display.print(options[i]);
        }
      }
      else if(highlight == i){
        //printing the special note icon
        if(menuTitle == "TRK" && options[i] == "ptch"){
          display.fillRect(topL[0]+2,(yLoc+1)*textHeight+topL[1]+1,7,9,SSD1306_WHITE);
          display.drawChar(topL[0]+3,(yLoc+1)*textHeight+topL[1]+2,0x0E,SSD1306_BLACK, SSD1306_WHITE,1);
          display.setCursor(topL[0]+3+6,(yLoc+1)*textHeight+topL[1]+2);
        }
        else{
          display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
          display.print(options[i]);
        }
        display.setTextColor(SSD1306_WHITE,SSD1306_BLACK);
      }
      yLoc++;
    }
  }
}
void Menu::displayMenu(bool bounded, bool underlined){
  displayMenu(bounded, underlined, false);
}
void Menu::displayMenu(){
  displayMenu(true,true,false);
}