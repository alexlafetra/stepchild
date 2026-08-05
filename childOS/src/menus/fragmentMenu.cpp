// #include "Stepchild.h"
// ;
// using namespace std;

// void fragmentAnimation(bool in){
//   if(in){
//     int width = 20;
//     int height = 20;
//     int xDepth = 10;
//     int yDepth = 10;
//     int maxWidth = 100;
//     int maxHeight = 100;
//     int h = 64;
//     int k = 40;
//     while(maxWidth>1){
//       if(millis()%1000>10){
//         height = random(maxHeight-10,maxHeight);
//         width = random(maxWidth-10,maxWidth);
//       }
//       stepchild.display.clearDisplay();
//       graphics.drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
//       maxWidth-=15;
//       maxHeight-=15;
//       k-=3;
//       stepchild.display.display();
//     }
//   }
//   else if(!in){
//     int width = 20;
//     int height = 20;
//     int xDepth = 10;
//     int yDepth = 10;
//     int maxWidth = 0;
//     int maxHeight = 0;
//     int h = 64;
//     int k = 20;
//     while(maxWidth<100){
//       if(millis()%1000>10){
//         height = random(maxHeight-10,maxHeight);
//         width = random(maxWidth-10,maxWidth);
//       }
//       stepchild.display.clearDisplay();
//       graphics.drawTetra(h,k,height,width,xDepth,yDepth,0,SSD1306_WHITE);
//       maxWidth+=15;
//       maxHeight+=15;
//       k+=3;
//       stepchild.display.display();
//     }
//   }
// }

// void fragmentMenu(){
//   //controls that we need
//   /*
//   - on/off
//   - min/max start
//   - min/max repeats
//   - min/max length
//   */
//   fragmentAnimation(true);
//   int xStart = 54;//line coord
//   int timeS = playheadPos;
//   int h;
//   int k;
//   int xDepth = random(-10,10);
//   int yDepth = random(-10,10);
//   int minReps = fragmentData[1][1];
//   int maxReps = fragmentData[1][2];
//   int minWidth = fragmentData[0][1];
//   int maxWidth = fragmentData[0][2];
//   int height;
//   int width;

//   //arbitrary max length val
//   int maxLengthVal = 384;
//   int maxRepsVal = 8;
//   //for drawing the start bounds
//   bool startedDrawing = false;
//   while(true){
//     h = 80;
//     k = 30;
//     stepchild.buttons.readButtons();
//     stepchild.buttons.readJoystick();
//     if(stepchild.itsbeen(200)){
//       if(stepchild.buttons.MENU()){
//         return;
//       }
//       if(stepchild.buttons.stepButtons(3)){
//         stepchild.buttons.stepButtons(3) = 0;
//         return;
//       }
//     }
//     //controlling where the line starts
//     if(stepchild.itsbeen(100) && x != 0){
//       stepchild.lastTime = millis();
//       if(stepchild.buttons.joystickX == 1 && timeS != -stepchild.SCREEN_WIDTH/2){//last part is so it doesn't 'bounce'
//         if(stepchild.buttons.SHIFT()){
//           timeS--;
//         }
//         else{
//           if((timeS+64)%fragmentSubDiv){
//             timeS-=(timeS+64)%fragmentSubDiv;
//           }
//           else{
//             timeS-=fragmentSubDiv;
//           }
//         }
//       }
//       if(stepchild.buttons.joystickX == -1){
//         if(stepchild.buttons.SHIFT()){
//           timeS++;
//         }
//         else{
//           if((timeS+64)%fragmentSubDiv){
//             timeS+=fragmentSubDiv-(timeS+64)%fragmentSubDiv;
//           }
//           else{
//             timeS+=fragmentSubDiv;
//           }
//         }
//       }
//       //bounds checking
//       if(timeS<-stepchild.SCREEN_WIDTH/2){
//         timeS = -stepchild.SCREEN_WIDTH/2;
//       }
//       if(timeS+64>stepchild.sequenceLength){
//         timeS = stepchild.sequenceLength-64;
//       }
//     }
//     if(stepchild.itsbeen(200)){
//       //scrolling thru the menu, when stepchild.buttons.SHIFT() isn't held
//       if(stepchild.buttons.joystickY != 0 && !stepchild.buttons.SHIFT()){
//         if(stepchild.buttons.joystickY == 1){
//           activeMenu.moveMenuCursor(true);
//           stepchild.lastTime = millis();
//         }
//         if(stepchild.buttons.joystickY == -1){
//           activeMenu.moveMenuCursor(false);
//           stepchild.lastTime = millis();
//         }
//       }
//       if(stepchild.buttons.SELECT() ){
//         if(!stepchild.buttons.SHIFT()){
//           fragmentData[2][1] = timeS+64;
//           stepchild.lastTime = millis();
//         }
//         else if(stepchild.buttons.SHIFT()){
//           fragmentData[2][2] = timeS+64;
//           stepchild.lastTime = millis();
//         }
//         //if start is bigger than end, swap em
//         if(fragmentData[2][1]>fragmentData[2][2]){
//           int max = fragmentData[2][1];
//           int min = fragmentData[2][2];
//           fragmentData[2][1] = min;
//           fragmentData[2][2] = max;
//         }
//       }
//       if(stepchild.buttons.PLAY()){
//         stepchild.togglePlay();
//         stepchild.lastTime = millis();
//       }
//     }
//     //encoder A changes minimums and maximums (while shifting)
//     while(stepchild.buttons.counterA != 0){
//       //changing minimums
//       if(!stepchild.buttons.SHIFT()){
//         if(stepchild.buttons.counterA >= 1){
//           //changing min length
//           if(activeMenu.options[activeMenu.highlight] == "size"){
//             if(fragmentData[0][1]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv)
//               fragmentData[0][1]++;
//           }
//           //changing min reps
//           else if(activeMenu.options[activeMenu.highlight] == "reps"){
//             if(fragmentData[1][1]<maxRepsVal)
//               fragmentData[1][1]++;
//           }
//           //changing min start
//           else if(activeMenu.options[activeMenu.highlight] == "entry"){
//             if(fragmentData[2][1]<stepchild.sequenceLength-stepchild.subDivision){
//               fragmentData[2][1]+=stepchild.subDivision;
//             }
//           }
//           else if(activeMenu.options[activeMenu.highlight] == "state"){
//             isFragmenting = !isFragmenting;
//           }
//         }
//         else if(stepchild.buttons.counterA <= -1){
//           //changing min length
//           if(activeMenu.options[activeMenu.highlight] == "size"){
//             if(fragmentData[0][1]*fragmentSubDiv>=fragmentSubDiv){
//               fragmentData[0][1]--;
//             }
//           }
//           //changing min reps
//           else if(activeMenu.options[activeMenu.highlight] == "reps"){
//             if(fragmentData[1][1]>0)
//               fragmentData[1][1]--;
//           }
//           //changing min start
//           else if(activeMenu.options[activeMenu.highlight] == "entry"){
//             if(!stepchild.buttons.SHIFT() && fragmentData[2][1]>stepchild.subDivision){
//               fragmentData[2][1]-=stepchild.subDivision;
//             }
//           }
//           else if(activeMenu.options[activeMenu.highlight] == "state"){
//             isFragmenting = !isFragmenting;
//           }
//         }
//         for(int i = 0; i<3; i++){
//           if(fragmentData[i][1]>fragmentData[i][2]){
//             fragmentData[i][2] = fragmentData[i][1];
//           }
//         }
//       }
//       //changing maximums
//       else if(stepchild.buttons.SHIFT()){
//         if(stepchild.buttons.counterA >= 1){
//           //changing max length
//           if(activeMenu.options[activeMenu.highlight] == "size"){
//             if(fragmentData[0][2]*fragmentSubDiv<=maxLengthVal-fragmentSubDiv){
//               fragmentData[0][2]++;
//             }
//             else if(!stepchild.buttons.SHIFT() && fragmentData[0][2]<maxLengthVal-stepchild.subDivision){
//               fragmentData[0][2]+=stepchild.subDivision;
//             }
//           }
//           //changing min reps
//           else if(activeMenu.options[activeMenu.highlight] == "reps"){
//             if(fragmentData[1][2]<maxRepsVal)
//               fragmentData[1][2]++;
//           }
//           //changing min start
//           else if(activeMenu.options[activeMenu.highlight] == "entry"){
//             if(stepchild.buttons.SHIFT() && fragmentData[2][2]<stepchild.sequenceLength){
//               fragmentData[2][2]++;
//             }
//             else if(!stepchild.buttons.SHIFT() && fragmentData[2][2]<stepchild.sequenceLength-stepchild.subDivision){
//               fragmentData[2][2]+=stepchild.subDivision;
//             }
//           }
//           else if(activeMenu.options[activeMenu.highlight] == "state"){
//             isFragmenting = !isFragmenting;
//           }
//         }
//         else if(stepchild.buttons.counterA <= -1){
//           //changing max length
//           if(activeMenu.options[activeMenu.highlight] == "size"){
//             if(fragmentData[0][2]*fragmentSubDiv>=fragmentSubDiv){
//               fragmentData[0][2]--;
//             }
//           }
//           //changing min reps
//           else if(activeMenu.options[activeMenu.highlight] == "reps"){
//             if(fragmentData[1][2]>0)
//               fragmentData[1][2]--;
//           }
//           //changing min start
//           else if(activeMenu.options[activeMenu.highlight] == "entry"){
//             if(fragmentData[2][2]>0){
//               fragmentData[2][2]--;
//             }
//           }
//           else if(activeMenu.options[activeMenu.highlight] == "state"){
//             isFragmenting = !isFragmenting;
//           }
//         }
//         for(int i = 0; i<3; i++){
//           if(fragmentData[i][1]>fragmentData[i][2]){
//             fragmentData[i][1] = fragmentData[i][2];
//           }
//         }
//       }
//       stepchild.buttons.counterA += stepchild.buttons.counterA<0?1:-1;
//     }
//     //changing stepchild.subDivision
//     while(stepchild.buttons.counterB != 0 && activeMenu.options[activeMenu.highlight] == "div"){
//       if(stepchild.buttons.counterB <= -1 && !stepchild.buttons.SHIFT()){
//         changeFragmentSubDivInt(true);
//       }
//       //changing subdivint
//       if(stepchild.buttons.counterB >= 1 && !stepchild.buttons.SHIFT()){
//         changeFragmentSubDivInt(false);
//       }
//       //if shifting, toggle between 1/3 and 1/4 mode
//       else while(stepchild.buttons.counterB != 0 && stepchild.buttons.SHIFT()){
//         toggleFragmentTriplets();
//       }
//       stepchild.buttons.counterB += stepchild.buttons.counterB<0?1:-1;;
//     }
//     stepchild.display.clearDisplay();
//     int menuStart;
//     if(activeMenu.highlight>1){
//       menuStart = activeMenu.highlight - 1;
//     }
//     else if(activeMenu.highlight<=1){
//       menuStart = 0;
//     }
//     for(int i = menuStart; i<menuStart+3; i++){
//       if(i<activeMenu.options.size()){
//         stepchild.display.setCursor(4,2+(i-menuStart)*10);
//         if(i == activeMenu.highlight){
//           stepchild.display.drawRoundRect(2,1+(i-menuStart)*10,activeMenu.options[i].length()*6+3,10,4,SSD1306_WHITE);
//           // stepchild.display.setTextColor(SSD1306_BLACK);
//           stepchild.display.print(activeMenu.options[i]);
//           // stepchild.display.setTextColor(SSD1306_WHITE);
//         }
//         else{
//           stepchild.display.setTextColor(SSD1306_WHITE);
//           stepchild.display.print(activeMenu.options[i]);
//         }
//       }
//     }
//     //special displays for each menu option
//     if(activeMenu.options[activeMenu.highlight] == "entry"){
//       h = 64;
//       //display timeline when selecting the start
//       for(int lineStep = 0; lineStep<stepchild.SCREEN_WIDTH; lineStep++){
//         //if it's within the seq
//         if((lineStep+timeS)<=stepchild.sequenceLength&&(lineStep+timeS)>=0){
//           if(stepchild.playing()){
//             timeS = playheadPos - 64;
//             if(lineStep+timeS == playheadPos){
//               stepchild.display.drawRoundRect(lineStep, xStart-10, 3, 20, 3, SSD1306_WHITE);
//             }
//           }
//           //timeline is dotted
//           if((lineStep+timeS)%3){
//             stepchild.display.drawPixel(lineStep,xStart,SSD1306_WHITE);
//           }
//           //if it's on a subDiv
//           if(!((lineStep+timeS)%stepchild.subDivision)){
//             stepchild.display.drawLine(lineStep,xStart-1,lineStep,xStart+1,SSD1306_WHITE);
//           }
//           //if it's on a measure
//           if(!((lineStep+timeS)%96)){
//             stepchild.display.drawLine(lineStep,xStart-5,lineStep,xStart+5,SSD1306_WHITE);
//           }
//           //min fragment start value
//           if(lineStep+timeS == fragmentData[2][1]){
//             stepchild.display.fillTriangle(lineStep-3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
//           }
//           //max fragment start value
//           if(lineStep+timeS == fragmentData[2][2]){
//             stepchild.display.fillTriangle(lineStep+3,xStart,lineStep,xStart-3,lineStep,xStart+3,SSD1306_WHITE);
//           }
//           //in between the two vals
//           if(lineStep+timeS > fragmentData[2][1] && lineStep+timeS < fragmentData[2][2]){
//             stepchild.display.drawPixel(lineStep,xStart+3,SSD1306_WHITE);
//             stepchild.display.drawPixel(lineStep,xStart-3,SSD1306_WHITE);
//           }
//         }
//       }
//       graphics.printCursive(stepchild.SCREEN_WIDTH-30,2,"min",SSD1306_WHITE);
//       graphics.printCursive(stepchild.SCREEN_WIDTH-30,20,"max",SSD1306_WHITE);
//       stepchild.display.setCursor(stepchild.SCREEN_WIDTH-20-stepchild.stepsToPosition(fragmentData[0][1],false).length()*3,10);
//       stepchild.display.print(stepchild.stepsToPosition(fragmentData[2][1],false));
//       stepchild.display.setCursor(stepchild.SCREEN_WIDTH-20-stepchild.stepsToPosition(fragmentData[0][2],false).length()*3,28);
//       stepchild.display.print(stepchild.stepsToPosition(fragmentData[2][2],false));
//       stepchild.display.setCursor(5,35);
//       stepchild.display.print(stepchild.stepsToPosition(timeS+64,false));
//     }
//     else if(activeMenu.options[activeMenu.highlight] == "reps"){
//       graphics.printCursive(0,k+15,"minimum",SSD1306_WHITE);
//       graphics.printCursive(stepchild.SCREEN_WIDTH-42,k+15,"maximum",SSD1306_WHITE);
//       //drawing minimum
//       String temp = stringify(fragmentData[1][1]);
//       stepchild.display.setCursor(21-temp.length()*3,k+25);
//       stepchild.display.print(temp);
//       //drawing maximum
//       temp = stringify(fragmentData[1][2]);
//       stepchild.display.setCursor(stepchild.SCREEN_WIDTH-21-temp.length()*3,k+25);
//       stepchild.display.print(temp);

//       if(fragmentData[1][2]>1 && isFragmenting){
//         int reps = random(fragmentData[1][1],fragmentData[1][2]);
//         if(reps == 1){
//           graphics.drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//         }
//         else if(reps == 2){
//           graphics.drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//         }
//         else if(reps == 3){
//           graphics.drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//         }
//         else if(reps>3){
//           graphics.drawTetra(h+40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h-40+cos(millis()),k-5+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h+40+cos(millis()),k+10+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//           graphics.drawTetra(h-40+cos(millis()),k+10+sin(millis()),height*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//         }
//       }
//     }
//     else if(activeMenu.options[activeMenu.highlight] == "size"){
//       drawBracket(h,k+10,8,fragmentData[0][2]*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal)-1,0,SSD1306_WHITE);
//       drawBracket(h,13,4,fragmentData[0][1]*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal)-1,1,SSD1306_WHITE);
//       graphics.printCursive(0,k+15,"minimum",SSD1306_WHITE);
//       graphics.printCursive(stepchild.SCREEN_WIDTH-42,k+15,"maximum",SSD1306_WHITE);
//       //drawing minimum
//       String temp = "("+stringify(fragmentData[0][1])+")x("+stepchild.stepsToMeasures(fragmentSubDiv)+")";
//       stepchild.display.setCursor(0,k+25);
//       stepchild.display.print(temp);
//       //drawing maximum
//       temp = "("+stringify(fragmentData[0][2])+")x("+stepchild.stepsToMeasures(fragmentSubDiv)+")";
//       stepchild.display.setCursor(stepchild.SCREEN_WIDTH-temp.length()*6,k+25);
//       stepchild.display.print(temp);
//     }
//     else if(activeMenu.options[activeMenu.highlight] == "state"){
//       stepchild.display.setFont(&FreeSerifItalic9pt7b);
//       stepchild.display.setCursor(5,50);
//       if(isFragmenting){
//         stepchild.display.print("on");
//         // graphics.printCursive(5,50,"on",SSD1306_WHITE);
//       }
//       else{
//         // graphics.printCursive(5,50,"off",SSD1306_WHITE);
//         stepchild.display.print("off");
//       }
//       stepchild.display.setFont();
//     }
//     else if(activeMenu.options[activeMenu.highlight] == "div"){
//       stepchild.display.setTextSize(2);
//       stepchild.display.setCursor(2,48);
//       stepchild.display.print(stepchild.stepsToMeasures(fragmentSubDiv));
//       stepchild.display.setTextSize(1);
//     }
//     if(isFragmenting){
//       width = random(fragmentData[0][1],fragmentData[0][2]);
//       height = random(1,4);
//     }
//     else{
//       width = fragmentData[0][2];
//       height = 3;
//     }
//     // xDepth = random(-10,10);
//     // yDepth = random(-10,10);
//     //animated if the sequence is fragmenting
//     if(!stepchild.playing()&&!stepchild.recording()){
//       graphics.drawTetra(h+cos(millis()),k+sin(millis()),height*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxRepsVal),width*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(maxLengthVal),xDepth,yDepth,0,SSD1306_WHITE);
//     }
//     else{
//       graphics.drawTetra(h+cos(millis()),k+sin(millis()),fragmentData[1][0]*float(stepchild.SCREEN_HEIGHT)/float(fragmentData[1][2]),fragmentData[0][0]*fragmentSubDiv*float(stepchild.SCREEN_WIDTH)/float(fragmentData[0][2]),xDepth,yDepth,0,SSD1306_WHITE);
//     }
//     stepchild.display.display();
//   }
//   fragmentAnimation(false);
// }