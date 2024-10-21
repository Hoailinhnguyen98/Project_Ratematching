#ifndef SINK_H
#define SINK_H

#include <systemc.h>

SC_MODULE(sink) {
public:
    sc_in<bool>         din_valid;     // Valid signal from RateMatching
    sc_in<sc_lv<128>>   din_data;   // 128-bit input data
    sc_in<bool>         din_last;   // Last signal from RateMatching
    sc_out<bool>        din_ready;   // Ready signal to RateMatching

    sc_in<bool> clk;          // Clock
    sc_in<bool> rst;          // Reset

    // Constructor
    SC_CTOR(sink) {
        SC_THREAD(sink_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

private:
    void sink_thread();
};

#endif