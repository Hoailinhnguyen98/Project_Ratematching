
/*
 * ==============================================
 * File:        Source.cpp
 * Author:      Nguyen Thi Hoai Linh
 * Email:       linhnth@hnsoc.one
 * Date Created:
 * Last Modified: 21 October 2024
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
#include "ratematching.h"
#include "source.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

void source::source_thread() {

    // Initial conditions
    // ================================
    dout_data.write(sc_lv<128>("0"));  // Initialize dout_data to 0
    dout_valid.write(false);           // Initialize dout_valid to false
    dout_last.write(false);             // Initialize dout_last to false
    config_data.write(sc_lv<CFG_WIDTH>("0"));  // Initialize config_data to 0
    config_valid.write(false);         // Initialize config_valid to false

    // Files
    /*std::string cfgFilePath = "../../io/input/config_inputdata1.txt";
    std::string inputFilePath = "../../io/input/input_data1.txt";*/

    std::string cfgFilePath = "C:/Users/ADMIN/Desktop/Project_Ratematching/io/input/config_inputdata2.txt";
    std::string inputFilePath = "C:/Users/ADMIN/Desktop/Project_Ratematching/io/input/input_data2.txt";


    /////////////////////////////////////
    /****** Sending configuration ******/
    /////////////////////////////////////
    std::ifstream file(cfgFilePath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << cfgFilePath << std::endl;
        sc_stop();
    }

    ratematchingConfig config;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        int value;

        if (std::getline(iss, key, ':') && iss >> value)
        {
            // Remove whitespace
            key.erase(remove(key.begin(), key.end(), ' '), key.end());

            if (key == "input_length")
            {
                config.inlen = value;
            }
            else if (key == "output_length")
            {
                config.outlen = value;
            }
            else if (key == "redundancy_version")
            {
                config.rv = value;
            }
            else if (key == "layer")
            {
                config.nlayers = value;
            }
            else if (key == "modulation_type")
            {
                config.Qm = value;
            }
            else if (key == "Nref")
            {
                config.Nref = value;
            }
            else
            {
                std::cerr << "Warning: Unrecognized key in config file: " << key << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Incorrect format in line: " << line << std::endl;
        }
    }

    file.close();

    sc_lv<CFG_WIDTH> configData;
    configData.range(15, 0) = config.inlen;
    configData.range(31, 16) = config.outlen;
    configData.range(33, 32) = config.rv;
    configData.range(36, 34) = config.nlayers;
    configData.range(40, 37) = config.Qm;
    configData.range(46, 41) = config.Nref;

    config_data.write(configData);
    config_valid.write(true);

    while (!config_ready.read()) {
        wait();
    }

    config_valid.write(false);
    wait();

    /////////////////////////////////////
    /****** Sending input data    ******/
    /////////////////////////////////////
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << inputFilePath << std::endl;
        sc_stop();
    }
    std::vector<sc_lv<128>> dataBuffer;

    // Read lines from the file and store them in dataBuffer
    while (std::getline(inputFile, line)) {
        if (line.length() == 128) {
            dataBuffer.push_back(sc_lv<128>(line.c_str()));
        }
        else {
            std::cerr << "Warning: Skipping line with invalid length: " << line.length() << std::endl;
        }
    }

    inputFile.close(); // Close the file after reading

    /***** Send data to FIFO ******/
    
    int dataCount = 0;           // Reset data counter
    std::cout << "Source: Sending data to RateMatching dataCount [" << dataCount << "]" << std::endl; // Print datacout
    sc_lv<128> data = dataBuffer[dataCount];
    //std::cout << "Source: Sending data to RateMatching: " << data << std::endl;

    dout_data.write(data);
    dout_valid.write(true);   // Indicate valid data
    dataCount = dataCount + 1;

    // Main loop to send data to FIFO
    while (true) {

        std::cout << std::endl;
        if (dataCount >= (int)dataBuffer.size())
        {
            break;
        }

        // Check if reset signal is active
        if (rst.read()) {
            dout_valid.write(false);
            dout_last.write(false);
            std::cout << "Source: Reset signal received." << std::endl;
            dataCount = 0;           // Reset data counter
            continue;                // Skip the rest of the loop
        }


        // Wait for FIFO to be ready
        while (!dout_ready.read()) {
            wait(); // Wait until the FIFO is ready to accept data
        }

        // Send data only when FIFO is ready
        if (dout_ready.read()) {
            sc_lv<128> data = dataBuffer[dataCount];
            dout_data.write(data);
            dout_valid.write(true); // Indicate valid data
            //std::cout << "Source: Sending data to RateMatching: " << data << std::endl;
            std::cout << "Source: Sending data to RateMatching (dataCount: " << dataCount << ")" << std::endl;
            ++dataCount;

            // Assert out_last on last data word
            dout_last.write(dataCount == (int)dataBuffer.size());

        }

        wait(); // Wait for the next clock cycle
    }

    // Signal that no more data will be sent
    dout_valid.write(false);
    std::cout << "Source: No more data to send." << std::endl;

}
