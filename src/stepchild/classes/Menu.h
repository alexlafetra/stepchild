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
  coords.x1 = 0;
  coords.y1 = 0;
  coords.x2 = screenWidth;
  coords.y2 = screenHeight;
}

Menu::Menu(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
  highlight = 0;
  menuTitle = "MENU";
  page = 0;
  coords.x1 = x1;
  coords.y1 = y1;
  coords.x2 = x2;
  coords.y2 = y2;
}

Menu::Menu(int16_t x1, int16_t y1, int16_t x2, int16_t y2, String title){
  coords.x1 = x1;
  coords.y1 = y1;
  coords.x2 = x2;
  coords.y2 = y2;
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

//slides a menu in from the top,right,bottom, or left
void slideMenuIn(int fromWhere, int speed){
  //sliding in from the right
  if(fromWhere == 1){
    //store original coords
    CoordinatePair originalCoords = activeMenu.coords;
    //then, offset the menu coordinates
    int8_t offset = screenWidth-activeMenu.coords.x1;
    activeMenu.coords.x1+=offset;
    activeMenu.coords.x2+=offset;
    //continuously move the menu coords and display it, until it reaches original position
    while(activeMenu.coords.x1>originalCoords.x1){
      activeMenu.coords.x1-=speed;
      activeMenu.coords.x2-=speed;
      if(activeMenu.coords.x1<originalCoords.x1){
        activeMenu.coords.x1=originalCoords.x1;
        activeMenu.coords.x2=originalCoords.x2;
      }
      displaySeq();
      // delay(20);
    }
    activeMenu.coords.x1 = originalCoords.x1;
    activeMenu.coords.x2 = originalCoords.x2;
  }
  //from the bottom
  else if(fromWhere == 0){
    //store original coords
    CoordinatePair originalCoords = activeMenu.coords;
    //then, offset the menu coordinates
    int8_t offset = screenHeight-activeMenu.coords.y1;
    activeMenu.coords.y1+=offset;
    activeMenu.coords.y2+=offset;
    //continuously move the menu coords and display it, until it reaches original position
    while(activeMenu.coords.y1>originalCoords.y1){
      activeMenu.coords.y1-=speed;
      activeMenu.coords.y2-=speed;
      if(activeMenu.coords.y1<originalCoords.y1){
        activeMenu.coords.y1=originalCoords.y1;
        activeMenu.coords.y2=originalCoords.y2;
      }
      displaySeq();
      // delay(20);
    }
    activeMenu.coords.y1 = originalCoords.y1;
    activeMenu.coords.y2 = originalCoords.y2;
  }
}

//same thang, but in reverse
void slideMenuOut(int toWhere, int speed){
  if(toWhere == 1){//sliding out to the left side
    while(activeMenu.coords.x1<screenWidth){
      activeMenu.coords.x1+=speed;
      activeMenu.coords.x2+=speed;
      //make sure x bounds don't glitch out/overflow  (don't think this is necessary, leaving it for legacy/in case you find a menu that bugs)
      //Using this makes some menus slide a lil' ugly
      // if(activeMenu.coords.x2>screenWidth){
      //   activeMenu.coords.x2 = screenWidth;
      // }
      displaySeq();
      drawPram(5,0);
    }
  }
  //to the bottom
  else if(toWhere == 0){
    while(activeMenu.coords.y1<screenHeight){
      activeMenu.coords.y1+=speed;
      activeMenu.coords.y2+=speed;
      //make sure y bounds don't glitch out
      if(activeMenu.coords.y2>screenHeight){
        activeMenu.coords.y2 = screenHeight;
      }
      displaySeq();
      drawPram(5,0);
      // delay(20);
    }
  }
}

#define DEBUG_MENU 0
#define MAIN_MENU 1
#define CLOCK_MENU 2
#define SETTINGS_MENU 3
#define SEQUENCE_MENU 4
#define LOOP_MENU 5
#define MIDI_MENU 6
#define FRAGMENT 7
#define CURVE 8
#define ARP 9
#define FX 10
#define EDIT 11
#define TRK 12
#define QZ 13
#define HUMANIZE 14
#define SAVE 15
#define FILES 16
#define REC 17

void constructMenu(uint8_t id){
  resetEncoders();
  switch(id){
    case(MAIN_MENU):
    {
      Menu debugMenu(25,1,93,64,"MENU");
      if(activeMenu.menuTitle == "FILES")
        debugMenu.highlight = 9;
      activeMenu = debugMenu;
      if(menuIsActive){
        slideMenuIn(0,30);
      }
      return;
    }
    case DEBUG_MENU:
    {
      Menu debugMenu(32,4,128,64,"DEBUG");
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
    case EDIT:
    {
      Menu editingMenu(trackDisplay-5,0,screenWidth,headerHeight,"EDIT");
      activeMenu = editingMenu;
      if(menuIsActive){
        slideMenuIn(1,48);
      }
      editMenu();
      return;
    }
    case TRK:
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
void constructMenu(String title){
  if(title == "MENU"){
    constructMenu(MAIN_MENU);
  }
  else if(title == "DEBUG"){
    constructMenu(DEBUG_MENU);
  }
  else if(title == "CLOCK"){
    constructMenu(CLOCK_MENU);
  }
  else if(title == "SETTINGS"){
    constructMenu(SETTINGS_MENU);
  }
  else if(title == "SEQ"){
    constructMenu(SEQUENCE_MENU);
  }
  else if(title == "LOOP"){
    constructMenu(LOOP_MENU);
  }
  else if(title == "MIDI"){
    constructMenu(MIDI_MENU);
  }
  else if(title == "FRAGMENT"){
    constructMenu(FRAGMENT);
  }
  else if(title == "CURVE"){
    constructMenu(CURVE);
  }
  else if(title == "ARP"){
    constructMenu(ARP);
  }
  else if(title == "FX"){
    constructMenu(FX);
  }
  //edit menu is called by note menu
  else if(title == "EDIT"){
    constructMenu(EDIT);
  }
  else if(title == "TRK"){
    constructMenu(TRK);
  }
  else if(title == "QZ"){
    constructMenu(QZ);
  }
  else if(title == "HUMANIZE"){
    constructMenu(HUMANIZE);
  }
  else if(title == "SAVE"){
    constructMenu(SAVE);
  }
  else if(title == "FILES"){
    constructMenu(FILES);
  }
  else if(title == "REC"){
    constructMenu(REC);
  }
}

