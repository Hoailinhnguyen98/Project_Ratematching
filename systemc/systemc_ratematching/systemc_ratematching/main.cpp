/*
 * ==============================================
 * File:        main.cpp
 * Author:      Nguyen Thi Hoai Linh
 * Email:       linhnth@hnsoc.one
 * Date Created:
 * Last Modified: 21 October 2024
 * Version:     1.0
 * Description: Testbench for the rate matching
 *              module with source and sink.
 * ==============================================
 */

#include "ratematching.h"
#include "source.h"
#include "sink.h"

int sc_main(int argc, char* argv[]) {

    // Signal declarations
    sc_signal<bool> din_valid, din_ready, dout_valid, dout_ready;
    sc_signal<bool> din_last, dout_last;
    sc_signal<sc_lv<128>> din_data, dout_data;

    sc_signal<sc_lv<CFG_WIDTH>> config_data;
    sc_signal<bool> config_valid, config_ready;

    // Clock and Reset signals
    sc_clock clk("clk", 10, SC_NS);  // 10ns clock period
    sc_signal<bool> rst;

    // Instantiate modules
    source source("source");
    sink sink("sink");
    ratematching rate_matching("ratematching");

    // Connect signals for Source module
    source.clk(clk);
    source.rst(rst);
    source.config_data(config_data);
    source.config_valid(config_valid);
    source.config_ready(config_ready);
    source.dout_data(din_data);
    source.dout_valid(din_valid);
    source.dout_ready(din_ready);
    source.dout_last(din_last);

    // Connect signals for RateMatching module
    rate_matching.clk(clk);
    rate_matching.rst(rst);
    rate_matching.config_data(config_data);
    rate_matching.config_valid(config_valid);
    rate_matching.config_ready(config_ready);
    rate_matching.din_data(din_data);
    rate_matching.din_valid(din_valid);
    rate_matching.din_ready(din_ready);
    rate_matching.din_last(din_last);  // Fixed din_last signal mapping
    rate_matching.dout_data(dout_data);
    rate_matching.dout_valid(dout_valid);
    rate_matching.dout_ready(dout_ready);
    rate_matching.dout_last(dout_last);

    // Connect signals for Sink module
    sink.clk(clk);
    sink.rst(rst);
    sink.din_ready(dout_ready);
    sink.din_valid(dout_valid);
    sink.din_data(dout_data);
    sink.din_last(dout_last);

    /* Trace for debugging */
    sc_trace_file* wave_form = sc_create_vcd_trace_file("tb_ratematching");
    sc_trace(wave_form, clk, "clk");
    sc_trace(wave_form, rst, "rst_n");
    sc_trace(wave_form, config_data, "cfg_data");
    sc_trace(wave_form, config_valid, "cfg_vld");
    sc_trace(wave_form, config_ready, "cfg_rdy");
    sc_trace(wave_form, din_data, "input_data");
    sc_trace(wave_form, din_valid, "input_vld");
    sc_trace(wave_form, din_ready, "input_rdy");
    sc_trace(wave_form, din_last, "input_last");
    sc_trace(wave_form, dout_data, "output_data");
    sc_trace(wave_form, dout_valid, "output_vld");
    sc_trace(wave_form, dout_ready, "output_rdy");
    sc_trace(wave_form, dout_last, "output_last");

    // Start Simulation
    std::cout << "\nStarting simulation...\n" << std::endl;
    sc_start(1000, SC_NS); // Run simulation

    // End simulation
    sc_close_vcd_trace_file(wave_form);
    return 0;
}