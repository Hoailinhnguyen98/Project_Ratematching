#ifndef SOURCE_H
#define SOURCE_H

#include <systemc.h>

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

private:
    void source_thread();
};

#endif