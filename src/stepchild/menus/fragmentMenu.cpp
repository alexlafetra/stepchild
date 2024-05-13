void fragmentMenu(){
  //controls that we need
  /*
  - on/off
  - min/max start
  - min/max repeats
  - min/max length
  */
  fragmentAnimation(true);
  int xStart = 54;//line coord
  int timeS = playheadPos;
  int h;
  int k;
  int xDepth = random(-10,10);
  int yDepth = random(-10,10);
  int minReps = fragmentData[1][1];
  int maxReps = fragmentData[1][2];
  int minWidth = fragmentData[0][1];
  int maxWidth = fragmentData[0][2];
  int height;
  int width;

  //arbitrary max length val
  int maxLengthVal = 384;
  int maxRepsVal = 8;
  //for drawing the start bounds
  bool startedDrawing = false;
  while(menuIsActive){
    h = 80;
    k = 30;
    readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
      if(controls.MENU()){
        menuIsActive = false;
        constructMenu("MENU");
        lastTime = millis();
      }
      if(controls.stepButtons(3)){
        controls.stepButtons(3) = 0;
        constructMenu("MENU");
        menuIsActive = false;
        lastTime = millis();
      }
    }
    //controlling where the line starts
    if(utils.itsbeen(100) && x != 0){
      lastTime = millis();
      if(controls.joystickX == 1 && timeS != -screenWidth/2){//last part is so it doesn't 'bounce'
        if(controls.SHIFT()){
          timeS--;
        }
        else{
          if((timeS+64)%fragmentSubDiv){
            timeS-=(timeS+64)%fragmentSubDiv;
          }
          else{
            timeS-=fragmentSubDiv;
          }
        }
      }
      if(controls.joystickX == -1){
        if(controls.SHIFT()){
          timeS++;
        }
        else{
          if((timeS+64)%fragmentSubDiv){
            timeS+=fragmentSubDiv-(timeS+64)%fragmentSubDiv;
          }
          else{
            timeS+=fragmentSubDiv;
          }
        }
      }
      //bounds checking
      if(timeS<-screenWidth/2){
        timeS = -screenWidth/2;
      }
      if(timeS+64>seqEnd){
        timeS = seqEnd-64;
      }
    }
    if(utils.itsbeen(200)){
      //scrolling thru the menu, when controls.SHIFT() isn't held
      if(controls.joystickY != 0 && !controls.SHIFT()){
        if(controls.joystickY == 1){
          activeMenu.moveMenuCursor(true);
          lastTime = millis();
        }
        if(controls.joystickY == -1){
          activeMenu.moveMenuCursor(false);
          lastTime = millis();
        }
      }
      if(controls.SELECT() ){
        if(!controls.SHIFT()){
          fragmentData[2][1] = timeS+64;
          lastTime = millis();
        }
        else if(controls.SHIFT()){
          fragmentData[2][2] = timeS+64;
          lastTime = millis();
        }
        //if start is bigger than end, swap em
        if(fragmentData[2][1]>fragmentData[2][2]){
          int max = fragmentData[2][1];
          int min = fragmentData[2][2];
          fragmentData[2][1] = min;
          fragmentData[2][2] = max;
        }
      }
      if(controls.PLAY()){
        togglePlayMode();
        lastTime = millis();
      }
    }
    //encoder A changes minimums and maximums (while shifting)
    while(counterA != 0){
      //changing minimums
      if(!controls.SHIFT()){
        if(counterA >= 1){
          //changing min length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][1]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv)
              fragmentData[0][1]++;
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][1]<maxRepsVal)
              fragmentData[1][1]++;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(fragmentData[2][1]<seqEnd-subDivInt){
              fragmentData[2][1]+=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        else if(counterA <= -1){
          //changing min length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][1]*fragmentSubDiv>=fragmentSubDiv){
              fragmentData[0][1]--;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][1]>0)
              fragmentData[1][1]--;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(!controls.SHIFT() && fragmentData[2][1]>subDivInt){
              fragmentData[2][1]-=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        for(int i = 0; i<3; i++){
          if(fragmentData[i][1]>fragmentData[i][2]){
            fragmentData[i][2] = fragmentData[i][1];
          }
        }
      }
      //changing maximums
      else if(controls.SHIFT()){
        if(counterA >= 1){
          //changing max length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][2]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv){
              fragmentData[0][2]++;
            }
            else if(!controls.SHIFT() && fragmentData[0][2]<maxLengthVal-subDivInt){
              fragmentData[0][2]+=subDivInt;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][2]<maxRepsVal)
              fragmentData[1][2]++;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(controls.SHIFT() && fragmentData[2][2]<seqEnd){
              fragmentData[2][2]++;
            }
            else if(!controls.SHIFT() && fragmentData[2][2]<seqEnd-subDivInt){
              fragmentData[2][2]+=subDivInt;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        else if(counterA <= -1){
          //changing max length
          if(activeMenu.options[activeMenu.highlight] == "size"){
            if(fragmentData[0][2]*fragmentSubDiv>=fragmentSubDiv){
              fragmentData[0][2]--;
            }
          }
          //changing min reps
          else if(activeMenu.options[activeMenu.highlight] == "reps"){
            if(fragmentData[1][2]>0)
              fragmentData[1][2]--;
          }
          //changing min start
          else if(activeMenu.options[activeMenu.highlight] == "entry"){
            if(fragmentData[2][2]>0){
              fragmentData[2][2]--;
            }
          }
          else if(activeMenu.options[activeMenu.highlight] == "state"){
            isFragmenting = !isFragmenting;
          }
        }
        for(int i = 0; i<3; i++){
          if(fragmentData[i][1]>fragmentData[i][2]){
            fragmentData[i][1] = fragmentData[i][2];
          }
        }
      }
      counterA += counterA<0?1:-1;
    }
    //changing subDivInt
    while(counterB != 0 && activeMenu.options[activeMenu.highlight] == "div"){
      if(counterB <= -1 && !controls.SHIFT()){
        changeFragmentSubDivInt(true);
      }
      //changing subdivint
      if(counterB >= 1 && !controls.SHIFT()){
        changeFragmentSubDivInt(false);
      }
      //if shifting, toggle between 1/3 and 1/4 mode
      else while(counterB != 0 && controls.SHIFT()){
        toggleFragmentTriplets();
      }
      counterB += counterB<0?1:-1;;
    }
    display.clearDisplay();
    int menuStart;
    if(activeMenu.highlight>1){
      menuStart = activeMenu.highlight - 1;
    }
    else if(activeMenu.highlight<=1){
      menuStart = 0;
    }
    for(int i = menuStart; i<menuStart+3; i++){
      if(i<activeMenu.options.size()){
        display.setCursor(4,2+(i-menuStart)*10);
        if(i == activeMenu.highlight){
          display.drawRoundRect(2,1+(i-menuStart)*10,activeMenu.options[i].length()*6+3,10,4,SSD1306_WHITE);
          // display.setTextColor(SSD1306_BLACK);
          display.print(activeMenu.options[i]);
          // display.setTextColor(SSD1306_WHITE);
        }
        else{
          display.setTextColor(SSD1306_WHITE);
          display.print(activeMenu.options[i]);
        }
      }
    }
    //special displays for each menu option
    if(activeMenu.options[activeMenu.highlight] == "entry"){
      h = 64;
      //display timeline when selecting the start
      for(int lineStep = 0; lineStep<screenWidth; lineStep++){
        //if it's within the seq
        if((lineStep+timeS)<=seqEnd&&(lineStep+timeS)>=0){
          if(playing){
            timeS = playheadPos - 64;
            if(lineStep+timeS == playheadPos){
              display.drawRoundRect(lineStep, xStart-10, 3, 20, 3, SSD1306_WHITE);
            }
          }
          //timeline is dotted
          if((lineStep+timeS)%3){
            display.drawPixel(lineStep,xStart,SSD1306_WHITE);
          }
          //if it's on a subDiv
          if(!((lineStep+timeS)%subDivInt)){
            display.drawLine(lineStep,xStart-1,lineStep,xStart+1,SSD1306_WHITE);
          }
          //if it's on a measure
          if(!((lineStep+timeS)%96)){
            display.drawLine(lineStep,xStart-5,lineStep,xStart+5,SSD1306_WHITE);
          }
          //min fragment start value
          if(lineStep+timeS == fragmentData[2][1]){
            display.fillTriangle(lineStep-3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
          }
          //max fragment start value
          if(lineStep+timeS == fragmentData[2][2]){
            display.fillTriangle(lineStep+3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
          }
          //in between the two vals
          if(lineStep+timeS > fragmentData[2][1] && lineStep+timeS < fragmentData[2][2]){
            display.drawPixel(lineStep,xStart+3,SSD1306_WHITE);
            display.drawPixel(lineStep,xStart-3,SSD1306_WHITE);
          }
        }
      }
      printCursive(screenWidth-30,2,"min",SSD1306_WHITE);
      printCursive(screenWidth-30,20,"max",SSD1306_WHITE);
      display.setCursor(screenWidth-20-stepsToPosition(fragmentData[0][1],false).length()*3,10);
      display.print(stepsToPosition(fragmentData[2][1],false));
      display.setCursor(screenWidth-20-stepsToPosition(fragmentData[0][2],false).length()*3,28);
      display.print(stepsToPosition(fragmentData[2][2],false));
      display.setCursor(5,35);
      display.print(stepsToPosition(timeS+64,false));
    }
    else if(activeMenu.options[activeMenu.highlight] == "reps"){
      printCursive(0,k+15,"minimum",SSD1306_WHITE);
      printCursive(screenWidth-42,k+15,"maximum",SSD1306_WHITE);
      //drawing minimum
      String temp = stringify(fragmentData[1][1]);
      display.setCursor(21-temp.length()*3,k+25);
      display.print(temp);
      //drawing maximum
      temp = stringify(fragmentData[1][2]);
      display.setCursor(screenWidth-21-temp.length()*3,k+25);
      display.print(temp);

      if(fragmentData[1][2]>1 && isFragmenting){
        int reps = random(fragmentData[1][1],fragmentData[1][2]);
        if(reps == 1){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps == 2){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps == 3){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
        else if(reps>3){
          drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
          drawTetra(h-40+cos(millis()),k+10+sin(millis()),height*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
        }
      }
    }
    else if(activeMenu.options[activeMenu.highlight] == "size"){
      drawBracket(h,k+10,8,fragmentData[0][2]*fragmentSubDiv*float(screenWidth)/float(maxLengthVal)-1,0,SSD1306_WHITE);
      drawBracket(h,13,4,fragmentData[0][1]*fragmentSubDiv*float(screenWidth)/float(maxLengthVal)-1,1,SSD1306_WHITE);
      printCursive(0,k+15,"minimum",SSD1306_WHITE);
      printCursive(screenWidth-42,k+15,"maximum",SSD1306_WHITE);
      //drawing minimum
      String temp = "("+stringify(fragmentData[0][1])+")x("+stepsToMeasures(fragmentSubDiv)+")";
      display.setCursor(0,k+25);
      display.print(temp);
      //drawing maximum
      temp = "("+stringify(fragmentData[0][2])+")x("+stepsToMeasures(fragmentSubDiv)+")";
      display.setCursor(screenWidth-temp.length()*6,k+25);
      display.print(temp);
    }
    else if(activeMenu.options[activeMenu.highlight] == "state"){
      display.setFont(&FreeSerifItalic9pt7b);
      display.setCursor(5,50);
      if(isFragmenting){
        display.print("on");
        // printCursive(5,50,"on",SSD1306_WHITE);
      }
      else{
        // printCursive(5,50,"off",SSD1306_WHITE);
        display.print("off");
      }
      display.setFont();
    }
    else if(activeMenu.options[activeMenu.highlight] == "div"){
      display.setTextSize(2);
      display.setCursor(2,48);
      display.print(stepsToMeasures(fragmentSubDiv));
      display.setTextSize(1);
    }
    if(isFragmenting){
      width = random(fragmentData[0][1],fragmentData[0][2]);
      height = random(1,4);
    }
    else{
      width = fragmentData[0][2];
      height = 3;
    }
    // xDepth = random(-10,10);
    // yDepth = random(-10,10);
    //animated if the sequence is fragmenting
    if(!playing&&!recording){
      drawTetra(h+cos(millis()),k+sin(millis()),height*fragmentSubDiv*float(screenWidth)/float(maxRepsVal),width*fragmentSubDiv*float(screenWidth)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
    }
    else{
      drawTetra(h+cos(millis()),k+sin(millis()),fragmentData[1][0]*float(screenHeight)/float(fragmentData[1][2]),fragmentData[0][0]*fragmentSubDiv*float(screenWidth)/float(fragmentData[0][2]),xDepth,yDepth,0,SSD1306_WHITE);
    }
    display.display();
  }
  fragmentAnimation(false);
}