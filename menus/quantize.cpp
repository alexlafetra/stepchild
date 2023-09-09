void drawSubDivBackground(bool swinging){
  for(int16_t i = 0; i<96; i+=subDivInt){
    if(swinging){
      float offset = sin(2*PI/swingSubDiv * (i-swingSubDiv/4));
      if(!(offset+i<i && subDivInt == 1)){
        i+= offset;
      }
    }
    display.drawFastVLine(float(i)/float(96)*screenWidth,16,32,1);
  }
}
void drawQuantCubes(uint8_t distance){
  if(distance == 1){
    distance++;
  }
  const uint8_t width = 15;
  // uint8_t hOffset = 2*sin(millis()/150);
  const uint8_t hOffset = 2;
  if(distance == 0){
    drawBox(47+distance/2,hOffset+16-distance/2,width+1,width+1,12,12,4);
    drawBox(48+distance/2,hOffset+17-distance/2,width,width,11,11,3);
  }
  else{
    //bottomL
    // drawBox(48-distance/2,hOffset+12+distance/2,width,width,11,11,2);
    drawBox(48-distance/2,hOffset+16+distance/2,width,width,12,12,7);

    //inverse wirewrame (topR)
    drawBox(48+distance/2,hOffset+16-distance/2,width,width,12,12,7);

    //inner lines
    // drawBox(48-distance/2,hOffset+12+distance/2,width,width,12,12,6);

    //top line
    drawDottedLineDiagonal(48-distance/2,hOffset+16+distance/2,48+distance/2,hOffset+16-distance/2,2);
    //bottom line
    drawDottedLineDiagonal(48-distance/2+width+14,hOffset+16+distance/2+width+10,48+distance/2+width+13,hOffset+16-distance/2+width+11,2);
  }
}

void quantizeMenu(){
  uint8_t whichParam = 0;
  while(true){
    display.clearDisplay();

    //draw quant amount
    String q = stringify(quantizeAmount);
    while(q.length()<3){
      q = "0"+q;
    }
    print7SegSmall(90,53,q+"%",1);
    //draw subDiv
    q = stepsToMeasures(subDivInt);
    printSmall(28,6,q,1);

    //draw swing indicator
    printSmall(90,6,quantizeWithSwing?"swung":"unswung",1);

    if(whichParam == 0){
      drawArrow(96+16+2*sin(millis()/200),56,2,1,true);
      printSmall(90,46,"amount",1);
    }
    else{
      drawArrow(24+2*sin(millis()/200),8,2,0,true);
      printSmall(0,6," grid",1);
      //bracket
      drawSubDivBackground(quantizeWithSwing);
    }

    //graphic
    drawQuantCubes((100-quantizeAmount)/(5));

    display.display();

    joyRead();
    readButtons();
    if(!quantizeMenuControls(&whichParam)){
      break;
    }
  }
}