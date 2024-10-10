#include "myLibrary.h"

// Helper function for floor
int myFloor(double x) {
    int intPart = static_cast<int>(x);
    if (x < 0 && x != intPart) {
        return intPart - 1;
    }
    return intPart;
}

// Helper function for ceil
int myCeil(double x) {
    int intPart = static_cast<int>(x);
    if (x > 0 && x != intPart) {
        return intPart + 1;
    }
    return intPart;
}


// Check error function
int checkError(const sc_lv<128>& sinkData, const sc_lv<128>& outputData) {
    int errorCount = 0;
    for (size_t i = 0; i < (size_t)sinkData.size(); ++i)
    {

        if (sinkData[i] != outputData[i]) {
            ++errorCount;
        }
    }

    return errorCount;
}

void readDataFromFile(const std::string& filePath, std::vector<sc_lv<128>>& dataBuffer) {
    // Open the file
    std::ifstream inputFile(filePath);

    // Check if the file opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    // Read lines from the file
    std::string line;
    while (std::getline(inputFile, line)) {
        // Check if the line has exactly 128 characters
        if (line.length() == 128) {
            // Store the line as sc_lv<128>
            dataBuffer.push_back(sc_lv<128>(line.c_str()));
        }
    }

    // Close the file
    inputFile.close();
}