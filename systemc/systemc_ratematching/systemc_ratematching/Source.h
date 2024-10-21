#ifndef SOURCE_H
#define SOURCE_H

#include "ratematching.h"

SC_MODULE(source) {
public:
    sc_in<bool>                 clk;            // Clock
    sc_in<bool>                 rst;            // Reset

    sc_out<sc_lv<128>>          dout_data;    // 128-bit output data
    sc_out<bool>                dout_valid;   // Valid signal from Source
    sc_out<bool>                dout_last;
    sc_in<bool>                 dout_ready;   // Ready signal from RateMatching

    sc_out<sc_lv<CFG_WIDTH>>    config_data;    // 47-bit configuration data
    sc_out<bool>                config_valid;   // Valid signal to RateMatching
    sc_in<bool>                 config_ready;   // Ready signal from RateMatching

    // Constructor
    SC_CTOR(source) {
        SC_THREAD(source_thread);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

private:
    void source_thread();
};

#endif