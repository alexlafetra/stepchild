class FileMenu:public StepchildMenu{
  public:
    vector<String> filenames = {"*new*"};
    WireFrame folderWireframe;
    uint8_t menuStart = 0;
    uint8_t menuEnd = 4;
    uint8_t page = 0;
    FileMenu(){
      coords = CoordinatePair(7,3,128,64);
      folderWireframe = makeFolder(30);
      folderWireframe.scale = 3;
      folderWireframe.xPos = 96;
      folderWireframe.yPos = screenHeight/2;
      filenames = loadFiles();
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
    int xDistance = screenWidth-coords.start.x;//how far the display is gonna need to slide over
    int width = coords.end.x - coords.start.x;
    for(int i = 0; i< xDistance; i+=10){
      //this literally just shifts where the menu is over and over again
      coords.start.x = screenWidth-i;
      coords.end.x = coords.start.x + width;
      displayMenu(20-i/10,false);
    }
    cursor = 0;
    display.drawBitmap(0,0,folder_open_bmp,24,26,SSD1306_WHITE);
  }
  else if(!in){
    cursor = -1;
    int xDistance = screenWidth-coords.start.x;//how far the display is gonna need to slide over
    int width = coords.end.x - coords.start.x;
    for(int i = xDistance; i> 0; i-=10){
      //this literally just shifts where the menu is over and over again
      coords.start.x = screenWidth-i;
      coords.end.x = coords.start.x + width;
      displayMenu(20-i/10,false);
    }
    cursor = 0;
  }
}

bool loadBackup(){
  //if there's an active filename
  if(currentFile != ""){
    vector<String> ops = {"NAUR","YEA"};
    int8_t choice = binarySelectionBox(59,32,"NO","YEA","LOAD BACKUP?");
    if(choice){
      loadSeqFile(currentFile);
      return true;
    }
    return false;
  }
  //if there isn't, just enter files menu
  else{
    // slideMenuOut(0,20);
    return false;
  }
}

bool FileMenu::fileMenuControls_miniMenu(){
  //scrolling
  if(utils.itsbeen(100)){
    if(controls.joystickY == 1 && cursor<5){
      cursor++;
      lastTime = millis();
    }
    else if(controls.joystickY == -1 && cursor>0){
      cursor--;
      lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    //back to normal mode
    if(controls.MENU()){
      lastTime = millis();
      cursor = page;
      page = 0;
      openFolderAnimation(folderWireframe,30);
    }
    //selecting an option
    if(controls.SELECT() ){
      lastTime = millis();
      controls.setSELECT(false);
      switch(cursor){
        //overwrite
        case 1:
        {
          String fileName = filenames[page];
          int8_t choice = binarySelectionBox(64,32,"NO","YEA","overwrite file?");
          if(choice == 1){
            writeSeqFile(fileName);
          }
          break;
        }
        //load
        case 0:
        {
          String fileName = filenames[page];
          loadSeqFile(fileName);
          currentFile = fileName;
          alert("loaded "+currentFile+"!",500);
          break;
        }
        //rename
        case 2:
        {
          String filename = filenames[page];
          renameSeqFile(filename);
          filenames = loadFiles();
          break;
        }
        //export
        case 3:{
#ifndef HEADLESS
          LittleFS.begin();
          File f = LittleFS.open("/SAVES/"+filenames[page],"r");
          writeFileToSerial(f);
          LittleFS.end();
#endif
          break;}
        //del
        case 4:
        {
          String filename = filenames[page];
          if(deleteSeqFile(filename));
          filenames = loadFiles();
          cursor = page-1;
          page = 0;
          openFolderAnimation(folderWireframe,30);
          break;
        }
        //back
        case 5:
          lastTime = millis();
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
  controls.readButtons();
  controls.readJoystick();
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
  if(utils.itsbeen(100)){
    if(controls.joystickY == -1 && cursor>0){
        cursor--;
        if(cursor<menuStart){
            menuStart--;
            menuEnd--;
        }
        lastTime = millis();
    }
    if(controls.joystickY == 1 && cursor<(filenames.size()-1)){
        cursor++;
        if(cursor>menuEnd){
            menuStart++;
            menuEnd++;
        }
        lastTime = millis();
    }
  }
  if(utils.itsbeen(200)){
    if(controls.MENU()){
      lastTime = millis();
      return false;
    }
    if(controls.NEW()){
      lastTime = millis();
      String fileName = enterText("filename?");
      if(fileName != ""){
        writeSeqFile(fileName);
        filenames = loadFiles();
      }
    }
    if(filenames.size()>0){
      if(controls.SELECT() ){
        lastTime = millis();
        controls.setSELECT(false);
        if(filenames[cursor] == "*new*"){
          String fileName = enterText("filename?");
          if(fileName != ""){
            writeSeqFile(fileName);
            filenames = loadFiles();
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
      if(controls.DELETE()){
        lastTime = millis();
        controls.setDELETE(false);
        String filename = filenames[cursor];
        deleteSeqFile(filename);
        filenames = loadFiles();
        if(filenames.size() == 0)
          cursor = 0;
      }
      if(controls.COPY()){
        lastTime = millis();
        controls.setCOPY(false);
        String filename = filenames[cursor];
        duplicateSeqFile(filename);
        filenames = loadFiles();
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


void fileMenu(){
  controls.clearButtons();
  FileMenu fileMenu;
  fileMenu.fileMenuAnimation(false,true);
  while(fileMenu.fileMenuControls()){
    //draw menu
    fileMenu.displayMenu();
  }
  fileMenu.fileMenuAnimation(false,false);
}

void FileMenu::displayMiniMenu(){
  const uint8_t x1 = 66;
  const uint8_t y1 = 9;
  const vector<String> options = {"Load","Overwrite","Rename","Export","Delete","Back"};

  //mask for back arrow
  display.fillRoundRect(x1,y1+39,19,16,3,SSD1306_BLACK);
  display.drawRoundRect(x1,y1+39,19,16,3,SSD1306_WHITE);

  //menubox
  display.fillRoundRect(x1,y1,44,43,3,SSD1306_BLACK);
  display.drawRoundRect(x1,y1,44,43,3,SSD1306_WHITE);

  //mask (again)
  display.drawFastHLine(x1+1,y1+42,17,SSD1306_BLACK);
  display.drawPixel(x1+1,y1+41,SSD1306_BLACK);
  if(cursor == 5)
    display.drawBitmap(x1+2,y1+41,back_arrow_light_bmp,14,13,SSD1306_WHITE);
  else
    display.drawBitmap(x1+4,y1+43,back_arrow_bmp,10,9,SSD1306_WHITE);

  //if it's on an item other than the 'back' arrow
  for(uint8_t i = 0; i<5; i++){
    if(cursor == i){
      display.fillRoundRect(x1+2,y1+8*i+2,options[i].length()*4+4,7,2,SSD1306_WHITE);
    }
    printSmall(x1+4,y1+8*i+3,options[i],2);
  }
  printChunky(87-options[cursor].length()*3,y1-7,options[cursor],SSD1306_WHITE);

  //drawing icon
  if(cursor<5){
    display.fillRoundRect(100,29,16,16,3,0);
    display.drawRoundRect(100,29,16,16,3,1);

    display.drawBitmap(102,31,file_menu_icons[cursor],12,12,1);
  }
}

void FileMenu::displayMenu(int16_t textOffset, bool open){
  display.clearDisplay();
  unsigned short int menuHeight = abs(coords.end.y-coords.start.y);
  display.setCursor(coords.start.x+9,coords.start.y+3);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Files");
  display.setFont();
  printSmall(coords.start.x+9,coords.start.y+10,"--------",SSD1306_WHITE);

  if(cursor && !page)
    printSmall(coords.start.x-4,coords.start.y,stringify(cursor)+"/"+stringify(filenames.size()-1),1);

  const uint8_t textWidth = 20;
  const uint8_t textHeight = 9;

  uint8_t yLoc = 0;
  menuEnd = menuStart+4;
  if(menuEnd>=filenames.size()){
    menuEnd = filenames.size()-1;
  }
  //drawing indicator arrows
  if(menuStart>0){
    graphics.drawArrow(coords.start.x,coords.start.y+13-sin(millis()/150),2,2,false);
  }
  if(menuEnd<filenames.size()-1){
    graphics.drawArrow(coords.start.x,coords.start.y+60+sin(millis()/150),2,3,false);
  }
  //printing out the menu
  for(uint8_t i = menuStart; i<=menuEnd; i++){    
    if(page == 0){
      if(cursor != i){
        printSmall(coords.start.x+min(10+textOffset*(i-menuStart),screenWidth),(yLoc+1)*textHeight+coords.start.y+6,filenames[i],SSD1306_WHITE);
      }
      else if(cursor == i){
        graphics.drawBanner(coords.start.x+min(16+textOffset*(i-menuStart),screenWidth),(yLoc+1)*textHeight+coords.start.y+6,filenames[i]);
      }
    }
    else{
      if(page != i){
        printSmall(coords.start.x+min(10+textOffset*(i-menuStart),screenWidth),(yLoc+1)*textHeight+coords.start.y+6,filenames[i],SSD1306_WHITE);
      }
      else if(page == i){
        graphics.drawBanner(coords.start.x+min(16+textOffset*(i-menuStart),screenWidth),(yLoc+1)*textHeight+coords.start.y+6,filenames[i]);
      }
    }
    yLoc++;
  }
  folderWireframe.render();
  //draw mini menu
  if(page!=0)
    displayMiniMenu();
  display.display();
  folderWireframe.rotate(1,1);
}
