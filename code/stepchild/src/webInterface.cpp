#include "webInterface.h"
#include "Stepchild.h"

using namespace std;

void Console::log(String s){
    text.push_back(s);
    if(text.size()>maxLines){
        text.erase(text.begin());
    }
    currentChar = 0;
}
void Console::draw(uint8_t x1, uint8_t y1){
    for(uint8_t i = 0; i<text.size()-1; i++){
        graphics.printSmall(x1,y1+i*6,text[i],1);
    }
    graphics.printSmall(x1,y1+(text.size()-1)*6,text[text.size()-1].substring(0,currentChar),1);
    if(currentChar<text[text.size()-1].length())
        currentChar++;
    //blinking cursor
    if((millis()/400)%2)
        stepchild.display.fillRect(x1+(currentChar)*4+1,y1+(text.size()-1)*6,3,5,1);
}
void Console::clear(){
    text.erase(text.begin(),text.end());
}

WebInterface::WebInterface(){
    console.log("hello, child");
}
bool WebInterface::isConnectedToUSB(){
    return isConnectedToUSBPower();
}

bool WebInterface::interfaceControls(){
    stepchild.buttons.readButtons();
    stepchild.buttons.readJoystick();
    if(stepchild.itsbeen(200)){
        if(stepchild.buttons.MENU()){
            stepchild.lastTime = millis();
            return false;
        }
        if(stepchild.buttons.DELETE()){
            stepchild.lastTime = millis();
            console.clear();
        }
    }
    return true;
}

void WebInterface::parseCommand(uint8_t cmd){
    parseCommand(static_cast<WebInterfaceCommand>(cmd));
}

void WebInterface::parseCommand(WebInterfaceCommand command){
    //clear the serial buffer (is the clearing the input or the output...?)
    // Serial.flush();
    switch(command){
        case START_INTERFACE:
            connected = true;
            console.log("connected to web!");
            break;
        case DUMP_FILESYSTEM:
            console.log("dumping filesystem!");
            dumpFileSystem();
            break;
        case DUMP_SETTINGS:
            dumpSettings();
            break;
        case ENTER_BOOTSEL:
            console.log("entering bootsel mode!");
            enterBootsel();
            break;
        case ENABLE_SCREENCAPTURE:
            break;
        case DISABLE_SCREENCAPTURE:
            break;
        case DOWNLOAD_FILE:
            break;
        case SEND_NEXT_FILE_PLEASE:
            break;
        case SEND_FIRMWARE_VERSION:
            break;
        case SEND_FILE_COUNT:
            sendFileCount();
            break;
    }
}

uint8_t getFileCount(){
    uint8_t fileCount = 0;
    #ifndef HEADLESS
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
        fileCount++;
    }
    saves.rewind();
    #endif
    return fileCount;
}
void WebInterface::sendFileCount(){
    uint8_t fCount = getFileCount();
    console.log("sent file count ("+stringify(fCount)+")");
    Serial.write(fCount);
}

void WebInterface::dumpSettings(){
}

void WebInterface::dumpFileSystem(){
    #ifndef HEADLESS
    if(LittleFS.exists("/SAVES")){
        Dir saves = LittleFS.openDir("/SAVES");
        while(saves.next()){
            String fname = saves.fileName();
            console.log("dumping '"+fname+"'...");//add message to console

            Serial.print(fname+".child\n");//send filename followed by /n
            File f = saves.openFile("r");
            stepchild.filesystem.writeFileToSerial(f);//write the file
            f.close();//close the file

            //update display
            console.log("sent!");
            drawInterface();
        }
    }
    #endif
}

void WebInterface::drawInterface(){
    stepchild.display.clearDisplay();
    console.draw(0,0);
    stepchild.display.display();
}

void webInterface(){
    #ifndef HEADLESS
    LittleFS.begin();
    #endif
    WebInterface interface;
    while(interface.interfaceControls()){
        interface.drawInterface();
        if(Serial.available()>0){
            interface.parseCommand(Serial.read());
        }
    }
    #ifndef HEADLESS
    LittleFS.end();
    #endif
}

