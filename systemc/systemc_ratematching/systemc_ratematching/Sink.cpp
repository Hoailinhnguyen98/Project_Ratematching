
/*
 * ==============================================
 * File:        Sink.cpp
 * Author:      Nguyen Thi Hoai Linh
 * Email:       linhnth@hnsoc.one
 * Date Created:
 * Last Modified: 21 October 2024
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
#include "sink.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void sink::sink_thread() {

    /****** Open file ******/
    std::string outputFilePath = "C:/Users/ADMIN/Desktop/Project_Ratematching/io/output/output_data2.txt";
    std::ofstream outputFile;
    outputFile.open(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Failed to open output file: " << outputFilePath << std::endl;
    }


    int dataCount = 0;
    while (true) {
        wait(); // Wait for clock edge

        if (rst.read()) {
            din_ready.write(false);
            dataCount = 0;
            std::cout << "Sink: Reset signal received." << std::endl;
            continue;
        }

        // Indicate that the Sink is ready to receive data
        din_ready.write(true);

        // Check if valid data is received
        if (din_valid.read()) {
            sc_lv<128> received_data = din_data.read();
            std::cout << "Sink : Received dataCount [" << dataCount << "]:" << received_data << std::endl;
            outputFile << received_data << std::endl;
            dataCount++;
            //std::cout << "\n***************Sink: Data received and processed. (dataCount: " << dataCount << ")***************\n" << std::endl;
            
            // Check if it's the last data
            if (din_last.read()) {
                std::cout << "Sink: Last data received. Total received: " << dataCount << " data counts." << std::endl;
                din_ready.write(false);  // No more data to receive
                break;  // Exit the loop since all data has been received
            }
        }
        else {
            std::cout << "Sink: Waiting for valid data." << std::endl;
        }
    }

    sc_stop();
}
