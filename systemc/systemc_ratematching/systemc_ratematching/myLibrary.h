// myLibrary.h

#ifndef MYLIBRARY_H
#define MYLIBRARY_H

#include <systemc.h>

// Function declarations for floor and ceil operations
int myFloor(double x);
int myCeil(double x);
int checkError(const sc_lv<128>& sinkData, const sc_lv<128>& outputData);
void readDataFromFile(const std::string& filePath, std::vector<sc_lv<128>>& dataBuffer);

#endif // MYLIBRARY