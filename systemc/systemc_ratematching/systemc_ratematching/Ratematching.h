#ifndef RATEMATCHING_H
#define RATEMATCHING_H

#include <systemc.h>
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>

const int sizePort = 128;
const int numPorts = 11;
const int MAX_FIFO_SIZE = numPorts * sizePort; //rows * sizePort
const int outlenRM = 2000; // Output length for rate-matching

// Structure containing configuration parameters for RateMatching
struct RateMatchingConfig {
    sc_uint<16> inlen;        // Input length (16 bits)
    sc_uint<16> outlen;       // Output length (16 bits)
    sc_uint<2> rv;            // Redundancy version (2 bits)
    sc_uint<3> nlayers;       // Number of layers (3 bits)
    sc_uint<4> Qm;            // Modulation type (4 bits)
    sc_uint<6> Nref;          // Reference value for calculations (6 bits)
};

SC_MODULE(RateMatching) {
public:
    // Ports
    sc_in<bool> clk;          // Clock
    sc_in<bool> rst;          // Reset
    sc_in<bool> valid_in;     // Valid signal from input
    sc_out<bool> ready_out;   // Ready signal to input
    sc_in<sc_lv<128>> data_in; // 128-bit input data
    sc_out<sc_lv<128>> data_out; // 128-bit output data
    sc_out<bool> valid_out;   // Valid signal to output
    sc_in<bool> ready_in;     // Ready signal from output

    // Configuration bus input as a 47-bit signal
    sc_in<sc_lv<47>> config_bus;

    // Constructor
    SC_CTOR(RateMatching) {
        SC_THREAD(Ratematchingfunction);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

    // Function to parse the 47-bit configuration bus into the RateMatchingConfig structure
    void parseConfigBus(const sc_lv<47>& config, RateMatchingConfig& config_struct) {
        config_struct.inlen = config.range(15, 0).to_uint();      // bits [15:0] for inlen
        config_struct.outlen = config.range(31, 16).to_uint();    // bits [31:16] for outlen
        config_struct.rv = config.range(33, 32).to_uint();        // bits [33:32] for rv
        config_struct.nlayers = config.range(36, 34).to_uint();   // bits [36:34] for nlayers
        config_struct.Qm = config.range(40, 37).to_uint();        // bits [40:37] for Qm
        config_struct.Nref = config.range(46, 41).to_uint();      // bits [46:41] for Nref
    }

private:
    // Internal FIFO buffer for data
    std::queue<sc_lv<MAX_FIFO_SIZE>> dataFIFO;
    bool allDataWritten = false; // Flag to indicate when all data has been pushed

    // Internal function for rate matching logic
    void Ratematchingfunction();
};

#endif // RATEMATCHING_H