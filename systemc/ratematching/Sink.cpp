/*
 * ==============================================
 * File:        Sink.cpp
 * Author:      linhnth@soc.one
 * Date Created: 
 * Date Updated: 04 October 2024
 * Version:     1.0
 * brief Description: Sink module that receives and 
 *                    processes output data from the 
 *                    rate matching module. This module 
 *                    is responsible for handling data 
 *                    storage or transmission to the next 
 *                    stage in the system.
 *
 * ==============================================
 */

 /***************Include files**************/
#include "Sink.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void Sink::sink_thread() {

    /****** Open file ******/
    std::string outputFilePath = "../../io/output/output_data.txt";
	std::ofstream outputFile;
    outputFile.open(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Failed to open output file: " << outputFilePath << std::endl;
    }

	/****** Write data to file ******/

    int dataCount = 0;
    while (true) {
        wait(); // Wait for clock edge

        if (rst.read()) {
            ready_out.write(false);
            dataCount = 0;
            std::cout << "Sink: Reset signal received." << std::endl;
            continue;
        }

        // Indicate that the Sink is ready to receive data
        ready_out.write(true);

        // Check if valid data is received
        if (valid_in.read()) {
            sc_lv<128> received_data = data_in.read();
            std::cout << "Sink: Data received: " << received_data << std::endl;
            outputFile << received_data << std::endl;
            // Process received data (optional)
            dataCount++;

            std::cout << "\n***************Sink: Data received and processed. (dataCount: " << dataCount << ")***************\n" << std::endl;

        }
        else {
            std::cout << "Sink: Waiting for valid data." << std::endl;
        }
    }
    
    sc_stop();
    return;
}