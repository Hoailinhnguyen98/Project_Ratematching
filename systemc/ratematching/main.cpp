// ----------------------------------------------------------------------------
// Title:    : 
// Project   : 
// Filename  : main
// ----------------------------------------------------------------------------
// Author    : Nguyen Thi Hoai Linh
// Email     : linhnth@soc.one
// Date      : 2024-10-09 23:41:35
// Last Modified : 2024-10-10 18:23:34
// Modified By   : Nguyen Canh Trung
// ----------------------------------------------------------------------------
// Description: 
// 
// ----------------------------------------------------------------------------
// HISTORY:
// Date      	By	Comments
// ----------	---	---------------------------------------------------------
// 2024-10-10	NCT	File created!
// ----------------------------------------------------------------------------
#include "systemc.h"
#include "ratematching.h"
#include "Source.h"
#include "Sink.h"

int sc_main(int argc, char* argv[]) {

    sc_signal<bool> din_valid, din_ready, dout_valid, dout_ready;
    sc_signal<sc_lv<128>> din_data, dout_data;

    sc_signal<sc_lv<CFG_WIDTH>> config_data;
    sc_signal<bool> config_valid, config_ready;

    // Clock and Reset
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> rst;

    // Modules
    Source source("Source");
    Sink sink("Sink");
    RateMatching rate_matching("RateMatching");

    // Source connections
    source.clk(clk);
    source.rst(rst);

    source.config_data(config_data);
    source.config_valid(config_valid);
    source.config_ready(config_ready);

    source.source_data(din_data);
    source.source_valid(din_valid);
    source.source_ready(din_ready);

    // RateMatching connections
    rate_matching.clk(clk);
    rate_matching.rst(rst);

    rate_matching.config_data(config_data);
    rate_matching.config_valid(config_valid);
    rate_matching.config_ready(config_ready);

    rate_matching.din_data(din_data);
    rate_matching.din_valid(din_valid);
    rate_matching.din_ready(din_ready);

    rate_matching.dout_data(dout_data);
    rate_matching.dout_valid(dout_valid);
    rate_matching.dout_ready(dout_ready);

    // Sink connections
    sink.clk(clk);
    sink.rst(rst);
    sink.ready_out(dout_ready);
    sink.valid_in(dout_valid);
    sink.data_in(dout_data);

    /* Debugging/Trace methods      */
    sc_trace_file *wave_form = sc_create_vcd_trace_file("tb");
    sc_trace(wave_form, clk, "clk");
    sc_trace(wave_form, rst, "rst_n");
    sc_trace(wave_form, config_data, "cfg_dat");
    sc_trace(wave_form, config_valid, "cfg_vld");
    sc_trace(wave_form, config_ready, "cfg_rdy");
    sc_trace(wave_form, din_data, "input_dat");
    sc_trace(wave_form, din_valid, "input_vld");
    sc_trace(wave_form, din_ready, "input_rdy");
    sc_trace(wave_form, dout_data, "output_dat");
    sc_trace(wave_form, dout_valid, "output_vld");
    sc_trace(wave_form, dout_ready, "output_rdy");

    // Start Simulation
    std::cout << "\nStarting simulation...\n" << std::endl;

    // Run simulation
    std::cout << "***************************************" << std::endl;
    std::cout << "\nRunning simulation...\n" << std::endl;
    sc_start(1000, SC_NS); // Run simulation

    return 0;
}