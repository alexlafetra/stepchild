#pragma once

#include "classes/Note.h"
#include <vector>

//Overloaded stringify function to create strings so that this code is compatible with both the Arduino String class and the C++ string class
//This works for the Stepchild version of ChildOS
String stringify(int a);
String stringify(uint8_t a);
String stringify(int8_t a);
String stringify(uint16_t a);
String stringify(int16_t a);
String stringify(uint32_t a);
String stringify(int32_t a);
String stringify(std::vector<Note>::size_type a);
String stringify(float a);
String stringify(const char * a);
int toInt(String s);
