#ifndef SOURCE_H
#define SOURCE_H

#include <systemc.h>
#include <string>
#include <vector>
#include <fstream>

SC_MODULE(Source) {
public:
    sc_out<bool> valid_out;   // Valid signal from Source
    sc_out<sc_lv<128>> data_out; // 128-bit output data
    sc_in<bool> ready_in;     // Ready signal from RateMatching

    sc_in<bool> clk;          // Clock
    sc_in<bool> rst;          // Reset

    // Constructor
    SC_CTOR(Source) {
        SC_THREAD(source_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

    /*************** Public Member Functions ***************/
    // Method to configure the source with an input file path
    void configure(const std::string & inputFilePath);

private:
    void source_thread();
    /*************** Private Member Variables ***************/
    std::ifstream inputFile;             // Input file stream
    std::vector<sc_lv<128>> dataBuffer;  // Buffer to hold data lines
    int dataCount;                       // Tracks the current row of data being processed
};

#endif