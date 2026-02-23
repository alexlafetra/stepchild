#include "guiUtilities.h"
#include "Stepchild.h"
#include "StepchildGraphics.h"
#include "graphics/WireFrame.h"
extern StepchildGraphics graphics;
extern Stepchild stepchild;

using namespace std;

// 'folder2', 24x26px
const unsigned char folder_open_bmp []  = {
  0x00, 0x20, 0x00, 0x00, 0x60, 0x00, 0x02, 0xa0, 0x00, 0x05, 0x20, 0x00, 0x08, 0x20, 0x00, 0x10, 
  0x20, 0x00, 0x10, 0x20, 0x00, 0x10, 0x20, 0x00, 0x20, 0x20, 0x00, 0x40, 0x20, 0x00, 0x80, 0x20, 
  0x00, 0x80, 0x20, 0x00, 0x80, 0x20, 0x00, 0x80, 0x20, 0x03, 0x80, 0x23, 0xfd, 0x80, 0x3c, 0x02, 
  0x80, 0x40, 0x04, 0x80, 0x80, 0x08, 0x81, 0x00, 0x10, 0x82, 0x00, 0x20, 0x84, 0x00, 0x40, 0x88, 
  0x00, 0x80, 0x90, 0x01, 0x00, 0xa0, 0x0e, 0x00, 0xcf, 0xf0, 0x00, 0xf0, 0x00, 0x00
};

// 'back_arrow_small_highlight', 14x13px
const unsigned char back_arrow_light_bmp []  = {
	0x0f, 0x80, 0x10, 0x60, 0x20, 0x10, 0x10, 0x08, 0x0c, 0x04, 0x13, 0x04, 0x20, 0x04, 0x40, 0x08, 
	0x80, 0x10, 0x40, 0x20, 0x20, 0xc0, 0x17, 0x00, 0x08, 0x00
};


class FileMenu:public StepchildMenu{
  public:
    vector<String> filenames = {"*new*"};
    WireFrame folderWireframe;
    uint8_t menuStart = 0;
    uint8_t menuEnd = 4;
    uint8_t page = 0;
    bool loadedAFile = false;
    FileMenu(){
      coords = CoordinatePair(7,3,128,64);
      folderWireframe = makeFolder(30);
      folderWireframe.scale = 3;
      folderWireframe.offset.x = 96;
      folderWireframe.offset.y = stepchild.SCREEN_HEIGHT/2;
      filenames = stepchild.filesystem.loadFiles();
    }
    void displayMenu();
    void displayMenu(int16_t,bool);
    void displayMiniMenu();
    void fileAnimation(bool);
    void fileMenuAnimation(bool open, bool inOrOut);
    bool fileMenuControls();
    bool fileMenuControls_default();
    bool fileMenuControls_miniMenu();
};

//animation for the file menu
void FileMenu::fileAnimation(bool in){
  if(in){
    cursor = -1;
    int xDistance = stepchild.SCREEN_WIDTH-coords.start.x;//how far the display is gonna need to slide over
    int width = coords.end.x - coords.start.x;
    for(uint16_t i = 0; i< xDistance; i+=1){
      //this literally just shifts where the menu is over and over again
      coords.start.x = stepchild.SCREEN_WIDTH-i;
      coords.end.x = coords.start.x + width;
      displayMenu(20-i/20,false);
    }
    cursor = 0;
    stepchild.display.drawBitmap(0,0,folder_open_bmp,24,26,SSD1306_WHITE);
  }
  else if(!in){
    cursor = -1;
    int xDistance = stepchild.SCREEN_WIDTH-coords.start.x;//how far the display is gonna need to slide over
    int width = coords.end.x - coords.start.x;
    for(uint16_t i = xDistance; i> 0; i-=1){
      //this literally just shifts where the menu is over and over again
      coords.start.x = stepchild.SCREEN_WIDTH-i;
      coords.end.x = coords.start.x + width;
      displayMenu(20-i/20,false);
    }
    cursor = 0;
  }
}

//special function that loads a backup file
bool loadBackup(){
  //if there's an active filename
  if(stepchild.filesystem.currentFilename != ""){
    vector<String> ops = {"NAUR","YEA"};
    int8_t choice = binarySelectionBox(59,32,"NO","YEA","LOAD BACKUP?");
    if(choice){
      stepchild.filesystem.loadSeqFile(stepchild.filesystem.currentFilename);
      return true;
    }
    return false;
  }
  //if there isn't, just enter files menu
  // slideMenuOut(0,20);
  return false;
}

bool FileMenu::fileMenuControls_miniMenu(){
  //scrolling
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickY == 1 && cursor<5){
      cursor++;
      stepchild.lastTime = millis();
    }
    else if(stepchild.buttons.joystickY == -1 && cursor>0){
      cursor--;
      stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(200)){
    //back to normal mode
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      cursor = page;
      page = 0;
      openFolderAnimation(folderWireframe,30);
    }
    //selecting an option
    if(stepchild.buttons.SELECT() ){
      stepchild.lastTime = millis();
      stepchild.buttons.setSELECT(false);
      switch(cursor){
        //overwrite
        case 1:
        {
          String fileName = filenames[page];
          int8_t choice = binarySelectionBox(64,32,"NO","YEA","overwrite file?");
          if(choice == 1){
            stepchild.filesystem.writeSeqFile(fileName);
          }
          break;
        }
        //load
        case 0:
        {
          String fileName = filenames[page];
          stepchild.filesystem.loadSeqFile(fileName);
          stepchild.filesystem.currentFilename = fileName;
          alert("loaded "+stepchild.filesystem.currentFilename+"!",500);
          loadedAFile = true;
          return false;
        }
        //rename
        case 2:
        {
          String filename = filenames[page];
          stepchild.filesystem.renameSeqFile(filename);
          filenames = stepchild.filesystem.loadFiles();
          break;
        }
        //export
        case 3:{
#ifndef HEADLESS
          LittleFS.begin();
          File f = LittleFS.open("/SAVES/"+filenames[page],"r");
          stepchild.filesystem.writeFileToSerial(f);
          LittleFS.end();
#endif
          break;}
        //del
        case 4:
        {
          String filename = filenames[page];
          if(stepchild.filesystem.deleteSeqFile(filename)){
            filenames = stepchild.filesystem.loadFiles();
          }
          cursor = page-1;
          page = 0;
          openFolderAnimation(folderWireframe,30);
          break;
        }
        //back
        case 5:
          stepchild.lastTime = millis();
          cursor = page;
          page = 0;
          openFolderAnimation(folderWireframe,30);
          break;
      }
    }
  }
  return true;
}

bool FileMenu::fileMenuControls(){
  stepchild.buttons.readButtons();
  stepchild.buttons.readJoystick();
  if(page == 0){
    return fileMenuControls_default();
  }
  else{
    return fileMenuControls_miniMenu();
  }
}

bool FileMenu::fileMenuControls_default(){
  //menu data shit
  if(cursor>menuEnd){
    menuEnd = cursor;
    menuStart = menuEnd-4;
  }
  else if(cursor<menuStart){
    menuStart = cursor;
    menuEnd = menuStart+4;
  }
  if(stepchild.itsbeen(100)){
    if(stepchild.buttons.joystickY == -1 && cursor>0){
        cursor--;
        if(cursor<menuStart){
            menuStart--;
            menuEnd--;
        }
        stepchild.lastTime = millis();
    }
    if(stepchild.buttons.joystickY == 1 && cursor<(filenames.size()-1)){
        cursor++;
        if(cursor>menuEnd){
            menuStart++;
            menuEnd++;
        }
        stepchild.lastTime = millis();
    }
  }
  if(stepchild.itsbeen(200)){
    if(stepchild.buttons.MENU()){
      stepchild.lastTime = millis();
      return false;
    }
    if(stepchild.buttons.NEW()){
      stepchild.lastTime = millis();
      String fileName = enterText("filename?");
      if(fileName != ""){
        stepchild.filesystem.writeSeqFile(fileName);
        filenames = stepchild.filesystem.loadFiles();
      }
    }
    if(filenames.size()>0){
      if(stepchild.buttons.SELECT() ){
        stepchild.lastTime = millis();
        stepchild.buttons.setSELECT(false);
        if(filenames[cursor] == "*new*"){
          String fileName = enterText("filename?");
          if(fileName != ""){
            stepchild.filesystem.writeSeqFile(fileName);
            filenames = stepchild.filesystem.loadFiles();
          }
        }
        //entering the minimenu
        else{
          //set the 'page' variable to the current file index
          page = cursor;
          //reset highlight to 0
          cursor = 0;
          //open wireFrame
          openFolderAnimation(folderWireframe,80);
          return true;
        }
      }
      if(stepchild.buttons.DELETE()){
        stepchild.lastTime = millis();
        stepchild.buttons.setDELETE(false);
        String filename = filenames[cursor];
        stepchild.filesystem.deleteSeqFile(filename);
        filenames = stepchild.filesystem.loadFiles();
        if(filenames.size() == 0)
          cursor = 0;
      }
      if(stepchild.buttons.COPY()){
        stepchild.lastTime = millis();
        stepchild.buttons.setCOPY(false);
        String filename = filenames[cursor];
        stepchild.filesystem.duplicateSeqFile(filename);
        filenames = stepchild.filesystem.loadFiles();
      }
    }
  }
  return true;
}

void FileMenu::fileMenuAnimation(bool open, bool inOrOut){
  if(inOrOut){
    int16_t textOffset = 24;
    while(textOffset > 0){
      textOffset -= 5;
      if(textOffset< -5)
        textOffset = -5;
      //draw menu
      displayMenu(textOffset,false);
    }
  }
  else{
    int16_t textOffset = 0;
    while(textOffset < 128){
      textOffset+=16;
      //draw menu
      displayMenu(textOffset,false);
    }
  }
}

void FileMenu::displayMenu(){
  FileMenu::displayMenu(0,false);
}


bool fileMenu(){
  stepchild.buttons.clearButtons();
  FileMenu fileMenu;
  fileMenu.fileMenuAnimation(false,true);
  while(fileMenu.fileMenuControls()){
    //draw menu
    fileMenu.displayMenu();
  }
  fileMenu.fileMenuAnimation(false,false);
  if(fileMenu.loadedAFile)
    return true;
  return false;
}

void FileMenu::displayMiniMenu(){
  const uint8_t x1 = 66;
  const uint8_t y1 = 9;
  const vector<String> options = {"Load","Overwrite","Rename","Export","Delete","Back"};

  //mask for back arrow
  stepchild.display.fillRoundRect(x1,y1+39,19,16,3,SSD1306_BLACK);
  stepchild.display.drawRoundRect(x1,y1+39,19,16,3,SSD1306_WHITE);

  //menubox
  stepchild.display.fillRoundRect(x1,y1,44,43,3,SSD1306_BLACK);
  stepchild.display.drawRoundRect(x1,y1,44,43,3,SSD1306_WHITE);

  //mask (again)
  stepchild.display.drawFastHLine(x1+1,y1+42,17,SSD1306_BLACK);
  stepchild.display.drawPixel(x1+1,y1+41,SSD1306_BLACK);
  if(cursor == 5)
    stepchild.display.drawBitmap(x1+2,y1+41,back_arrow_light_bmp,14,13,SSD1306_WHITE);
  else
    stepchild.display.drawBitmap(x1+4,y1+43,back_arrow_bmp,10,9,SSD1306_WHITE);

  //if it's on an item other than the 'back' arrow
  for(uint8_t i = 0; i<5; i++){
    if(cursor == i){
      stepchild.display.fillRoundRect(x1+2,y1+8*i+2,options[i].length()*4+4,7,2,SSD1306_WHITE);
    }
    graphics.printSmall(x1+4,y1+8*i+3,options[i],2);
  }
  graphics.printChunky(87-options[cursor].length()*3,y1-7,options[cursor],SSD1306_WHITE);

  //drawing icon
  if(cursor<5){
    stepchild.display.fillRoundRect(100,29,16,16,3,0);
    stepchild.display.drawRoundRect(100,29,16,16,3,1);

    stepchild.display.drawBitmap(102,31,file_menu_icons[cursor],12,12,1);
  }
}

void FileMenu::displayMenu(int16_t textOffset, bool open){
  stepchild.display.clearDisplay();
  stepchild.display.setCursor(coords.start.x+9,coords.start.y+3);
  stepchild.display.setFont(&FreeSerifItalic9pt7b);
  stepchild.display.print("Files");
  stepchild.display.setFont();
  graphics.printSmall(coords.start.x+9,coords.start.y+10,"--------",SSD1306_WHITE);

  if(cursor && !page)
    graphics.printSmall(coords.start.x-4,coords.start.y,stringify(cursor)+"/"+stringify(filenames.size()-1),1);

  const uint8_t textHeight = 9;

  uint8_t yLoc = 0;
  menuEnd = menuStart+4;
  if(menuEnd>=filenames.size()){
    menuEnd = filenames.size()-1;
  }
  //drawing indicator arrows
  if(menuStart>0){
    graphics.drawArrow(coords.start.x,coords.start.y+13-sin(millis()/150),2,ARROW_UP,false);
  }
  if(menuEnd<filenames.size()-1){
    graphics.drawArrow(coords.start.x,coords.start.y+60+sin(millis()/150),2,ARROW_DOWN,false);
  }
  //printing out the menu
  for(uint8_t i = menuStart; i<=menuEnd; i++){    
    if(page == 0){
      if(cursor != i){
        graphics.printSmall(coords.start.x+min(10+textOffset*(i-menuStart),stepchild.SCREEN_WIDTH),(yLoc+1)*textHeight+coords.start.y+6,filenames[i],SSD1306_WHITE);
      }
      else if(cursor == i){
        graphics.drawBanner(coords.start.x+min(16+textOffset*(i-menuStart),stepchild.SCREEN_WIDTH),(yLoc+1)*textHeight+coords.start.y+6,filenames[i]);
      }
    }
    else{
      if(page != i){
        graphics.printSmall(coords.start.x+min(10+textOffset*(i-menuStart),stepchild.SCREEN_WIDTH),(yLoc+1)*textHeight+coords.start.y+6,filenames[i],SSD1306_WHITE);
      }
      else if(page == i){
        graphics.drawBanner(coords.start.x+min(16+textOffset*(i-menuStart),stepchild.SCREEN_WIDTH),(yLoc+1)*textHeight+coords.start.y+6,filenames[i]);
      }
    }
    yLoc++;
  }
  folderWireframe.render();
  //draw mini menu
  if(page!=0)
    displayMiniMenu();
  stepchild.display.display();
  folderWireframe.rotate(1,1);
}
