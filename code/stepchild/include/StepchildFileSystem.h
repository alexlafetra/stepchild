#pragma once
#include <LittleFS.h>
#include <vector>

class Stepchild;   // forward declaration

class StepchildFileSystem{
  public:
    /*
      Sequence files are very simple! They are stored as:
      byte listing how many sections there are ("N")
      "N" bytes for each section, listing the order of each section
    */

    String currentFilename = "";
    Stepchild* stepchild;

    //Enum to organize the header of each file
    enum FileFormatCode:uint8_t {
      BLANK,//filler
      //Saving
      SEQ_DATA,
      TRACK_AND_NOTE_DATA,
      AUTOTRACK_DATA,
      LOOP_DATA,
      CLOCK_DATA,
      MIDI_PORT_DATA,
      PC_DATA,

      //Settings
      TRACK_DISPLAY,
      LEDS_ACTIVE,
      CLOCK_SOURCE,
      REC_MODE,
      REC_TRIGGER,
      SLEEP_TIME,
      SCREEN_BRIGHTNESS
    };

    const FileFormatCode sequenceFileHeader[7] = {
      SEQ_DATA,
      TRACK_AND_NOTE_DATA,
      AUTOTRACK_DATA,
      LOOP_DATA,
      CLOCK_DATA,
      MIDI_PORT_DATA,
      PC_DATA
    };

    const FileFormatCode settingsFileHeader[7] = {
      TRACK_DISPLAY,
      LEDS_ACTIVE,
      CLOCK_SOURCE,
      REC_MODE,
      REC_TRIGGER,
      SLEEP_TIME,
      SCREEN_BRIGHTNESS
    };

    StepchildFileSystem(Stepchild* ptr);
    void writeFileHeader(File& f);
    void writeCurrentFileFormatToSerial();
    //lmfao
    void writeFileFormatFromFileToSerial(File& f);
    //sends the number of bytes in the file
    void sendByteCount(String filename);
    void sendFileName(String filename);
    //returns Bytes of a file (must be called within LittleFS)
    uint32_t getByteCount(String filename);
    uint32_t getByteCount_standAlone(String filename);
    void writeSeqFile(String filename);
    void writeCurrentSeqToSerial(bool waitForResponse);
    void loadSeqFile(String filename);
    //loads files into menu, for browsing
    //also returns each file's filesize
    std::vector<String> loadFilesAndSizes();
    std::vector<String> loadFiles();
    void renameSeqFile(String filename);
    bool deleteSeqFile(String filename);
    void quickSave();
    void duplicateSeqFile(String filename);
    void writeCurrentSettingsToFile();
    void loadSavedSettingsFromFile();
    //loads settings from settings.txt
    void loadSettings();
    //formats a number of bytes into a XXXB or XXXkB string
    String bytesToString(uint32_t bytes);
    void writeFileToSerial(File f);

};
