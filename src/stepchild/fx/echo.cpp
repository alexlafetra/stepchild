void drawEcho(unsigned short int xStart, unsigned short int yStart, short unsigned int time, short unsigned int decay, short unsigned int repeats){
  unsigned short int previewLength = 12;
  // drawNote(xStart,yStart,previewLength,trackHeight,1,false,false);//pilot note
  float vel = 127;
  for(int rep = 1; rep<repeats+1; rep++){
    vel = vel * decay/100;
    short unsigned int x1 = xStart+time*rep;
    short unsigned int y1 = yStart;
    // drawNote_vel(x1,y1,previewLength,trackHeight,vel,false,false);
  }
}

void echoMenu(){
  animOffset = 0;
  uint8_t cursor = 0;
  // echoAnimation();
  while(true){
    readButtons();
    readJoystick();
    if(!echoMenuControls(&cursor)){
      return;
    }
    display.clearDisplay();
    drawEchoMenu(cursor);
    display.display();
  }
}