#ifndef RATEMATCHING_H
#define RATEMATCHING_H

#include <systemc.h>
#include <iostream>
#include <vector>
#include <queue>
#include <string>


const int sizePort = 128;
const int numPorts = 11;
const int MAX_FIFO_SIZE = numPorts * sizePort; //rows * sizePort
const int outlenRM = 2000; // Output length for rate-matching

const int CFG_WIDTH = 47; // Configuration bus width

// Structure containing configuration parameters for RateMatching
struct ratematchingConfig {
    sc_uint<16> inlen;        // Input length (16 bits)
    sc_uint<16> outlen;       // Output length (16 bits)
    sc_uint<2> rv;            // Redundancy version (2 bits)
    sc_uint<3> nlayers;       // Number of layers (3 bits)
    sc_uint<4> Qm;            // Modulation type (4 bits)
    sc_uint<6> Nref;          // Reference value for calculations (6 bits)
};

SC_MODULE(ratematching) {
public:
    // Ports
    sc_in<bool>             clk;            // Clock
    sc_in<bool>             rst;            // Reset

    sc_in<sc_lv<128>>       din_data;       // 128-bit input data
    sc_in<bool>             din_valid;      // Valid signal from input
    sc_in<bool>             din_last;
    sc_out<bool>            din_ready;      // Ready signal to input

    sc_out<sc_lv<128>>      dout_data;      // 128-bit output data
    sc_out<bool>            dout_valid;     // Valid signal to output
    sc_out<bool>            dout_last;
    sc_in<bool>             dout_ready;     // Ready signal from output

    // Configuration bus input as a 47-bit signal
    sc_in<sc_lv<CFG_WIDTH>> config_data;
    sc_in<bool>             config_valid;
    sc_out<bool>            config_ready;

    // Constructor
    SC_CTOR(ratematching) {
        SC_THREAD(ratematchingfunction);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

private:
    // Internal FIFO buffer for data
    std::queue<sc_lv<MAX_FIFO_SIZE>> dataFIFO;
    bool allDataWritten = false; // Flag to indicate when all data has been pushed

    // Internal function for rate matching logic
    void ratematchingfunction();
};

#endif // RATEMATCHING_H