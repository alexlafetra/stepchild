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
          menuIsActive = false;
          constructMenu("MENU");
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
      slideMenuOut(1,30);
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
            slideMenuOut(1,30);
            menuIsActive = false;
            constructMenu("MENU");
          }
        }
        //entering the minimenu
        else{
          //set the 'page' variable to the current file index
          activeMenu.page = activeMenu.highlight;
          //reset highlight to 0
          activeMenu.highlight = 0;
          //open wireFrame
          openFolderAnimation(w,120);
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

void filesMenu(){
  uint8_t menuStart = 0;
  uint8_t menuEnd = 4;
  clearButtons();
  WireFrame folder = makeFolder(30);
  folder.scale = 3;
  folder.xPos = 96;
  folder.yPos = screenHeight/2;
  vector<String> filenames = loadFiles();
  while(menuIsActive){
    display.clearDisplay();
    //draw menu
    activeMenu.displayFilesMenu(0,false,menuStart,menuEnd,filenames);
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
        constructMenu("MENU");
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
}

void Menu::displayFilesMenu(){
  vector<String> filenames = {"*new*"};
  displayFilesMenu(0,false,0,1,filenames);
}

void Menu::displayFilesMenu(int16_t textOffset, bool open, uint8_t menuStart, uint8_t menuEnd,vector<String> filenames){
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
    drawArrow(coords.x1,coords.y1+13-sin(millis()/150),2,2,false);
  }
  if(menuEnd<filenames.size()-1){
    drawArrow(coords.x1,coords.y1+60+sin(millis()/150),2,3,false);
  }
  //printing out the menu
  for(int i = menuStart; i<=menuEnd; i++){
    if(page == 0){
      if(highlight != i){
        printSmall(coords.x1+10,(yLoc+1)*textHeight+coords.y1+6,filenames[i],SSD1306_WHITE);
      }
      else if(highlight == i){
        drawBanner(coords.x1+16,(yLoc+1)*textHeight+coords.y1+6,filenames[i]);
      }
    }
    else{
      if(page != i){
        printSmall(coords.x1+10,(yLoc+1)*textHeight+coords.y1+6,filenames[i],SSD1306_WHITE);
      }
      else if(page == i){
        drawBanner(coords.x1+16,(yLoc+1)*textHeight+coords.y1+6,filenames[i]);
      }
    }
    yLoc++;
  }
}
