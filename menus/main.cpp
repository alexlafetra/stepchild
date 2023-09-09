void drawMainMenuLabel(){
  String text;
  switch(activeMenu.highlight){
    //DT
    case 0:
      text = "AUTO";
      break;
    //Loop
    case 1:
      text = "LOOP";
      break;
    //instruments
    case 2:
      text = "INST";
      break;
    //settings
    case 3:
      text = "PREFS";
      break;
    //quicksave
    case 4:
      text = "QIKSAVE";
      break;
    //clock
    case 5:
      text = "FX";
      break;
    //rec
    case 6:
      text = "PLY/REC";
      break;
    //heart
    case 7:
      text = "CONSOLE";
      break;
    //midi
    case 8:
      text = "MIDI";
      break;
    //files
    case 9:
      text = "FILES";
      break;
    //clock
    case 10:
      text = "CLOCK";
      break;
    //arp menu
    case 11:
      text = "ARP";
      break;
  }
  // display.setRotation(1);
  drawLabel(112,32,text,true);
  // drawBanner(112-text.length()*2-countSpaces(text),32,text);

  // display.setRotation(UPRIGHT);
}
void mainMenu(){
  uint8_t activeWireFrame;
  WireFrame icon = getMenuWireFrame();
  icon.xPos = 112;
  icon.yPos = 16;
  while(true){
    if(!mainMenuControls())
      break;
    display.clearDisplay();
    drawSeq(true,false,false,false,false);
    drawPram(5,0);
    display.fillCircle(111,13,23,0);
    display.drawCircle(111,13,23,1);
    activeMenu.displayMainMenu();
    icon.render();
    drawMainMenuLabel();
    display.display();
    //if the highlight changes
    if(activeWireFrame != activeMenu.highlight){
      icon = getMenuWireFrame();
      icon.xPos = 112;
      if(activeMenu.highlight == 7)
        icon.yPos = 11;
      else
        icon.yPos = 16;
      activeWireFrame = activeMenu.highlight;
    }
    animateIcon(&icon);
    joyRead();
    readButtons();
  }
}

void Menu::displayMainMenu(){
  //drawing menu box (+16 so the title is transparent)
  display.fillRect(topL[0],topL[1]+13, bottomR[0]-topL[0], bottomR[1]-topL[1], SSD1306_BLACK);
  display.drawRect(topL[0],topL[1]+12, bottomR[0]-topL[0], bottomR[1]-topL[1]-12, SSD1306_WHITE);

  display.setCursor(topL[0]+topL[1]-2,5);
  display.setFont(&FreeSerifItalic9pt7b);
  display.print("Menu");
  display.setFont();

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      //animate rec icon 
      // if(count == 6){
      //   if(sin(millis()/200+count)<0)
      //     display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      //   else
      //     display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),playback2_bmp,12,12,SSD1306_WHITE);
      // }
      //animate clock hands
     if(count == 10){
        display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
        drawCircleRadian(topL[0]+4+width*i+6,topL[1]+j*(width-1)+17+sin(millis()/200+count)+6,4,millis()/10,0);
        drawCircleRadian(topL[0]+4+width*i+6,topL[1]+j*(width-1)+17+sin(millis()/200+count)+6,3,millis()/100,0);
      }
      else
        display.drawBitmap(topL[0]+4+width*i,topL[1]+j*(width-1)+17+sin(millis()/200+count),mainMenu_icons[count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight){
        display.drawRoundRect(topL[0]+2+width*i,topL[1]+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}