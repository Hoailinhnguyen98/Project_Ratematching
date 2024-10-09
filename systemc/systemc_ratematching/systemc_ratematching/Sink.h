#ifndef SINK_H
#define SINK_H

#include <systemc.h>

SC_MODULE(Sink) {
public:
    sc_in<bool> valid_in;     // Valid signal from RateMatching
    sc_in<sc_lv<128>> data_in; // 128-bit input data
    sc_out<bool> ready_out;   // Ready signal to RateMatching

    sc_in<bool> clk;          // Clock
    sc_in<bool> rst;          // Reset

    // Constructor
    SC_CTOR(Sink) {
        SC_THREAD(sink_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

private:
    void sink_thread();
};

#endif