#include "Stepchild.h"

class Console{
    private:
        std::vector<String> text;
        const uint8_t maxLines = 10;
        uint8_t currentChar = 0;
    public:
        Console(){}
        void log(String s);
        void draw(uint8_t x1, uint8_t y1);
        void clear();
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
        WebInterface();
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