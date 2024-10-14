//
//  FileSystem.cpp
//  childOS_headless
//
//  Created by Alex LaFetra Thompson on 10/9/23.
//

//wrapping the LittleFS functions into a filesystem manager
//need to replace:
/*
 LittleFs.begin()
 //directory object
 Dir LittleFs.openDir(string path);
 bool Dir.next();
 string Dir.fileName();
 LittleFS.end();
 File f = LittleFS.open(string,char);
 */

#include <iostream>
#include <fstream>
#include <filesystem>

void quickSave(){}

//replacement for the Pico's filesystem
//FileSystem LittleFS;

