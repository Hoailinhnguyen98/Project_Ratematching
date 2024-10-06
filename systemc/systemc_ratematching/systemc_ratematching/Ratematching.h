#ifndef RATEMATCHING_H
#define RATEMATCHING_H

#include <systemc.h>
#include <vector>
#include <queue>

const int sizePort = 128;
const int numPorts = 11;
const int MAX_FIFO_SIZE = numPorts * sizePort; //rows * sizePort
const int outlenRM = 2000;

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

    SC_HAS_PROCESS(RateMatching);
    RateMatching(sc_module_name name) : sc_module(name) {
        SC_THREAD(Ratematchingfunction);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

    // New FIFO buffer for data
    std::queue<sc_lv<MAX_FIFO_SIZE>> dataFIFO;
    bool allDataWritten = false; // Flag to indicate when all data has been pushed

    // Public methods
    void configure(int inlen, int outlen, int rv, int Qm, int nlayers, int Nref);

private:
    // Internal variables
    int inlen, outlen, rv, Qm, nlayers, Nref;

    // Function to perform rate matching
    std::vector<int> performRateMatching(const std::vector<int>&inputData);
    // Internal functions
    void Ratematchingfunction();
};

#endif