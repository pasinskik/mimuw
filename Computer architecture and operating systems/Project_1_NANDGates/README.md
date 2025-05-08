# NAND Gate Simulator (Dynamic Library)

This project implements a **dynamically loaded C library** for modeling boolean circuits made of **NAND gates**. It was developed as part of coursework in Computer Architecture and Operating Systems.

The library allows building a network of NAND gates and evaluating outputs, including computing the **critical path** length of a circuit.

---

## Features

- Dynamically allocated NAND gates
- Support for boolean signals and gate-to-gate connections
- Functions for:
  - Connecting and disconnecting inputs/outputs
  - Propagating signals through gates
  - Detecting cycles in the circuit
  - Calculating **output signals** and **critical paths**
- Defensive error handling using `errno`

---

## Example Usage

This is a C library intended to be compiled into a `.so` file and dynamically loaded by other C programs. To test the logic, you can link it with a test program or include `main()` for standalone testing.