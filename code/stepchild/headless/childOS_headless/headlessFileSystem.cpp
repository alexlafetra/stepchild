#include <string>
#include <vector>
#include "headlessFileSystem.h"
using namespace std;
void flashTest(){
  return;
}
StepchildFileSystem::StepchildFileSystem(Stepchild* s){}

void StepchildFileSystem::writeSeqFile(string fname){
  return;
}
//  void writeBytesToSerial(uint8_t* byteArray, uint16_t number){
//    return;
//  }
void StepchildFileSystem::writeCurrentSeqToSerial(bool w){
  return;
}
void StepchildFileSystem::sendByteCount(string filename){
  return;
}
void StepchildFileSystem::sendFileName(string filename){
  return;
}
uint32_t StepchildFileSystem::getByteCount_standAlone(string filename){
  return 0;
}
uint32_t StepchildFileSystem::getByteCount(string filename){
  return 0;
}
void StepchildFileSystem::loadSeqFile(string filename){
  return;
}
vector<string> StepchildFileSystem::loadFilesAndSizes(){
  vector<string> fileSizes = {"0"};
  return fileSizes;
}
vector<string> StepchildFileSystem::loadFiles(){
  vector<string> filenames = {"*new*","test","beebebebbe","idk","i love you","sp404","microKORG"};
  return filenames;
}
void StepchildFileSystem::renameSeqFile(string filename){
  return;
}
bool StepchildFileSystem::deleteSeqFile(string filename){
  return false;
}
void StepchildFileSystem::duplicateSeqFile(string filename){
  return;
}
void StepchildFileSystem::writeCurrentSettingsToFile(){
}

void StepchildFileSystem::loadSavedSettingsFromFile(){
}
void StepchildFileSystem::loadSettings(){
}
void StepchildFileSystem::quickSave(){}
