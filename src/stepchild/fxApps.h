/* 
   ======================================
            FX
   ======================================

    Basically, this file catches all undefined fx application functions, texts, and icons
    and defines them to the default data.
    
    It also contains the code for the FX Menu

*/

// 'echo', 12x12px
const unsigned char echo_bmp []  = {
	0x1f, 0x80, 0x20, 0x40, 0x4f, 0x20, 0x90, 0x90, 0xa6, 0x50, 0xa9, 0x50, 0xa9, 0x50, 0xa6, 0x50, 
	0x90, 0x90, 0x4f, 0x20, 0x20, 0x40, 0x1f, 0x80
};
// 'humanize', 12x12px
//(same as heart)
const unsigned char humanize_bmp []  = {
	0x79, 0xe0, 0x86, 0x10, 0xb0, 0x10, 0xa0, 0x10, 0x80, 0x10, 0x80, 0x10, 0x80, 0x10, 0x48, 0x20, 
	0x24, 0x40, 0x10, 0x80, 0x09, 0x00, 0x06, 0x00
};
// 'mayhem', 12x12px
const unsigned char mayhem_bmp []  = {
	0x7f, 0xe0, 0xff, 0xf0, 0xff, 0xf0, 0xcf, 0x30, 0x86, 0x10, 0xcf, 0x30, 0xff, 0xf0, 0x79, 0xe0, 
	0x7f, 0xe0, 0x1f, 0x80, 0x16, 0x80, 0x1f, 0x80
};
// 'quant', 12x12px
const unsigned char quantize_bmp []  = {
	0xaa, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8f, 0xf0, 0x08, 0x10, 0x88, 0x90, 0x08, 0x10, 
	0xaa, 0x90, 0x08, 0x10, 0x08, 0x10, 0x0f, 0xf0
};
// 'reverse', 12x12px
const unsigned char reverse_bmp []  = {
	0x70, 0x80, 0x78, 0xc0, 0x3f, 0xe0, 0x1f, 0xf0, 0x07, 0xe0, 0x10, 0xc0, 0x30, 0x80, 0x7e, 0x00, 
	0xff, 0x80, 0x7f, 0xc0, 0x31, 0xe0, 0x10, 0xe0
};

// 'splinter', 12x12px
const unsigned char splinter_bmp []  = {
	0x04, 0x70, 0x24, 0xf0, 0x11, 0xf0, 0x03, 0xe0, 0x61, 0xc0, 0x00, 0x80, 0x18, 0x30, 0x3c, 0x00, 
	0x7c, 0x40, 0xf9, 0x20, 0xf1, 0x00, 0xe0, 0x00
};
// 'scramble2', 12x12px
const unsigned char scramble_bmp []  = {
	0x06, 0x00, 0x0f, 0x00, 0x17, 0x80, 0x27, 0xc0, 0x2f, 0xc0, 0x5f, 0xe0, 0x7f, 0xe0, 0x7f, 0xe0, 
	0x7f, 0xe0, 0x3f, 0xc0, 0x1f, 0x80, 0x0f, 0x00
};
// 'strum', 12x12px
const unsigned char strum_bmp []  = {
	0xc0, 0x00, 0xe0, 0x00, 0x30, 0x00, 0x1b, 0x00, 0x0f, 0x80, 0x1d, 0x80, 0x1f, 0xe0, 0x07, 0x30, 
	0x63, 0x10, 0x43, 0xb0, 0xc1, 0xe0, 0xc0, 0x00
};
// 'chop', 12x12px
const unsigned char chop_bmp []  = {
	0x08, 0x30, 0x88, 0x70, 0x41, 0xe0, 0x06, 0xc0, 0x18, 0x80, 0x20, 0x80, 0x28, 0x80, 0x20, 0x80, 
	0x21, 0x30, 0x26, 0x00, 0x28, 0x80, 0x30, 0x40
};
// 'warp', 12x12px
const unsigned char warp_bmp []  = {
	0x00, 0x00, 0x08, 0x00, 0x0a, 0x00, 0x2a, 0x20, 0x2a, 0xa0, 0xaa, 0xa0, 0xaa, 0xa0, 0x2a, 0xa0, 
	0x2a, 0x20, 0x0a, 0x00, 0x08, 0x00, 0x00, 0x00
};

//TBD, because they have sick little icons
/*
  epd_bitmap_scramble,
  epd_bitmap_splinter,
  epd_bitmap_chop,
  epd_bitmap_mayhem,
*/

//Default ChildOS FX applications

#define FX_APP1_ICON quantize_bmp
#define FX_APP2_ICON humanize_bmp
#define FX_APP3_ICON strum_bmp
#define FX_APP4_ICON echo_bmp
#define FX_APP5_ICON reverse_bmp
#define FX_APP6_ICON warp_bmp

#define FX_APP1_FUNCTION quantizeMenu
#define FX_APP2_FUNCTION humanizeMenu
#define FX_APP3_FUNCTION strumMenu
#define FX_APP4_FUNCTION echoMenu
#define FX_APP5_FUNCTION reverse
#define FX_APP6_FUNCTION warp

#define FX_APP1_TEXT "quantize"
#define FX_APP2_TEXT "humanize"
#define FX_APP3_TEXT "strum"
#define FX_APP4_TEXT "echo"
#define FX_APP5_TEXT "reverse"
#define FX_APP6_TEXT "warp"

/*
    Check to see if the user has defined these in their code!
    If not, set them to a default function or bitmap
*/

//Icons
#ifndef FX_APP6_ICON
    #define FX_APP6_ICON default_bmp
#endif
#ifndef FX_APP7_ICON
    #define FX_APP7_ICON default_bmp
#endif
#ifndef FX_APP8_ICON
    #define FX_APP8_ICON default_bmp
#endif
#ifndef FX_APP9_ICON
    #define FX_APP9_ICON default_bmp
#endif
#ifndef FX_APP10_ICON
    #define FX_APP10_ICON default_bmp
#endif
#ifndef FX_APP11_ICON
    #define FX_APP11_ICON default_bmp
#endif
#ifndef FX_APP12_ICON
    #define FX_APP12_ICON default_bmp
#endif
#ifndef FX_APP13_ICON
    #define FX_APP13_ICON default_bmp
#endif
#ifndef FX_APP14_ICON
    #define FX_APP14_ICON default_bmp
#endif
#ifndef FX_APP15_ICON
    #define FX_APP15_ICON default_bmp
#endif
#ifndef FX_APP16_ICON
    #define FX_APP16_ICON default_bmp
#endif
#ifndef FX_APP17_ICON
    #define FX_APP17_ICON default_bmp
#endif
#ifndef FX_APP18_ICON
    #define FX_APP18_ICON default_bmp
#endif
#ifndef FX_APP19_ICON
    #define FX_APP19_ICON default_bmp
#endif
#ifndef FX_APP20_ICON
    #define FX_APP20_ICON default_bmp
#endif
#ifndef FX_APP21_ICON
    #define FX_APP21_ICON default_bmp
#endif
#ifndef FX_APP22_ICON
    #define FX_APP22_ICON default_bmp
#endif
#ifndef FX_APP23_ICON
    #define FX_APP23_ICON default_bmp
#endif
#ifndef FX_APP24_ICON
    #define FX_APP24_ICON default_bmp
#endif

//Functions
#ifndef FX_APP6_FUNCTION
    #define FX_APP6_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP7_FUNCTION
    #define FX_APP7_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP8_FUNCTION
    #define FX_APP8_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP9_FUNCTION
    #define FX_APP9_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP10_FUNCTION
    #define FX_APP10_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP11_FUNCTION
    #define FX_APP11_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP12_FUNCTION
    #define FX_APP12_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP13_FUNCTION
    #define FX_APP13_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP14_FUNCTION
    #define FX_APP14_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP15_FUNCTION
    #define FX_APP15_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP16_FUNCTION
    #define FX_APP16_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP17_FUNCTION
    #define FX_APP17_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP18_FUNCTION
    #define FX_APP18_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP19_FUNCTION
    #define FX_APP19_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP20_FUNCTION
    #define FX_APP20_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP21_FUNCTION
    #define FX_APP21_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP22_FUNCTION
    #define FX_APP22_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP23_FUNCTION
    #define FX_APP23_FUNCTION defaultAppFunction
#endif
#ifndef FX_APP24_FUNCTION
    #define FX_APP24_FUNCTION defaultAppFunction
#endif

//Texts
#ifndef FX_APP6_TEXT
    #define FX_APP6_TEXT ""
#endif
#ifndef FX_APP7_TEXT
    #define FX_APP7_TEXT ""
#endif
#ifndef FX_APP8_TEXT
    #define FX_APP8_TEXT ""
#endif
#ifndef FX_APP9_TEXT
    #define FX_APP9_TEXT ""
#endif
#ifndef FX_APP10_TEXT
    #define FX_APP10_TEXT ""
#endif
#ifndef FX_APP11_TEXT
    #define FX_APP11_TEXT ""
#endif
#ifndef FX_APP12_TEXT
    #define FX_APP12_TEXT ""
#endif
#ifndef FX_APP13_TEXT
    #define FX_APP13_TEXT ""
#endif
#ifndef FX_APP14_TEXT
    #define FX_APP14_TEXT ""
#endif
#ifndef FX_APP15_TEXT
    #define FX_APP15_TEXT ""
#endif
#ifndef FX_APP16_TEXT
    #define FX_APP16_TEXT ""
#endif
#ifndef FX_APP17_TEXT
    #define FX_APP17_TEXT ""
#endif
#ifndef FX_APP18_TEXT
    #define FX_APP18_TEXT ""
#endif
#ifndef FX_APP19_TEXT
    #define FX_APP19_TEXT ""
#endif
#ifndef FX_APP20_TEXT
    #define FX_APP20_TEXT ""
#endif
#ifndef FX_APP21_TEXT
    #define FX_APP21_TEXT ""
#endif
#ifndef FX_APP22_TEXT
    #define FX_APP22_TEXT ""
#endif
#ifndef FX_APP23_TEXT
    #define FX_APP23_TEXT ""
#endif
#ifndef FX_APP24_TEXT
    #define FX_APP24_TEXT ""
#endif

const unsigned char* fxApplicationIcons[24] = {
    FX_APP1_ICON,
    FX_APP2_ICON,
    FX_APP3_ICON,
    FX_APP4_ICON,
    FX_APP5_ICON,
    FX_APP6_ICON,
    FX_APP7_ICON,
    FX_APP8_ICON,
    FX_APP9_ICON,
    FX_APP10_ICON,
    FX_APP11_ICON,
    FX_APP12_ICON,
    FX_APP13_ICON,
    FX_APP14_ICON,
    FX_APP15_ICON,
    FX_APP16_ICON,
    FX_APP17_ICON,
    FX_APP18_ICON,
    FX_APP19_ICON,
    FX_APP20_ICON,
    FX_APP21_ICON,
    FX_APP22_ICON,
    FX_APP23_ICON,
    FX_APP24_ICON
};

//loading all the functions
void (*const fxApplicationFunctions[24])() = {
    FX_APP1_FUNCTION,
    FX_APP2_FUNCTION,
    FX_APP3_FUNCTION,
    FX_APP4_FUNCTION,
    FX_APP5_FUNCTION,
    FX_APP6_FUNCTION,
    FX_APP7_FUNCTION,
    FX_APP8_FUNCTION,
    FX_APP9_FUNCTION,
    FX_APP10_FUNCTION,
    FX_APP11_FUNCTION,
    FX_APP12_FUNCTION,
    FX_APP13_FUNCTION,
    FX_APP14_FUNCTION,
    FX_APP15_FUNCTION,
    FX_APP16_FUNCTION,
    FX_APP17_FUNCTION,
    FX_APP18_FUNCTION,
    FX_APP19_FUNCTION,
    FX_APP20_FUNCTION,
    FX_APP21_FUNCTION,
    FX_APP22_FUNCTION,
    FX_APP23_FUNCTION,
    FX_APP24_FUNCTION
};

const String fxApplicationTexts[24] = {
    FX_APP1_TEXT,
    FX_APP2_TEXT,
    FX_APP3_TEXT,
    FX_APP4_TEXT,
    FX_APP5_TEXT,
    FX_APP6_TEXT,
    FX_APP7_TEXT,
    FX_APP8_TEXT,
    FX_APP9_TEXT,
    FX_APP10_TEXT,
    FX_APP11_TEXT,
    FX_APP12_TEXT,
    FX_APP13_TEXT,
    FX_APP14_TEXT,
    FX_APP15_TEXT,
    FX_APP16_TEXT,
    FX_APP17_TEXT,
    FX_APP18_TEXT,
    FX_APP19_TEXT,
    FX_APP20_TEXT,
    FX_APP21_TEXT,
    FX_APP22_TEXT,
    FX_APP23_TEXT,
    FX_APP24_TEXT
};

bool fxMenuControls(){
  if(itsbeen(100)){
    if(x != 0){
      if(x == -1){
        //if it's not divisible by four (in which case this would be 0)
        if((activeMenu.highlight+1)%4)
          activeMenu.highlight++;
        lastTime = millis();
      }
      else if(x == 1){
        if((activeMenu.highlight)%4)
          activeMenu.highlight--;
        lastTime = millis();
      }
    }
    if(y != 0){
      if(y == 1){
        //first row
        if(activeMenu.highlight < 20)
          activeMenu.highlight += 4;
        lastTime = millis();
      }
      else if(y == -1){
        //second row
        if(activeMenu.highlight > 3)
          activeMenu.highlight -= 4;
        lastTime = millis();
      }
      activeMenu.page = activeMenu.highlight/12;
    }
  }
  if(itsbeen(200)){
    if(menu_Press){
      lastTime = millis();
      slideMenuOut(0,20);
      return false;
    }
    if(sel){
      sel = false;
      lastTime = millis();
      fxApplicationFunctions[activeMenu.highlight]();
    }
  }
  return true;
}

void drawFxLabel(){
  String text = fxApplicationTexts[activeMenu.highlight];
  printChunky(activeMenu.coords.x1+activeMenu.coords.y1+38,4,text,SSD1306_WHITE);
}

void fxMenu(){
  int angleX = 0;
  int angleY = 0;
  int angleZ = 0;

  int angle2X = 0;
  int angle2Y = 0;
  int angle2Z = 0;

  WireFrame icon = makeGyro(angleX,angleY,angleZ,
                  angle2X,angle2Y,angle2Z);
  while(menuIsActive && activeMenu.menuTitle == "FX"){
    display.clearDisplay();
    drawSeq(true,false,true,false,false);
    drawPram(5,0);
    display.fillCircle(111,13,23,0);
    display.drawCircle(111,13,23,1);
    activeMenu.displayFxMenu();
    icon.render();
    printItalic(103,13,"FX",1);
    display.display();
    readJoystick();
    readButtons();
    if(!fxMenuControls()){
      break;
    }
    angleX+=2;
    angleY-=2;
    // angleZ-=2;
    angleX%=360;
    angleY%=360;
    angleZ%=360;
    angle2X+=2;
    // angle2Y+=2;
    angle2Z+=2;
    angle2X%=360;
    angle2Y%=360;
    angle2Z%=360;
    icon = makeGyro(angleX,angleY,angleZ,
                    angle2X,angle2Y,angle2Z);
  }
  if(menuIsActive)
    constructMenu("MENU");
}

void Menu::displayFxMenu(){
  //drawing menu box (+16 so the title is transparent)
  display.fillRect(coords.x1,coords.y1+13, coords.x2-coords.x1, coords.y2-coords.y1, SSD1306_BLACK);
  display.drawRect(coords.x1,coords.y1+12, coords.x2-coords.x1, coords.y2-coords.y1-12, SSD1306_WHITE);

  //if the title will be on screen
  if(coords.x1+coords.y1-1<screenWidth){
    display.fillRect(coords.x1+coords.y1-1,0,18,13,0);
    display.setCursor(coords.x1+coords.y1-1,5);
    display.setFont(&FreeSerifItalic9pt7b);
    display.print("Fx");
    display.setFont();
  }
  
  drawFxLabel();
  //printing page number/arrows
  printSmall(coords.x1+coords.y1+21,2,stringify(page+1)+"/2",SSD1306_WHITE);
  if(page == 0){
    drawArrow(coords.x1+coords.y1+26,11+((millis()/400)%2),2,3,true);
  }
  else{
    drawArrow(coords.x1+coords.y1+26,9+((millis()/400)%2),2,2,true);
  }

  const uint8_t width = 16;
  uint8_t count = 0;
  for(uint8_t j = 0; j<3; j++){
    for(uint8_t i = 0; i<4; i++){
      display.drawBitmap(coords.x1+4+width*i,coords.y1+j*(width-1)+17+sin(millis()/200+count),fxApplicationIcons[page*12+count],12,12,SSD1306_WHITE);
      if(i+4*j == highlight-12*page){
        display.drawRoundRect(coords.x1+2+width*i,coords.y1+j*(width-1)+15+sin(millis()/200+count),width,width,3,SSD1306_WHITE);
      }
      count++;
    }
  }
}




