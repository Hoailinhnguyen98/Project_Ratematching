/*
 * ==============================================
 * File:        Ratematching.cpp
 * Author:      Nguyen Thi Hoai Linh
 * Email:       linhnth@hnsoc.one
 * Date Created:
 * Last Modified: 21 October 2024
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

#include "RateMatching.h"
#include <fstream>
#include <iostream>
#include "myLibrary.h"

 /*************** Define constants**************/
const std::vector<int> ZcVec = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    18, 20, 22, 24, 26, 28, 30, 32, 36, 40, 44, 48, 52,
    56, 60, 64, 72, 80, 88, 96, 104, 112, 120, 128, 144,
    160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
};

/*************** Define functions**************/
void ratematching::ratematchingfunction() {

    ratematchingConfig config;

    // Initialize output signals
    din_ready.write(false);
    dout_valid.write(false);
    dout_last.write(false);
    config_ready.write(false);

    while (true) {
        wait(); // Wait for clock edge

        // Reset condition
        if (rst.read()) {

            dout_valid.write(false);
            dout_last.write(false);
            din_ready.write(false);
            // Clear the FIFO
            while (!dataFIFO.empty()) {
                dataFIFO.pop();
            }
            config_ready.write(false); 
            continue; // Move to next cycle
        }

        // Configuration input handling
        if (config_valid.read()) {
            sc_lv<CFG_WIDTH> iConfigData = config_data.read();
            // Parse the configuration data
            config.inlen = iConfigData.range(15, 0).to_uint();
            config.outlen = iConfigData.range(31, 16).to_uint();
            config.rv = iConfigData.range(33, 32).to_uint();
            config.nlayers = iConfigData.range(36, 34).to_uint();
            config.Qm = iConfigData.range(40, 37).to_uint();
            config.Nref = iConfigData.range(46, 41).to_uint();
            config_ready.write(true); // Indicate configuration is processed
            std::cout << "RateMatching: Configuration received and processed." << std::endl;
            wait(); // Wait for next cycle to continue processing
            continue;
        }
        else {
            config_ready.write(false); // No config, continue waiting
        }

        // Always set din_ready based on FIFO size
        if (dataFIFO.size() < (MAX_FIFO_SIZE / sizePort)) {
            din_ready.write(true);  // FIFO is not full, allowing new data to be received
        }
        else {
            din_ready.write(false); // FIFO full, not receiving new data
        }

        while (!din_valid.read()) {
            wait();
        }

        if (din_valid.read() && din_ready.read()) {
            sc_lv<128> input_data = din_data.read();
            dataFIFO.push(input_data); // Add data to the FIFO
            //std::cout << "index Input" << dataFIFO.size() << std::endl;
            //std::cout << "RateMatching: Data received: " << input_data << std::endl;
            din_ready.write(true); // Ready to receive more data	

        }
        else {
            din_ready.write(false); // Not ready to receive data
        }

        // Begin rate matching if enough data is available in FIFO
        if (dataFIFO.size() == (MAX_FIFO_SIZE / sizePort)) {

            std::cout << "RateMatching: Starting rate matching process." << std::endl;
            din_ready.write(false);

            // Read configuration values
            int inlen = config.inlen;
            int outlen = config.outlen;
            int rv = config.rv;
            int nlayers = config.nlayers;
            int Qm = config.Qm;
            int Nref = config.Nref;

            std::cout << "Config Loaded: " << std::endl;
            std::cout << "  Input Length (inlen): " << inlen << std::endl;
            std::cout << "  Output Length (outlen): " << outlen << std::endl;
            std::cout << "  Redundancy Version (rv): " << rv << std::endl;
            std::cout << "  Number of Layers (nlayers): " << nlayers << std::endl;
            std::cout << "  Modulation Type (Qm): " << Qm << std::endl;
            std::cout << "  Nref: " << Nref << std::endl;

            // Concatenate all FIFO data into one data stream for processing
            sc_lv<MAX_FIFO_SIZE> concatenatedData;
            int bitIndex = 0;
            while (!dataFIFO.empty()) {
                sc_lv<128> data = dataFIFO.front();
                dataFIFO.pop();

                // Insert 128-bit data into concatenatedData
                for (int i = 0; i < 128; ++i) {
                    concatenatedData[bitIndex + i] = data[127 - i];
                }
                bitIndex += 128; // Move the index for the next chunk
            }

            std::cout << "RateMatching: Concatenated data size: " << bitIndex << " bits." << std::endl;

            // Convert concatenated data to vector of bits for rate matching
            std::vector<int> concatenatedDataVector(bitIndex);
            for (int i = 0; i < bitIndex; ++i) {
                char bit = concatenatedData[i].to_char();
                concatenatedDataVector[i] = (bit == '1') ? 1 : 0;
            }

            // Perform the rate matching process using the updated logic
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
                    //k0 = myFloor(17.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(17.0 * Ncb / inlen) * Zc);
                }
                else if (rv == 2) {
                    //k0 = myFloor(33.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(33.0 * Ncb / inlen) * Zc);
                }
                else {
                    //k0 = myFloor(56.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(56.0 * Ncb / inlen) * Zc);
                }
            }
            else {
                if (rv == 0) {
                    k0 = 0;
                }
                else if (rv == 1) {
                    //k0 = myFloor(13.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(13.0 * Ncb / inlen) * Zc);
                }
                else if (rv == 2) {
                    //k0 = myFloor(25.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(25.0 * Ncb / inlen) * Zc);
                }
                else {
                    //k0 = myFloor(43.0 * Ncb / inlen) * Zc;
                    k0 = static_cast<int>(std::floor(43.0 * Ncb / inlen) * Zc);
                }
            }

            /*int E = (1 - (outlen / (nlayers * Qm)) - 1) >= 0 ? nlayers * Qm * myFloor(static_cast<double>(outlen) / (nlayers * Qm))
                : nlayers * Qm * myCeil(static_cast<double>(outlen) / (nlayers * Qm));*/

            int E;
            if ((1 - (outlen / (nlayers * Qm)) - 1) >= 0)
            {
                E = nlayers * Qm * static_cast<int>(std::floor(static_cast<double>(outlen) / (nlayers * Qm)));
            }
            else {
                E = nlayers * Qm * static_cast<int>(std::ceil(static_cast<double>(outlen) / (nlayers * Qm)));
            }

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

            std::vector<int> rateMatchedData = e_reshaped; // output of rate matching
            std::cout << std::endl;
            dout_valid.write(true); // Signal valid output
            int dRows = rateMatchedData.size();

            // Transmit the rate matched data, with the last chunk condition
            for (int start = 0; start < dRows; start += sizePort) {
                sc_lv<128> sinkdata;
                for (int i = 0; i < sizePort; ++i) {
                    sinkdata[sizePort - 1 - i] = (start + i < dRows) ? rateMatchedData[start + i] : 0; // Zero-padding
                }

                dout_data.write(sinkdata); // Write to output

                // Signal dout_last if this is the last output chunk
                if (start + sizePort >= dRows) {
                    dout_last.write(true); // Indicate last data
                    std::cout << "RateMatching: Last output data transmitted." << std::endl;
                }

                wait(); // Wait for the next clock cycle
            }

            // Once the final output is sent, reset valid and last signals
            dout_valid.write(false);
            dout_last.write(false);
            
        }

    }
}