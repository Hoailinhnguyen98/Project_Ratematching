/*
 * ==============================================
 * File:        Source.cpp
 * Author:      linhnth@soc.one
 * Date Created: 
 * Date Updated: 04 October 2024
 * Version:     1.0
 * brief Description: Source module that generates 
 *                    input data for the rate matching 
 *                    process. This module is responsible 
 *                    for producing a continuous stream 
 *                    of data that meets specified 
 *                    requirements for further processing.
 * 
 * ==============================================
 */

#include "Source.h"
#include <iostream>


void Source::configure(const std::string& inputFilePath) {
    //std::cout << "Configuring Source module" << std::endl;
    inputFile.open(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << inputFilePath << std::endl;
        return;
    }

    // Read lines from the file and store them in dataBuffer
    std::string line;
    while (std::getline(inputFile, line)) {
        // Convert each line to sc_lv<128> and store it in the buffer
        // std::cout << "Line: " << line << std::endl;
        if (line.length() == 128) {
            //dataBuffer.push_back(sc_lv<128>(line.c_str()));
            dataBuffer.push_back(sc_lv<128>(line.c_str()));
        }
    }


    inputFile.close(); // Close the file after reading

}

void Source::source_thread() {

    dataCount = 0;           // Reset data counter
    std::cout << "Source: Sending data to RateMatching (dataCount: " << dataCount << ")" << std::endl; // Print datacout
    sc_lv<128> data = dataBuffer[dataCount];
    std::cout << "Source: Sending data to RateMatching: " << data << std::endl;

    data_out.write(data);
    valid_out.write(true); // Indicate valid data
    dataCount = dataCount + 1;

    // Main loop to send data to FIFO
    while (true) {

        std::cout << std::endl;
        if (dataCount  >= dataBuffer.size())
        {
            break;
        }    

        // Check if reset signal is active
        if (rst.read()) {
            valid_out.write(false);
            std::cout << "Source: Reset signal received." << std::endl;
            dataCount = 0;           // Reset data counter
            continue;                // Skip the rest of the loop
        }


        // Wait for FIFO to be ready
        while (!ready_in.read()) {
            wait(); // Wait until the FIFO is ready to accept data
        }

        // Send data only when FIFO is ready
        if (ready_in.read()) {
            sc_lv<128> data = dataBuffer[dataCount];
            data_out.write(data);
            valid_out.write(true); // Indicate valid data
            std::cout << "Source: Sending data to RateMatching: " << data << std::endl;
            std::cout << "Source: Sending data to RateMatching (dataCount: " << dataCount << ")" << std::endl;
            ++dataCount;
        }

        wait(); // Wait for the next clock cycle
    }

    // Signal that no more data will be sent
    valid_out.write(false);
    std::cout << "Source: No more data to send." << std::endl;

}