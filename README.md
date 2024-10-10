
# Project Ratematching

This project implements a rate-matching function using SystemC. The rate-matching function is crucial in communication systems to ensure that the data rate of the transmitted signal matches the data rate of the channel.

## Hierarchy

+ `io` : Includes input, output, and configuration data.
+ `matlab` : Program generates test cases for testing the rate-matching function.
+ `systemc` : C++ program simulates the rate-matching function.


## AXIS convention

```cpp
// AXIS signals
<port name>_data
<port name>_valid
<port name>_ready
<port name>_last
```

## Prerequisite

1. Ensure you have SystemC installed on your system.
    ```
    systemc-2.3.3
    ```

If you have not installed systemC library yet. You can run

    ```
    make systemc
    ```

It will install the systemC library at `./lib`

2. Change the environment `SYSTEMC_HOME` 

In the file `./systemc/scripts/library.mk` 

```sh
SYSTEMC_HOME ?=/opt/systemc/systemc-2.3.3
```

Please change the path to your local systemc lib path.

## Building the Project

To build the project, follow these steps:

1. Navigate to the project directory:
    ```sh
    cd ./systemc/ratematching
    ```

2. Compile the SystemC code:
    ```sh
    make all
    ```

## Running the Code

To run the code, execute the following command in the project directory:
```sh
make run
```
This will run the rate-matching simulation and output the results to the console.

## View the waveform

```sh
make view
```

Note that: you need to install `gtkwave` in your system to be able to view the waveform
