//Menu object
class Menu{
  public:
    //String for storing the menu title
    String menuTitle;

    //these are the coordinates that define the rectangle of a menu
    //some menus don't really use this! especially if it's fullscreen
    CoordinatePair coords;

    //this is the menu "cursor"
    //useful for highlighting and selecting items
    uint8_t highlight;

    //the page variable is basically a spare byte that you can set
    //to toggle different menu states, or "pages," within one menu object
    int8_t page;

    Menu();
    Menu(int16_t,int16_t,int16_t,int16_t);
    Menu(int16_t x1, int16_t y1, int16_t x2, int16_t y2, String title);
    void displayMenu();
    void displayMenu(bool,bool);
    void displayMenu(bool, bool, bool);
    void displayMainMenu();
    void displayFileMenu();
    void displayFileMenu(int16_t, bool, uint8_t, uint8_t,vector<String>);
    void displayTrackMenu();
    void displayTrackMenu_trackEdit(uint8_t);
    void displayEditMenu(uint8_t*, uint8_t, uint8_t);
    void displayEditMenu();
    void displaySettingsMenu(uint8_t,uint8_t,uint8_t,uint8_t);
    void displayRecMenu(uint8_t,uint8_t,uint8_t);
    void displayFxMenu();
    void displayHumanizeMenu();
    void displayArpMenu();
    void displayClockMenu(float,uint8_t);
};

Menu::Menu(){
  highlight = 0;
  menuTitle = "test";
  page = 0;
  coords.start.x = 0;
  coords.start.y = 0;
  coords.end.x = screenWidth;
  coords.end.y = screenHeight;
}

Menu::Menu(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
  highlight = 0;
  menuTitle = "MENU";
  page = 0;
  coords.start.x = x1;
  coords.start.y = y1;
  coords.end.x = x2;
  coords.end.y = y2;
}

Menu::Menu(int16_t x1, int16_t y1, int16_t x2, int16_t y2, String title){
  coords.start.x = x1;
  coords.start.y = y1;
  coords.end.x = x2;
  coords.end.y = y2;
  highlight = 0;
  menuTitle = title;
  page = 0;
}

Menu activeMenu;

void Menu::displayMenu(bool bounded, bool underlined, bool separateWindow){
  if(menuTitle == "CLOCK")
    activeMenu.displayClockMenu(1,0);
  else if(menuTitle == "TRK")
    activeMenu.displayTrackMenu();
  else if(menuTitle == "EDIT")
    activeMenu.displayEditMenu();
  else if(menuTitle == "FX")
    activeMenu.displayFxMenu();
  else if(menuTitle == "TRK")
      activeMenu.displayMenu(true,false,false);
  else if(menuTitle == "EDIT")
      activeMenu.displayMenu(false, true, false);
  else if(menuTitle == "FILES")
      activeMenu.displayFileMenu();
  else if(menuTitle == "MENU")
    activeMenu.displayMainMenu();
}

void Menu::displayMenu(bool bounded, bool underlined){
  displayMenu(bounded, underlined, false);
}
void Menu::displayMenu(){
  displayMenu(true,true,false);
}

#define DEBUG_MENU 0
#define MAIN_MENU 1
#define CLOCK_MENU 2
#define SETTINGS_MENU 3
#define SEQUENCE_MENU 4
#define LOOP_MENU 5
#define MIDI_MENU 6
#define FRAGMENT_MENU 7
#define CURVE_MENU 8
#define ARP_MENU 9
#define FX_MENU 10
#define EDIT_MENU 11
#define TRACK_MENU 12
#define QUANTIZE_MENU 13
#define HUMANIZE_MENU 14
#define SAVE_MENU 15
#define FILE_MENU 16
#define REC_MENU 17

#define CURVE 8
#define FRAGMENT 7
#define SAVE 15
#define FILES 16
#define REC 17
#define ARP 9
#define FX 10
#define EDIT 11
#define TRK 12
#define QZ 13
#define HUMANIZE 14

void constructMenu(uint8_t id,uint8_t highlight){
  // controls.resetEncoders();
  switch(id){
    case(MAIN_MENU):
    {
      Menu mainMenu(25,1,93,64,"MENU");
      mainMenu.highlight = highlight;
      activeMenu = mainMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    case DEBUG_MENU:
    {
      Menu debugMenu(32,4,128,64,"DEBUG");
      debugMenu.highlight = highlight;
      activeMenu = debugMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    //This one needs a slide in animation
    case CLOCK_MENU:
    {
      Menu clckMenu(0,0,35,64,"CLOCK");
      clckMenu.highlight = highlight;
      activeMenu = clckMenu;
      if(menuIsActive){
        slideMenuIn(0,20);
      }
      clockMenu();
      return;
    }
    case SETTINGS_MENU:
    {
      Menu settingMenu(0,2,128,64,"SETTINGS");
      activeMenu = settingMenu;
      settingsMenu();
      return;
    }
    case LOOP_MENU:
    {
      Menu loopMenu(2,4,80,60,"LOOP");
      activeMenu = loopMenu;
      return;
      // cassetteAnimation();
    }
    case MIDI_MENU:
    {
      Menu midiMenu(2,4,80,60,"MIDI");
      activeMenu = midiMenu;
      return;
    }
    case FRAGMENT:
    {
      // fragmentMenu();
      return;
    }
    case ARP:
    {
      Menu arpMenu(0,0,128,64,"ARP");
      activeMenu = arpMenu;
      return;
    }
    case FX:
    {
      Menu efexMenu(25,1,93,64,"FX");
      activeMenu = efexMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    case EDIT_MENU:
    {
      Menu editingMenu(trackDisplay-5,0,screenWidth,headerHeight,"EDIT");
      activeMenu = editingMenu;
      if(menuIsActive){
        slideMenuIn(1,48);
      }
      editMenu();
      return;
    }
    case TRACK_MENU:
    {
      Menu trkMenu(94,0,129,65,"TRK");
      activeMenu = trkMenu;
      if(menuIsActive){
        slideMenuIn(1,10);
      }
      trackMenu();
      // slideMenuOut(1,10);
      return;
    }
    case FILES:
    {
      Menu filesMenu(7,3,128,64,"FILES");
      activeMenu = filesMenu;
      activeMenu.highlight = 0;
      // slideMenuIn(1,30);
      fileMenu();
      return;
    }
  }
}
//these gotta go here so the menu display can talk to them
//doing it this way is a little better i think? so all the menu dat doesn't need to be global and only loads when you need it
void constructMenu(String title,uint8_t highlight){
  if(title == "MENU"){
    constructMenu(MAIN_MENU,highlight);
  }
  else if(title == "DEBUG"){
    constructMenu(DEBUG_MENU,highlight);
  }
  else if(title == "CLOCK"){
    constructMenu(CLOCK_MENU,highlight);
  }
  else if(title == "SETTINGS"){
    constructMenu(SETTINGS_MENU,highlight);
  }
  else if(title == "SEQ"){
    constructMenu(SEQUENCE_MENU,highlight);
  }
  else if(title == "LOOP"){
    constructMenu(LOOP_MENU,highlight);
  }
  else if(title == "MIDI"){
    constructMenu(MIDI_MENU,highlight);
  }
  else if(title == "FRAGMENT"){
    constructMenu(FRAGMENT,highlight);
  }
  else if(title == "CURVE"){
    constructMenu(CURVE,highlight);
  }
  else if(title == "ARP"){
    constructMenu(ARP,highlight);
  }
  else if(title == "FX"){
    constructMenu(FX,highlight);
  }
  //edit menu is called by note menu
  else if(title == "EDIT"){
    constructMenu(EDIT,highlight);
  }
  else if(title == "TRK"){
    constructMenu(TRK,highlight);
  }
  else if(title == "QZ"){
    constructMenu(QZ,highlight);
  }
  else if(title == "HUMANIZE"){
    constructMenu(HUMANIZE,highlight);
  }
  else if(title == "SAVE"){
    constructMenu(SAVE,highlight);
  }
  else if(title == "FILES"){
    constructMenu(FILES,highlight);
  }
  else if(title == "REC"){
    constructMenu(REC,highlight);
  }
}

void constructMenu(String title){
  constructMenu(title,0);
}
void constructMenu(uint8_t id){
  constructMenu(id,0);
}

