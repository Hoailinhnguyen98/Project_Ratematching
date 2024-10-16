/*
 * ==============================================
 * File:        Ratematching.cpp
 * Author:      linhnth@soc.one
 * Date Created: 
 * Date Updated: 04 October 2024
 * Version:     1.0
 * Description: This file implements the rate matching
 *              algorithm, which processes 
 *              input data arrays and generates rate-matched
 *              output based on system parameters.
 *
 * Variable Descriptions:
 *  - inlen: input array representing data to be rate-matched.
 *  - outlen: output array holding rate-matched data.
 *  - inputLength: Length of the input data array.
 *  - outputLength: Length of the output data array after rate matching.
 *  - rv: The redundancy version (rv) of the output is controlled by the rv parameter (0,1,2,3)
 *  - nlayers: The total number of transmission layers associated with the transport block (1...4)
 *  - Qm: Represents the modulation scheme used (e.g., QPSK <=> Qm = 2, 16-QAM <=> Qm = 4).
 *  - Nref: The parameter defined in TS 38.212 Section 5.4.2.1.
 * Function 
 * std::vector<int> RateMatching::performRateMatching(const std::vector<int>& inputData) => return outputData
 * ==============================================
 */


 /***************Include files**************/
#include "RateMatching.h"
#include <iostream>
#include <cmath>
#include <vector>
#include "myLibrary.h"
#include <fstream>
#include <sstream>

/*************** Define constants**************/

const std::vector<int> ZcVec = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    18, 20, 22, 24, 26, 28, 30, 32, 36, 40, 44, 48, 52,
    56, 60, 64, 72, 80, 88, 96, 104, 112, 120, 128, 144,
    160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
};

/*************** Define functions**************/

// Main rate matching function
void RateMatching::Ratematchingfunction() {

    RateMatchingConfig config;

    // Define the file path
    std::string filename = "C:/Users/ADMIN/Desktop/Project_Ratematching/io/input/config_inputdata1.txt";

    // Open the file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        int value;

        // Extract key and value
        if (std::getline(iss, key, ':') && iss >> value) {
            // Remove whitespace
            key.erase(remove(key.begin(), key.end(), ' '), key.end());

            if (key == "input_length") {
                config.inlen = value;
            }
            else if (key == "output_length") {
                config.outlen = value;
            }
            else if (key == "redundancy_version") {
                config.rv = value;
            }
            else if (key == "layer") {
                config.nlayers = value;
            }
            else if (key == "modulation_type") {
                config.Qm = value;
            }
            else if (key == "Nref") {
                config.Nref = value;
            }
            else {
                std::cerr << "Warning: Unrecognized key in config file: " << key << std::endl;
            }
        }
        else {
            std::cerr << "Warning: Incorrect format in line: " << line << std::endl;
        }
    }

	// Close the file
    file.close();

    while (true) {
        wait(); // Wait for clock edge
        if (rst.read()) { // Reset behavior
            std::cout << "RateMatching: Reset signal received." << std::endl;
            valid_out.write(false);
            ready_out.write(false);

            // Clear FIFO
            while (!dataFIFO.empty()) {
                dataFIFO.pop();
            }
            allDataWritten = false;
            continue;
        }

        ready_out.write(dataFIFO.size() < MAX_FIFO_SIZE); // Ready to receive data if FIFO isn't full

        while (!valid_in.read()) {
            wait();
        }

        // Push data into FIFO if valid and ready
        if (valid_in.read() && ready_out.read()) {
            std::cout << "DataFIFO: Pushing data into FIFO." << dataFIFO.size() << std::endl;
            if (dataFIFO.size() < MAX_FIFO_SIZE) {
                sc_lv<128> data = data_in.read();
                dataFIFO.push(data);
                std::cout << "RateMatching: Data received and pushed into FIFO." << std::endl;

                // Debugging statement to check FIFO size after insertion
                /*std::cout << "Current FIFO size after pushing: " << dataFIFO.size() << std::endl;
                std::cout << (MAX_FIFO_SIZE / sizePort) << std::endl;*/

                // Check if the FIFO is full minus one element
                if (dataFIFO.size() == (MAX_FIFO_SIZE / sizePort)) {
                    allDataWritten = true; // Set the flag to start rate matching
                    std::cout << "RateMatching: FIFO is nearly full (size: " << dataFIFO.size() << "), ready to perform rate matching." << std::endl;
                }

            }
            else {
                std::cout << "RateMatching: FIFO is full, cannot receive more data." << std::endl;
            }
        }

        // Process the FIFO once all data is written and the FIFO contains data
        if (allDataWritten && !dataFIFO.empty()) {
            std::cout << "RateMatching: Starting rate matching process." << std::endl;

            // Read configuration values from fileConfig
            int inlen = config.inlen;
            int outlen = config.outlen;
            int rv = config.rv;
            int nlayers = config.nlayers;
            int Qm = config.Qm;
            int Nref = config.Nref;

            // Print loaded configuration values
            std::cout << "Config Loaded: " << std::endl;
            std::cout << "  Input Length (inlen): " << config.inlen << std::endl;
            std::cout << "  Output Length (outlen): " << config.outlen << std::endl;
            std::cout << "  Redundancy Version (rv): " << config.rv << std::endl;
            std::cout << "  Number of Layers (nlayers): " << config.nlayers << std::endl;
            std::cout << "  Modulation Type (Qm): " << config.Qm << std::endl;
            std::cout << "  Nref: " << config.Nref << std::endl;


            //Concatenate all FIFO data into a single large data string
            sc_lv<MAX_FIFO_SIZE> concatenatedData; // Assuming 5200-bit buffer
            int bitIndex = 0;

            while (!dataFIFO.empty()) {
                sc_lv<128> data = dataFIFO.front();
                dataFIFO.pop();

                // Insert 128-bit chunk into concatenated data
                for (int i = 0; i < 128; ++i) {
                    concatenatedData[bitIndex + i] = data[127 - i];
                }
                bitIndex += 128; // Move the bit index by 128 bits
            }

            std::cout << "RateMatching: Concatenated data size: " << bitIndex << " bits." << std::endl;

            //Convert concatenatedData to vector
            //std::vector<int> concatenatedDataVector = convertToVector(concatenatedData);
            std::vector<int> concatenatedDataVector;
            concatenatedDataVector.reserve(MAX_FIFO_SIZE);

            for (int i = 0; i < MAX_FIFO_SIZE; ++i) {
                char bit = concatenatedData[i].to_char();
                if (bit == '0') {
                    concatenatedDataVector.push_back(0);
                }
                else if (bit == '1') {
                    concatenatedDataVector.push_back(1);
                }
                else {
                    throw std::invalid_argument("Invalid character in bit string at position " + std::to_string(i));
                }
            }

            /**************Perform rate matching********************/
			std::cout << "RateMatching: Performing rate matching on the entire concatenated data." << std::endl;

            // Get input vector with inlen elements
            std::vector<int> input(concatenatedDataVector.begin(), concatenatedDataVector.begin() + inlen);

            // Get code block soft buffer size
            int minValue = (inlen < Nref) ? inlen : Nref;
            int Ncb = (Nref != 0) ? minValue : inlen;

            // Determine base graph number from N
            int bgn, ncwnodes, Zc;
            bool found = 0;
            for (size_t i = 0; i < ZcVec.size(); ++i) {
                if (inlen == ZcVec[i] * 66) {
                    found = 1;
                }
            }

            if (found) {
                bgn = 1;
                ncwnodes = 66;
            }
            else {
                bgn = 2;
                ncwnodes = 50;
            }
            Zc = inlen / ncwnodes;

            // Get starting position in circular buffer
            int k0;
            if (bgn == 1) {
                if (rv == 0) {
                    k0 = 0;
                }
                else if (rv == 1) {
                    k0 = myFloor(17.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(17.0 * Ncb / inlen) * Zc);
                }
                else if (rv == 2) {
                    k0 = myFloor(33.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(33.0 * Ncb / inlen) * Zc);
                }
                else {
                    k0 = myFloor(56.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(56.0 * Ncb / inlen) * Zc);
                }
            }
            else {
                if (rv == 0) {
                    k0 = 0;
                }
                else if (rv == 1) {
                    k0 = myFloor(13.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(13.0 * Ncb / inlen) * Zc);
                }
                else if (rv == 2) {
                    k0 = myFloor(25.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(25.0 * Ncb / inlen) * Zc);
                }
                else {
                    k0 = myFloor(43.0 * Ncb / inlen) * Zc;
                    //k0 = static_cast<int>(std::floor(43.0 * Ncb / inlen) * Zc);
                }
            }

            int E = (1 - (outlen / (nlayers * Qm)) - 1) >= 0 ? nlayers * Qm * myFloor(static_cast<double>(outlen) / (nlayers * Qm))
                : nlayers * Qm * myCeil(static_cast<double>(outlen) / (nlayers * Qm));

            //    int E;
            //    if ((1 - (outlen / (nlayers * Qm)) - 1)>=0)
            //    {
            //        E = nlayers * Qm * static_cast<int>(std::floor(static_cast<double>(outlen) / (nlayers * Qm)));
            //    }
            //    else {
            //        E = nlayers * Qm * static_cast<int>(std::ceil(static_cast<double>(outlen) / (nlayers * Qm)));
            //    }

            // Perform rate matching
            std::vector<int> e(E, 0);
            int k = 0;
            int j = 0;

            while (k < E) {
                if (input[(k0 + j) % Ncb] != -1) {
                    e[k] = input[(k0 + j) % Ncb];
                    ++k;
                }
                ++j;
            }

            // Bit Interleaving
            int rows = E / Qm;
            std::vector<int> e_reshaped(E);
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < Qm; ++j) {
                    e_reshaped[i * Qm + j] = e[j * rows + i];
                }
            }

            std::vector<int> rateMatchedData;
            rateMatchedData = e_reshaped; // output of rate matching

            //std::cout << "\nSize of rateMatchedData: " << rateMatchedData.size() << std::endl;
            //for (int i = 0; i < rateMatchedData.size(); ++i) {
            //    std::cout << rateMatchedData[i];
            //    
            //    if ((i + 1) % 128 == 0) {
            //        std::cout << std::endl; 
            //    }
            //}
            //std::cout<<"\nRateMatching: Rate matching process completed." << std::endl;


            //Send the rate-matched data in chunks of 128 bits
            int totalBits = outlenRM; // Length of the rate-matched data

            // Load data from MATLAB file
            int numCount = 0;
            std::string filePath = "C:/Users/ADMIN/Desktop/Project_Ratematching/io/output/output_data1_matlab.txt";
            std::vector<sc_lv<128>> dataBuffer;
            // Call the function to read data
            readDataFromFile(filePath, dataBuffer);

            // Send data in sinkdata of 128 bits
            for (int start = 0; start < totalBits; start += sizePort) {
                sc_lv<128> sinkdata;

                //// Copy 128 bits from rateMatchedData to chunk
                //for (int i = 0; i < sizePort; ++i) {
                //    if (start + i < totalBits) {
                //        sinkdata[i] = rateMatchedData[start + i]; //Copy data
                //    }
                //    else {
                //        sinkdata[i] = 0; // Padding 0 without data
                //    }
                //}

                for (int i = 0; i < sizePort; ++i) {
                    if (start + i < totalBits) {
                        sinkdata[sizePort - 1 - i] = rateMatchedData[start + i]; //Copy data
                    }
                    else {
                        sinkdata[sizePort - 1 - i] = 0; // Padding 0 without data
                    }
                }

                sc_lv<128> lv_value;
                for (int i = 0; i < 128; ++i) {
                    lv_value[i] = dataBuffer[numCount][i];
                }
                std::cout << "Value in dataBuffer[" << numCount << "] as sc_lv<128>: " << lv_value << std::endl;

                // Check error
                int ERR = checkError(sinkdata, dataBuffer[numCount]);
                numCount = numCount + 1;
                std::cout << "Error: " << ERR << std::endl;

                // Send each sinkdata to the output
                data_out.write(sinkdata);  // Write the 128-bit sinkdata to the sink
                valid_out.write(true);  // Indicate valid output
                wait();                 // Ensure the result is stable for one clock cycle
            }

            // After sending all sinkdata, signal that no more valid data remains
            valid_out.write(false);
            std::cout << "RateMatching: All rate-matched data sent to sink." << std::endl;
        }
        else {
            valid_out.write(false); // No valid input
            std::cout << "RateMatching: Waiting for valid input data." << std::endl;
        }
    }
}





 