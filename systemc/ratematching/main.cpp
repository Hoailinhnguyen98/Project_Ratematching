#include "systemc.h"
#include "Ratematching.h"
#include "Source.h"
#include "Sink.h"

int sc_main(int argc, char* argv[]) {

    sc_signal<bool> valid_sig, ready_sig, valid_out_sig, ready_in_sig;
    sc_signal<sc_lv<128>> data_sig, data_out_sig;

    sc_signal<sc_lv<47>> config_bus_signal;

    // Clock and Reset
    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> rst;

    // Modules
    Source source("Source");
    Sink sink("Sink");
    RateMatching rate_matching("RateMatching");

    // Source connections
    source.valid_out(valid_sig);
    source.data_out(data_sig);
    source.ready_in(ready_sig);
    source.clk(clk);
    source.rst(rst);

    // RateMatching connections
    rate_matching.valid_in(valid_sig);
    rate_matching.ready_out(ready_sig);
    rate_matching.data_in(data_sig);

    rate_matching.valid_out(valid_out_sig);
    rate_matching.data_out(data_out_sig);
    rate_matching.ready_in(ready_in_sig);
    rate_matching.clk(clk);
    rate_matching.rst(rst);
    rate_matching.config_bus(config_bus_signal);

    // Sink connections
    sink.valid_in(valid_out_sig);
    sink.data_in(data_out_sig);
    sink.ready_out(ready_in_sig);
    sink.clk(clk);
    sink.rst(rst);

    // Start Simulation
    std::cout << "\nStarting simulation...\n" << std::endl;

    // Run simulation
    std::cout << "***************************************" << std::endl;
    std::cout << "\nRunning simulation...\n" << std::endl;
    sc_start(1000, SC_NS); // Run simulation

    return 0;
}