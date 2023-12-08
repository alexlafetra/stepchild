/*
Serial commands that the stepchild responds to:
(BYTE - BYTE - BYTE)
0 0 0 -- exit
1 0 X -- send sequence file X
2 0 0 -- send all sequence files
3 0 0 -- send file names
4 0 0 -- get ready to download a sequence file
255 255 255 -- null command
*/

const uint8_t maxConsoleLines = 5;
class ConsoleLog{
    public:
        vector<String> text;
        ConsoleLog(){
            text.push_back("waiting for response...");
        }
        void addLine(String a){
            if(text.size()>=maxConsoleLines){
                for(uint8_t i = 1; i<maxConsoleLines; i++){
                    text[i-1] = text[i]; 
                }
                text[maxConsoleLines-1] = a;
            }
            else{
                text.push_back(a);
            }
        }
        void print(uint8_t x1, uint8_t y1){
            for(uint8_t i = 0; i<text.size(); i++){
                // printSmall_overflow(x1,y1+7*i,128,text[i],1);
                printSmall(x1,y1+7*i,text[i],1);
                //adjusting line height if overflow
                y1+=(7*(text[i].length()+x1)/128);
            }
        }
};
#ifndef HEADLESS
void downloadFile(){
    uint8_t filesize[4] = {0,0,0,0};
    Serial.readBytes(filesize,4);
    uint32_t byteCount = (filesize[0]<<24)|(filesize[1]<<16)|(filesize[2]<<8)|(filesize[3]);
    uint8_t data[byteCount];
    Serial.readBytes(data,byteCount);
    if(byteCount>0){
        String filename = "test";
        LittleFS.begin();
        String path = "/SAVES/"+filename;
        File newFile = LittleFS.open(path,"w");
        newFile.write(data,byteCount);
        LittleFS.end();
    }
}

String getFilenameFromIndex(uint8_t index){
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    uint8_t i = 0;
    while(saves.next()){
        if(i == index){
            return saves.fileName();
        }
        i++;
    }
  }
  LittleFS.end();
  return "Couldn't get filename from index";
}

void sendFileNames(){
  LittleFS.begin();
  if(LittleFS.exists("/SAVES")){
    Dir saves = LittleFS.openDir("/SAVES");
    vector<String> filenames;
    while(saves.next()){
        filenames.push_back(saves.fileName()+"\n");
    }
    Serial.write(uint8_t(filenames.size()));
    for(uint8_t i = 0; i<filenames.size(); i++){
        Serial.print(filenames[i]);
    }
  }
  LittleFS.end();
}
#else
void sendFileNames(){
}
String getFilenameFromIndex(uint8_t i){
    return "error! headless";
}
void downloadFile(){
}
#endif

//loop that reads serial input
void webInterface(){
    uint8_t command[3] = {255,255,255};
    ConsoleLog console = ConsoleLog();
    while(true){
        display.clearDisplay();
        drawLogo(0,10);
        console.print(6,24);
        display.display();
        readButtons();
        if(itsbeen(200)){
            if(menu_Press){
                lastTime = millis();
                return;
            }
        }
        //if there are at least 3 bytes to be read
        if(Serial.available()>=3){
            Serial.readBytes(command,3);
            switch(command[0]){
                //exit command
                case 0:{
                    return;
                }
                //send file X command
                case 1:{
                    String fName = getFilenameFromIndex(command[2]);
                    console.addLine("sending "+fName+"...");
                    exportSeqFileToSerial_standAlone(fName);
                    console.addLine("sent "+stringify(getByteCount_standAlone(fName))+" bytes!");
                    break;
                }
                //send all files command
                case 2:{
                    dumpFilesToSerial();
                    break;
                }
                // send file names
                case 3:{
                    sendFileNames();
                    break;
                }
                // get ready to download
                case 4:{
                    downloadFile();
                    break;
                }
            }
            command[0] = 255;
            command[1] = 255;
            command[2] = 255;
        }
    }
}
