void saveMenu(){
  uint8_t menuStart = 0;
  uint8_t menuEnd = 3;
  clearButtons();
  while(menuIsActive){
    display.clearDisplay();
    activeMenu.displaySaveMenu(menuStart,menuEnd);
    display.display();
    readButtons();
    joyRead();
    saveMenuControls();
    //menu data shit
    if(activeMenu.highlight>menuEnd){
      menuEnd = activeMenu.highlight;
      menuStart = menuEnd-3;
    }
    else if(activeMenu.highlight<menuStart){
      menuStart = activeMenu.highlight;
      menuEnd = menuStart+3;
    }
  }
}

void Menu::displaySaveMenu(){
  displaySaveMenu(0,3);
}

void Menu::displaySaveMenu(uint8_t start, uint8_t end){
  unsigned short int menuHeight = abs(bottomR[1]-topL[1]);
  display.clearDisplay();
  int frame = millis()%700;
  if(frame<100){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc1_bmp,15,45,SSD1306_WHITE);
  }
  else if(frame<200){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc3_bmp,15,45,SSD1306_WHITE);
  }
  else if(frame<300){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc2_bmp,15,45,SSD1306_WHITE);
  }  
  else if(frame<400){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc4_bmp,15,45,SSD1306_WHITE);
  }
  else if(frame<500){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc1_bmp,15,45,SSD1306_WHITE);
  }
  else if(frame<600){
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc3_bmp,15,45,SSD1306_WHITE);
  }
  else{
    display.drawBitmap(topL[0]-5,(screenHeight-45)/2,disc2_bmp,15,45,SSD1306_WHITE);
  }
  //drawing menu box
  display.setCursor(topL[0]+9,topL[1]+3);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print(menuTitle);
  display.setFont();

  unsigned short int textWidth = 20;
  unsigned short int textHeight = menuHeight/5;

  unsigned short int menuStart = start;
  unsigned short int menuEnd = end;
  unsigned short int yLoc = 0;

  //making sure it can't read options that don't exist
  if(options.size() == 0)
    menuEnd = 0;
  else if(menuEnd>=options.size()){
    menuEnd = options.size()-1;
  }
  //printing out the menu
  for(int i = menuStart; i<=menuEnd; i++){
    display.setCursor(topL[0]+10,(yLoc+1)*textHeight+topL[1]+2);
    if(highlight != i){
      display.print(options[i]);
    }
    else if(highlight == i){
      display.fillRoundRect(topL[0]+8,(yLoc+1)*textHeight+topL[1]+1,screenWidth, textHeight-3, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
      display.print(options[i]);
    }
    display.setTextColor(SSD1306_WHITE);
    yLoc++;
  }
}