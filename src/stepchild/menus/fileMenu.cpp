//animation for the file menu
void fileAnimation(bool in){
  vector<String> filenames = {"*new*"};
  if(in){
    activeMenu.highlight = -1;
    int xDistance = screenWidth-activeMenu.coords.x1;//how far the display is gonna need to slide over
    int width = activeMenu.coords.x2 - activeMenu.coords.x1;
    for(int i = 0; i< xDistance; i+=10){
      //this literally just shifts where the menu is over and over again
      activeMenu.coords.x1 = screenWidth-i;
      activeMenu.coords.x2 = activeMenu.coords.x1 + width;
      display.clearDisplay();
      activeMenu.displayFileMenu(20-i/10,0,0,3,filenames);
      // display.drawBitmap(screenWidth-i,0,folder_closed_bmp,14,26,SSD1306_WHITE);
      display.display();
    }
    activeMenu.highlight = 0;
    display.drawBitmap(0,0,folder_open_bmp,24,26,SSD1306_WHITE);
  }
  else if(!in){
    activeMenu.highlight = -1;
    int xDistance = screenWidth-activeMenu.coords.x1;//how far the display is gonna need to slide over
    int width = activeMenu.coords.x2 - activeMenu.coords.x1;
    for(int i = xDistance; i> 0; i-=10){
      //this literally just shifts where the menu is over and over again
      activeMenu.coords.x1 = screenWidth-i;
      activeMenu.coords.x2 = activeMenu.coords.x1 + width;
      display.clearDisplay();
      activeMenu.displayFileMenu(30-i/10,0,0,3,filenames);
      // display.drawBitmap(xDistance-i,0,folder_closed_bmp,14,26,SSD1306_WHITE);
      display.display();
    }
    activeMenu.highlight = 0;
  }
}

vector<String> fileMenuControls_miniMenu(WireFrame* w,vector<String> filenames);
bool fileMenuControls(uint8_t menuStart, uint8_t menuEnd,WireFrame* w,vector<String> filenames);

void quickSave(){
  //if it hasn't been saved yet
  if(currentFile == ""){
    String fileName = enterText("filename?");
    if(fileName != "default"){
      writeSeqFile(fileName);
      currentFile = fileName;
      menuText = "saved \'"+currentFile+"\'";
    }
  }
  //if there is a filename for it
  else{
    writeSeqFile(currentFile);
    menuText = "saved \'"+currentFile+"\'";
  }
}

void fileMenuDisplayWrapper(){
  activeMenu.displayFileMenu();
}
void loadBackup(){
  //if there's an active filename
  if(currentFile != ""){
    vector<String> ops = {"NAUR","YEA"};
    int8_t choice = binarySelectionBox(59,32,"NO","YEA","LOAD BACKUP?",fileMenuDisplayWrapper);
    if(choice){
      loadSeqFile(currentFile);
      slideMenuOut(1,30);
      menuIsActive = false;
      constructMenu("MENU");
    }
  }
  //if there isn't, just enter files menu
  else{
    slideMenuOut(0,20);
    constructMenu("FILES");
  }
}

vector<String> fileMenuControls_miniMenu(WireFrame* w,vector<String> filenames){
  //scrolling
  if(itsbeen(100)){
    if(y == 1 && activeMenu.highlight<5){
      activeMenu.highlight++;
      lastTime = millis();
    }
    else if(y == -1 && activeMenu.highlight>0){
      activeMenu.highlight--;
      lastTime = millis();
    }
  }
  if(itsbeen(200)){
    //back to normal mode
    if(menu_Press){
      lastTime = millis();
      activeMenu.highlight = activeMenu.page;
      activeMenu.page = 0;
      openFolderAnimation(w,30);
    }
    //selecting an option
    if(sel){
      lastTime = millis();
      sel = false;
      switch(activeMenu.highlight){
        //overwrite
        case 1:
        {
          String fileName = filenames[activeMenu.page];
          int8_t choice = binarySelectionBox(64,32,"NO","YEA",displayMiniMenu);
          if(choice == 1){
            writeSeqFile(fileName);
          }
          break;
        }
        //load
        case 0:
        {
          String fileName = filenames[activeMenu.page];
          loadSeqFile(fileName);
          currentFile = fileName;
          alert("loaded "+currentFile+"!",500);
          break;
        }
        //rename
        case 2:
        {
          String filename = filenames[activeMenu.page];
          renameSeqFile(filename);
          filenames = loadFiles();
          break;
        }
        //export
        case 3:
          exportSeqFileToSerial_standAlone(filenames[activeMenu.page]);
          break;
        //delete
        case 4:
        {
          String filename = filenames[activeMenu.page];
          if(deleteSeqFile(filename));
          filenames = loadFiles();
          activeMenu.highlight = activeMenu.page-1;
          activeMenu.page = 0;
          openFolderAnimation(w,30);
          break;
        }
        //back
        case 5:
          lastTime = millis();
          activeMenu.highlight = activeMenu.page;
          activeMenu.page = 0;
          openFolderAnimation(w,30);
          break;
      }
    }
  }
  return filenames;
}

bool fileMenuControls(uint8_t menuStart, uint8_t menuEnd,WireFrame* w,vector<String> filenames){
  if(itsbeen(100)){
    if(y == -1 && activeMenu.highlight>0){
        activeMenu.highlight--;
        if(activeMenu.highlight<menuStart){
            menuStart--;
            menuEnd--;
        }
        lastTime = millis();
    }
    if(y == 1 && activeMenu.highlight<(filenames.size()-1)){
        activeMenu.highlight++;
        if(activeMenu.highlight>menuEnd){
            menuStart++;
            menuEnd++;
        }
        lastTime = millis();
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      return false;
    }
    if(n){
      if(shift){
        dumpFilesToSerial();
      }
      else{
      }
    }
    if(filenames.size()>0){
      if(sel){
        lastTime = millis();
        sel = false;
        if(filenames[activeMenu.highlight] == "*New*"){
          String fileName = enterText("filename?");
          if(fileName != "default"){
            writeSeqFile(fileName);
            // slideMenuOut(1,30);
            // menuIsActive = false;
            // constructMenu("MENU");
            return false;
          }
        }
        //entering the minimenu
        else{
          //set the 'page' variable to the current file index
          activeMenu.page = activeMenu.highlight;
          //reset highlight to 0
          activeMenu.highlight = 0;
          //open wireFrame
          openFolderAnimation(w,80);
          return true;
        }
      }
      if(del){
        lastTime = millis();
        del = false;
        String filename = filenames[activeMenu.highlight];
        deleteSeqFile(filename);
        filenames = loadFiles();
        if(filenames.size() == 0)
          activeMenu.highlight = 0;
      }
      if(copy_Press){
        lastTime = millis();
        copy_Press = false;
        String filename = filenames[activeMenu.highlight];
        duplicateSeqFile(filename);
        filenames = loadFiles();
      }
    }
  }
  return true;
}

void fileMenuAnimation(bool open, uint8_t menuStart, uint8_t menuEnd,vector<String> filenames, bool inOrOut){
  if(inOrOut){
    int16_t textOffset = 24;
    while(textOffset > 0){
      textOffset -= 5;
      if(textOffset< -5)
        textOffset = -5;
      display.clearDisplay();
      //draw menu
      activeMenu.displayFileMenu(textOffset,false,menuStart,menuEnd,filenames);
      display.display();
    }
  }
  else{
    int16_t textOffset = 0;
    while(textOffset < 128){
      textOffset+=16;
      display.clearDisplay();
      //draw menu
      activeMenu.displayFileMenu(textOffset,false,menuStart,menuEnd,filenames);
      display.display();
    }
  }
}

void fileMenu(){
  uint8_t menuStart = 0;
  uint8_t menuEnd = 4;
  clearButtons();
  WireFrame folder = makeFolder(30);
  folder.scale = 3;
  folder.xPos = 96;
  folder.yPos = screenHeight/2;
  vector<String> filenames = loadFiles();
  fileMenuAnimation(false,menuStart,menuEnd,filenames,true);
  while(menuIsActive){
    display.clearDisplay();
    //draw menu
    activeMenu.displayFileMenu(0,false,menuStart,menuEnd,filenames);
    //render wireframe
    folder.render();
    //draw mini menu
    if(activeMenu.page!=0)
      displayMiniMenu();
    display.display();
    folder.rotate(1,1);
    readButtons();
    readJoystick();
    if(activeMenu.page == 0){
      if(!fileMenuControls(menuStart,menuEnd,&folder,filenames)){
        fileMenuAnimation(false,menuStart,menuEnd,filenames,false);
        constructMenu(MAIN_MENU,9);
        return;
      }
        //menu data shit
      if(activeMenu.highlight>menuEnd){
        menuEnd = activeMenu.highlight;
        menuStart = menuEnd-4;
      }
      else if(activeMenu.highlight<menuStart){
        menuStart = activeMenu.highlight;
        menuEnd = menuStart+4;
      }
    }
    else{
      filenames = fileMenuControls_miniMenu(&folder,filenames);
    }
  }
}

void displayMiniMenu(){
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
  if(activeMenu.highlight == 5)
    display.drawBitmap(x1+2,y1+41,back_arrow_light_bmp,14,13,SSD1306_WHITE);
  else
    display.drawBitmap(x1+4,y1+43,back_arrow_bmp,10,9,SSD1306_WHITE);

  //if it's on an item other than the 'back' arrow
  for(uint8_t i = 0; i<5; i++){
    if(activeMenu.highlight == i){
      display.fillRoundRect(x1+2,y1+8*i+2,options[i].length()*4+4,7,2,SSD1306_WHITE);
    }
    printSmall(x1+4,y1+8*i+3,options[i],2);
  }
  printChunky(87-options[activeMenu.highlight].length()*3,y1-7,options[activeMenu.highlight],SSD1306_WHITE);

  //drawing icon
  if(activeMenu.highlight<5){
    display.fillRoundRect(100,29,16,16,3,0);
    display.drawRoundRect(100,29,16,16,3,1);

    display.drawBitmap(102,31,file_menu_icons[activeMenu.highlight],12,12,1);
  }
}

void Menu::displayFileMenu(){
  vector<String> filenames = {"*new*"};
  displayFileMenu(0,false,0,1,filenames);
}

void Menu::displayFileMenu(int16_t textOffset, bool open, uint8_t menuStart, uint8_t menuEnd,vector<String> filenames){
  unsigned short int menuHeight = abs(coords.y2-coords.y1);
  display.setCursor(coords.x1+9,coords.y1+3);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Files");
  display.setFont();
  printSmall(coords.x1+9,coords.y1+10,"--------",SSD1306_WHITE);

  const uint8_t textWidth = 20;
  const uint8_t textHeight = 9;

  uint8_t yLoc = 0;

  //making sure it can't read filenames that don't exist
  if(filenames.size() == 0){
    printSmall(24,24,"No files, kid",SSD1306_WHITE);
    return;
  }
  if(menuEnd>=filenames.size()){
    menuEnd = filenames.size()-1;
  }
  //drawing indicator arrows
  if(menuStart>0){
    graphics.drawArrow(coords.x1,coords.y1+13-sin(millis()/150),2,2,false);
  }
  if(menuEnd<filenames.size()-1){
    graphics.drawArrow(coords.x1,coords.y1+60+sin(millis()/150),2,3,false);
  }
  //printing out the menu
  for(int i = menuStart; i<=menuEnd; i++){
    if(page == 0){
      if(highlight != i){
        printSmall(coords.x1+10+textOffset*(i-menuStart),(yLoc+1)*textHeight+coords.y1+6,filenames[i],SSD1306_WHITE);
      }
      else if(highlight == i){
        graphics.drawBanner(coords.x1+16+textOffset*(i-menuStart),(yLoc+1)*textHeight+coords.y1+6,filenames[i]);
      }
    }
    else{
      if(page != i){
        printSmall(coords.x1+10+textOffset*(i-menuStart),(yLoc+1)*textHeight+coords.y1+6,filenames[i],SSD1306_WHITE);
      }
      else if(page == i){
        graphics.drawBanner(coords.x1+16+textOffset*(i-menuStart),(yLoc+1)*textHeight+coords.y1+6,filenames[i]);
      }
    }
    yLoc++;
  }
}
