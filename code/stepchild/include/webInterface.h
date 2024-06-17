class Console{
    public:
        vector<String> text;
        const uint8_t maxLines = 10;
        Console(){}
        void log(String s){
            text.push_back(s);
            if(text.size()>maxLines){
                text.erase(text.begin());
            }
        }
        void draw(uint8_t x1, uint8_t y1){
            for(uint8_t i = 0; i<text.size(); i++){
                printSmall(x1,y1+i*6,text[i],1);
            }
        }
        void clear(){
            text.erase(text.begin(),text.end());
        }
};

class WebInterface{
    public:
        enum WebInterfaceCommand:uint8_t {
            DUMP_FILESYSTEM,
            DUMP_SETTINGS,
            ENTER_BOOTSEL,
            ENABLE_SCREENCAPTURE,
            DISABLE_SCREENCAPTURE,
            DOWNLOAD_FILE,
            SEND_NEXT_FILE_PLEASE,
            SEND_FIRMWARE_VERSION,
            RESET_SYSTEM,
            EXIT_INTERFACE,
            START_INTERFACE,
            SEND_FILE_COUNT
        };

        Console console;
        bool connected = false;
        WebInterface(){
            console.log("hello, child");
        }
        bool isConnectedToUSB();
        void drawInterface();
        void dumpFileSystem();
        void dumpSettings();
        void downloadFile();
        void parseCommand(uint8_t);
        void parseCommand(WebInterfaceCommand);
        bool interfaceControls();
        void sendFileCount();
};

bool WebInterface::isConnectedToUSB(){
    return isConnectedToUSBPower();
}

bool WebInterface::interfaceControls(){
    controls.readButtons();
    controls.readJoystick();
    if(utils.itsbeen(200)){
        if(controls.MENU()){
            lastTime = millis();
            return false;
        }
        if(controls.DELETE()){
            lastTime = millis();
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
    Dir saves = LittleFS.openDir("/SAVES");
    while(saves.next()){
        fileCount++;
    }
    saves.rewind();
    return fileCount;
}
void WebInterface::sendFileCount(){
    uint8_t fCount = getFileCount();
    console.log("sent file count ("+stringify(fCount)+")");
    Serial.write(fCount);
}

void WebInterface::dumpSettings(){
}

//just writes all the file data to the serial buffer
void writeFileToSerial(File f){
    uint32_t fileSize = f.size();
    uint8_t byteCount[4] = {uint8_t(fileSize>>24),uint8_t(fileSize>>16),uint8_t(fileSize>>8),uint8_t(fileSize)};
    Serial.write(byteCount,4);//write the filesize
    // Serial.write(fileSize);
    char data[fileSize];
    f.readBytes(data,fileSize);//read the file into a buffer
    Serial.write(data,fileSize);//write the buffer to the serial buffer
    Serial.flush();//wait for data to be sent
}

void WebInterface::dumpFileSystem(){
    if(LittleFS.exists("/SAVES")){
        Dir saves = LittleFS.openDir("/SAVES");
        while(saves.next()){
            String fname = saves.fileName();
            console.log("dumping '"+fname+"'...");//add message to console

            Serial.print(fname+".child\n");//send filename followed by /n
            File f = saves.openFile("r");
            writeFileToSerial(f);//write the file
            f.close();//close the file

            //update display
            console.log("sent!");
            drawInterface();
        }
    }
}

void WebInterface::drawInterface(){
    display.clearDisplay();
    console.draw(0,0);
    display.display();
}

void webInterface(){
    LittleFS.begin();
    WebInterface interface;
    while(interface.interfaceControls()){
        interface.drawInterface();
        if(Serial.available()>0){
            interface.parseCommand(Serial.read());
        }
    }
    LittleFS.end();
}

